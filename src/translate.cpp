#include "translate.hpp"
#include "normalize.hpp"
#include "type_helpers.hpp"
#include "util_string.hpp"

#include "TSystem.h"
#include "TClass.h"
#include "TBaseClass.h"
#include "TMethod.h"
#include "TMethodArg.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <set>
#include <queue>
#include <algorithm>
#include <regex>
#include <ostream>

using namespace std;

int _g_no_name_arg_index = 0;

set<string> reserved_words = {"from"};

///
// Translate a method argument.
method_arg translate_argument(const TMethodArg *arg){
    auto result = method_arg();

	result.name = arg->GetName();
	result.raw_typename = arg->GetTypeName();
	result.full_typename = arg->GetFullTypeName();

    // Some arguments have no name to them
    if (result.name.size() == 0) {
        if (_g_no_name_arg_index == 0) {
            result.name = "noname_arg";
        } else {
            ostringstream arg_name;
            arg_name << "noname_arg_" << _g_no_name_arg_index;
            result.name = arg_name.str();
        }
        _g_no_name_arg_index++;
    } else if (reserved_words.find(result.name) != reserved_words.end()) {
        result.name += "_arg";
    }

    return result;
}

///
// Translate a method
method_info translate_method(TMethod *method) {
    method_info m;

    // Get the method name
    m.name = method->GetName();

    // Get the method return type
    m.return_type = method->GetReturnTypeName();
    if (m.return_type == "void") {
        m.return_type = "";
    }

    // The arguments for the method
    auto l = method->GetListOfMethodArgs();
    _g_no_name_arg_index = 0;
	for (int i_arg = 0; i_arg < l->GetSize(); i_arg++) {
        m.arguments.push_back(translate_argument(static_cast<TMethodArg*> (l->At(i_arg))));
	}

    return m;
}

bool is_good_method(const TClass *c_info, const TMethod *m_info, const set<string> &inherited_classes) {
    if (string(m_info->GetName()) == string(c_info->GetName())) {
        return false;
    }
    
    if (m_info->GetName()[0] == '~') {
        return false;
    }

    if (string(m_info->GetName()).rfind("operator", 0) == 0) {
        return false;
    }
    
    if (inherited_classes.find(m_info->GetName()) != inherited_classes.end()) {
        return false;
    }

    return true;
}

///
// Get all publically directly inherited classes
vector<string> inherited_public_classes(TClass *c_info) {
    auto inherited_list = c_info->GetListOfBases();
    TIter next(inherited_list);
    vector<string> result;
    while (auto bobj = static_cast<TBaseClass *>(next()))
    {
        // Do not grab private or protected inheritance. Only the public
        // interface for us.
        if (
            ((bobj->Property() & kIsPrivate) == 0) && ((bobj->Property() & kIsProtected) == 0))
        {
            auto cl = bobj->GetClassPointer();
            result.push_back(cl->GetName());
        }
    }
    return result;
}

vector<string> inherited_public_classes(const std::string &cls_name) {
    auto c_info = TClass::GetClass(cls_name.c_str());
    return inherited_public_classes(c_info);
}

///
/// Find all inherited classes
///
set<string> all_inherited_classes(const std::string &cls_name) {
    set<string> result;
    queue<string> to_do;
    to_do.push(cls_name);

    while (to_do.size() > 0) {
        auto top_class(to_do.front());
        to_do.pop();
        if (result.find(top_class) != result.end())
            continue;
        result.insert(top_class);

        auto in_names = inherited_public_classes(top_class);
        for (auto &&i_name : in_names)
        {
            if (i_name.size() > 0) {
                to_do.push(i_name);
            }
        }
    }

    return result;
}

string clean_so_name(string original_name)
{
    string result(trim(original_name));
    result = remove_prefix(result, "lib");
    result = remove_postfix(result, "Dict.so");
    return result;
}

