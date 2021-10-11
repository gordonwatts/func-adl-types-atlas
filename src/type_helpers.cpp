#include "type_helpers.hpp"
#include "class_info.hpp"
#include "util_string.hpp"

#include "TROOT.h"
#include "TClassTable.h"

#include <algorithm>
#include <iterator>
#include <sstream>

using namespace std;

///
// Return a type name without any qualifiers
//
string type_name(const string &qualified_type_name)
{
    string result(qualified_type_name);
    auto i = result.rfind(':');
    if (i != std::string::npos) {
        result = result.substr(i+1);
    }
    return result;
}

///
// Take list of types names and clean them up
//
vector<string> type_name(const vector<std::string> &qualified_type_names)
{
    vector<string> result;
    transform(qualified_type_names.begin(), qualified_type_names.end(),
                back_insert_iterator(result),
                [](const string &a) {return type_name(a);});
    return result;
}

set<string> type_name(const set<string> &qualified_type_names)
{
    auto v = type_name(vector<string>(qualified_type_names.begin(), qualified_type_names.end()));
    return set<string>(v.begin(), v.end());
}

// Strip out leading "const" and post modifiers (like ptr, etc.)
string unqualified_type_name(const string &full_type_name)
{
    string result(full_type_name);

    // Remove leading const
    if (result.find("const ") != result.npos) {
        result = result.substr(6);
    }

    // Remove trailing references or pointers
    bool found = true;
    while (found) {
        auto last_char = result[result.size()-1];
        found = (last_char == '*') || (last_char == '&');
        if (found) {
            result = result.substr(0, result.size()-1);
        }
    }

    return result;
}

// Find all classes that inherrit from a given class name
vector<string> all_that_inherrit_from(const string &c_name)
{
    TClassTable::Init();
    while (auto m_info = TClassTable::Next()) {
        cout << "** " << m_info << endl;
    }

    TIter next(gROOT->GetListOfClasses());
    set<string> results;
    while (auto c_info = static_cast<TClass *>(next()))
    {
        cout << " -> " << c_info->GetName() << endl;
        if (c_info->InheritsFrom(c_name.c_str()))
        {
            results.insert(c_info->GetName());
            cout << "    *** Inherrits!" << endl;
        }
    }
    return vector<string>(results.begin(), results.end());
}

// Build a list of type mapping
map<string, vector<string>> root_typedef_map()
{
    // Build a typedef backwards mapping
	TIter i_typedef (gROOT->GetListOfTypes(true));
	int junk = gROOT->GetListOfTypes()->GetEntries();
	TDataType *typedef_spec;
    map<string, vector<string>> typdef_back_map;
	while ((typedef_spec = static_cast<TDataType*>(i_typedef.Next())) != 0)
	{
		string typedef_name = typedef_spec->GetName();
		string base_name = typedef_spec->GetFullTypeName();

        if (typedef_name != base_name) {
            typdef_back_map[base_name].push_back(typedef_name);
        }
    }
    return typdef_back_map;
}

///
// Look through the list of typedefs, and add aliases for any classes
// we've already seen.
//
void fixup_type_aliases(vector<class_info> &classes)
{    
    // Build a typedef backwards mapping
    map<string, vector<string>> typdef_back_map = root_typedef_map();

    // Loop through all the classes we are looking at to see if there is an alias we should be done.
    for (auto &&c : classes)
    {
        if (typdef_back_map.find(c.name) != typdef_back_map.end()) {
            c.aliases = typdef_back_map[c.name];
        }
    }
}

// Parse a horrendus typename into its various peices.
//
// "int"
// class_name::size_type
// class_name<t1,t2>::class_name2<t3, t4>::size_type
typename_info parse_typename(const string &type_name)
{
    typename_info result;
    result.nickname = trim(type_name);

    int ns_depth = 0;
    size_t t_index = 0;
    string name;
    while (t_index < type_name.size()) {
        switch (type_name[t_index]) {
            case '<':
                if (ns_depth == 0) {
                    result.type_name = name;
                    name = "";
                } else {
                    name += type_name[t_index];
                }
                ns_depth++;
                break;
            
            case '>':
                ns_depth--;
                if (ns_depth == 0) {
                    result.template_arguments.push_back(parse_typename(name));
                    name = "";
                } else {
                    name += type_name[t_index];
                }
                break;
            
            case ',':
                if (ns_depth == 1) {
                    result.template_arguments.push_back(parse_typename(name));
                    name = "";
                } else {
                    name += type_name[t_index];
                }
                break;

            case ':':
                if (ns_depth == 0) {
                    if (name.size() > 0) {
                        result.namespace_list.push_back(parse_typename(name));
                        name = "";
                    } else {
                        typename_info nested_ns = result;
                        result = typename_info();

                        result.nickname = nested_ns.nickname;
                        nested_ns.nickname = nested_ns.nickname.substr(0, t_index);

                        result.namespace_list.push_back(nested_ns);
                    }
                    t_index++; // Get past the double colon
                } else {
                    name += type_name[t_index];
                }
                break;

            default:
                name += type_name[t_index];
                break;
        }
        t_index++;
    }
    if (name.size() > 0 && result.type_name.size() == 0) {
        result.type_name = name;
        name = "";
    }

    return result;
}

// Returns the type info for the first class or inherrited class that
// has name as the name as a class. Can't climb the inherritance tree far,
// but it will try.
typename_info get_first_class(const class_info &c, const string &name) {
    // Check the class itself
    if (c.name_as_type.type_name == name) {
        return c.name_as_type.template_arguments[0];
    }

    // Go after the inherrited item
    for (auto &&i_name : c.inherrited_class_names)
    {
        auto t_info = parse_typename(i_name);
        if (t_info.type_name ==  name) {
            return t_info.template_arguments[0];
        }
    }

    return typename_info();
}

// Dump out the typename as fully qualified, but normalzied
// (rather than just C++).
string normalized_type_name(const typename_info &ti)
{
    ostringstream full_name;
    full_name << ti;
    return full_name.str();
}

// Return a C++ type that has been normalized.
string normalized_type_name(const string &ti)
{
    return normalized_type_name(parse_typename(ti));
}
