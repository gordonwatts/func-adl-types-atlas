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

// Find all classes that inherit from a given class name
vector<string> all_that_inherit_from(const string &c_name)
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
            cout << "    *** inherits!" << endl;
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

map<string, string> g_typedef_map;

void build_typedef_map() {
    if (g_typedef_map.size() > 0)
        return;

    // Build the forward map just once
	TIter i_typedef (gROOT->GetListOfTypes(true));
	int junk = gROOT->GetListOfTypes()->GetEntries();
	TDataType *typedef_spec;
	while ((typedef_spec = static_cast<TDataType*>(i_typedef.Next())) != 0)
	{
		string typedef_name = typedef_spec->GetName();
		string base_name = typedef_spec->GetFullTypeName();

        if (typedef_name != base_name) {
            g_typedef_map[typedef_name] = base_name;
        }
    }

    // Add a few special ones to keep the system working
    g_typedef_map["ULong64_t"] = "unsigned long long";
    g_typedef_map["uint32_t"] = "unsigned int";
}

// From typedefs, return resolved typedefs.
// Do not call until all libraries have been loaded!
string resolve_typedef(const string &c_name) {
    build_typedef_map();
    string result = c_name;
    bool done = false;
    while (!done) {
        auto td = g_typedef_map.find(result);
        if (td == g_typedef_map.end()) {
            done = true;
        } else {
            result = td->second;
        }
    }
    return result;
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

// Find referenced arguments in methods and resolve any typedefs in there
void fixup_type_defs(vector<class_info> &classes)
{    
    // Loop through all the classes we are looking at to see if there is an alias we should be done.
    for (auto &&c : classes)
    {
        for (auto &&m : c.methods)
        {
            m.return_type = resolve_typedef(m.return_type);
            for (auto &&a : m.arguments)
            {
                a.full_typename = resolve_typedef(a.full_typename);
            }            
        }
    }
}

// Parse a horrendous typename into its various pieces.
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

// Returns the type info for the first class or inherited class that
// has name as the name as a class. Can't climb the inheritance tree far,
// but it will try.
typename_info get_first_class(const class_info &c, const string &name) {
    // Check the class itself
    if (c.name_as_type.type_name == name) {
        return c.name_as_type.template_arguments[0];
    }

    // Go after the inherited item
    for (auto &&i_name : c.inherited_class_names)
    {
        auto t_info = parse_typename(i_name);
        if (t_info.type_name ==  name) {
            return t_info.template_arguments[0];
        }
    }

    return typename_info();
}

// Dump out the typename as fully qualified, but normalized
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

// Figure out if this type is a container that can be
// iterated over using standard for like C++ syntax
// (vector, etc.)
bool is_collection(const typename_info &ti) {
    if (ti.type_name == "vector" & ti.template_arguments.size() > 0) {
        return true;
    }
    return false;
}

typename_info container_of(const typename_info &ti) {
    throw runtime_error("ops");
}

// Look at the class see if this is a vector of some sort that
// can be iterated over.
bool is_collection(const class_info &ci) {
    // Look to see if there is a begin/end method. If so, then we will
    // assume that is good to go!

    if (has_methods(ci, {"begin", "end"})) {
        return true;
    }
    return false;
}

map<string, string> _g_container_iterator_specials = {
    {"TIter", "TObject"},
    {"xAOD::JetConstituentVector::iterator", "xAOD::JetConstituent"}
};

// Given this is a container, as above, figure out
// what it is containing.
typename_info container_of(const class_info &ci) {
    // If there is a begin/end object, lets lift that out.
    if (has_methods(ci, {"begin", "end"})) {
        auto rtn_type_name = get_method(ci, "begin")[0].return_type;
        auto rtn_type = _g_container_iterator_specials.find(rtn_type_name);
        if (rtn_type == _g_container_iterator_specials.end()) {
            throw runtime_error("Unable to find container type for iterator type " + rtn_type_name);
        }
        return parse_typename(rtn_type->second);
    }

    throw runtime_error("Do not know how to find container type for class " + ci.name);
}
