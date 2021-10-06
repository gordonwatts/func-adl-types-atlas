#include "translate.hpp"
#include "normalize.hpp"
#include "type_helpers.hpp"

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

using namespace std;


///
// Translate a method argument.
method_arg translate_argument(const TMethodArg *arg){
    auto result = method_arg();

	result.name = arg->GetName();
	result.raw_typename = arg->GetTypeName();
	result.full_typename = arg->GetFullTypeName();

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
	for (int i_arg = 0; i_arg < l->GetSize(); i_arg++) {
        m.arguments.push_back(translate_argument(static_cast<TMethodArg*> (l->At(i_arg))));
	}

    return m;
}


bool is_good_method(const TClass *c_info, const TMethod *m_info, const set<string> &inherited_classes) {
    if (string(m_info->GetName()) == string(c_info->GetName()))
        return false;
    
    if (m_info->GetName()[0] == '~')
        return false;

    if (string(m_info->GetName()).rfind("operator", 0) == 0)
        return false;
    
    if (inherited_classes.find(m_info->GetName()) != inherited_classes.end())
        return false;

    return true;
}


///
// Get all publically directly inherrited classes
vector<string> inherited_public_classes(const std::string &cls_name) {
    auto c_info = TClass::GetClass(cls_name.c_str());

    auto inherited_list = c_info->GetListOfBases();
    TIter next(inherited_list);
    vector<string> result;
    while (auto bobj = static_cast<TBaseClass *>(next()))
    {
        // Do not grab private or protected inherritance. Only the public
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

///
/// Find all inherrited classes
///
set<string> all_inherrited_classes(const std::string &cls_name) {
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


class_info translate_class(const std::string &class_name)
{
    // Get the class
    auto c_info = TClass::GetClass(class_name.c_str());
    if (c_info == nullptr)
    {
        std::cerr << "ERROR: Cannot translate class " << class_name << ": ROOT's type system doesn't have it loaded." << std::endl;
        return class_info();
    }
    class_info result;
    result.name = c_info->GetName();
    result.name_as_type = parse_typename(result.name);

    // Look to inherrited classes
    auto inherited_list = c_info->GetListOfBases();
    TIter next(inherited_list);
    while (auto bobj = static_cast<TBaseClass *>(next()))
    {
        // Do not grab private or protected inherritance. Only the public
        // interface for us.
        for (auto &&m_name : inherited_public_classes(c_info->GetName()))
        {
            result.inherrited_class_names.push_back(m_name);            
        }
    }

    // Look at all public methods
    {
        auto all_inherited = type_name(all_inherrited_classes(result.name));
        auto all_methods = c_info->GetListOfAllPublicMethods();
        TIter next(all_methods);
        while (auto method = static_cast<TMethod *>(next.Next()))
        {
            if (is_good_method(c_info, method, all_inherited)) {
                result.methods.push_back(translate_method(method));
            }
        }
    }

    return result;
}