// Return the include file name for a particular class. Using some
// heuristics to get it right.
string get_include_file_for_class(const string &class_name)
{
    auto c_info = TClass::GetClass(class_name.c_str());
    if (c_info == nullptr) {
        return "";
    }
    if ((c_info->GetDeclFileName() != nullptr) && c_info->GetSharedLibs() != nullptr) {
        return string(clean_so_name(c_info->GetSharedLibs())) + "/" + c_info->GetDeclFileName();
    }
    return "";
}

// Take the interior object, and rename as a container
string get_include_file_for_container(const string &class_name, const string &raw_object_name)
{
    auto object_name = std::regex_replace(raw_object_name, std::regex("_v[0-9]+$"), "");
    auto parsed_info = parse_typename(object_name);

    auto c_info = TClass::GetClass(class_name.c_str());
    if (c_info == nullptr) {
        return "";
    }
    return string(clean_so_name(c_info->GetSharedLibs())) + "/" + parsed_info.type_name + "Container.h";
}

bool include_file_exists(const string &include_path) {
    string full = "$ROOTCOREDIR/include/" + include_path;
    string expanded = gSystem->ExpandPathName(full.c_str());
    return !gSystem->AccessPathName(expanded.c_str(), kFileExists);
}

class_info translate_class(const std::string &class_name)
{
    // Get the class
    class_info result;
    auto unq_class_name = unqualified_type_name(class_name);
    auto c_info = TClass::GetClass(unq_class_name.c_str());
    if (c_info == nullptr)
    {
        std::cerr << "ERROR: Cannot translate class '" << class_name << "': ROOT's type system doesn't have it loaded." << std::endl;
        return result;
    }
    // There are several other types of classes we do not need to translate as well.
    string name = c_info->GetName();
    auto t = parse_typename(name);
    if (t.type_name.substr(0,2) == "__") {
        cerr << "INFO: Not translating '" << class_name << "' as it is a private internal class" << endl;
        return result;
    }

    result.name = name;
    result.name_as_type = t;
    if (t.type_name == "vector") {
        // We do very simple things with vector.
        method_info size_method;
        size_method.name = "size";
        size_method.return_type = "size_t";
        result.methods.push_back(size_method);
        return result;
    }


    // Library is just the clean so name for us
    if (c_info->GetSharedLibs() != nullptr) {
        result.library_name = clean_so_name(c_info->GetSharedLibs());
    }

    // Get all inherited classes
    auto all_bases = inherited_public_classes(c_info);
    for (auto &&b : all_bases)
    {
        result.inherited_class_names.push_back(b);
    }

    // Look at all public methods
    // TODO: We totally ignore the fact that methods can have different calls depending on the arguments
    //       given. For now, we demand a single method, and just use the first one.
    {
        set<string> seen_names;
        auto all_inherited = type_name(all_inherited_classes(unq_class_name));
        auto all_methods = c_info->GetListOfAllPublicMethods();
        TIter next(all_methods);
        while (auto method = static_cast<TMethod *>(next.Next()))
        {
            if (seen_names.find(method->GetName()) == seen_names.end()) {
                if (is_good_method(c_info, method, all_inherited)) {
                    result.methods.push_back(translate_method(method));
                }
            }
            seen_names.insert(method->GetName());
        }
    }

    // Get include files associated with this class. This is quite messy, actually, because of the way
    // the modern root records where things are located, adn the fact we are dealing with typedef's.
    // In short - we have to use heuristics.
    string include;
    if ((include == "")) {
        auto dv_info = get_first_class(result, "DataVector");
        if (dv_info.nickname.size() > 0) {
            include = get_include_file_for_container(class_name, dv_info.nickname);
            if (include.size() > 0 && !include_file_exists(include)) {
                include = "";
            }
        }
    }
    if (include == "") {
        include = get_include_file_for_class(class_name);
        if (!include_file_exists(include)) {
            include = "";
        }
    }
    result.include_file = include;

    return result;
}