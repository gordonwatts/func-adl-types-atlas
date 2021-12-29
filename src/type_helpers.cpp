#include "type_helpers.hpp"
#include "class_info.hpp"
#include "util_string.hpp"

#include "TROOT.h"
#include "TClassTable.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <boost/algorithm/string.hpp>

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
    auto t = parse_typename(full_type_name);
    return unqualified_typename(t);
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
    // Check the typedef name
    build_typedef_map();
    auto t = parse_typename(c_name);
    if (t.type_name == "") {
        return "";
    }

    // If it is size_t then we are going to take a shortcut
    // For whatever reason, ROOT never seems to know about this,
    // and this is a strictly C++ type.
    if (t.type_name == "size_t") {
        return "int";
    }


    string result = unqualified_typename(t);
    bool done = false;
    while (!done) {
        auto td = g_typedef_map.find(result);
        if (td == g_typedef_map.end()) {
            done = true;
        } else {
            result = td->second;
        }
    }

    // Last is to normalize, if possible, with a class name
    auto c = TClass::GetClass(result.c_str());
    if (c != nullptr) {
        result = c->GetName();
    }

    // Reapply the various modifiers
    if (t.is_const) {
        result = "const " + result;
    }
    if (t.is_pointer) {
        result = result + "*";
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
    result.is_const = false;
    result.is_pointer = false;
    result.nickname = trim(type_name);
    bool top_level_is_const = false;

    // Simple bail if this is a blank.
    if (result.nickname.size() == 0) {
        return result;
    }

    // Walk through, parsing.
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

            case '*':
                if (ns_depth == 0) {
                    result.is_pointer = true;
                } else {
                    name += type_name[t_index];
                }
                break;
            
            case ' ':
                if (ns_depth == 0) {
                    auto n1 = boost::trim_copy(name);
                    if (n1 == "const") {
                        top_level_is_const = true;
                        name = "";
                    } else {
                        name += type_name[t_index];
                    }
                } else {
                    name += type_name[t_index];
                }
            
            case '&':
                // We don't care about reference modifiers for this work.
                if (ns_depth != 0) {
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
        boost::trim(name);
        result.type_name = name;
        name = "";
    }

    result.is_const = top_level_is_const;

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
    string result(full_name.str());

    // Iterable should not be touched as it is understood as a python template
    // (the only template that is understood).
    if (result.substr(0,9) == "Iterable[") {
        return result;
    }

    // Walk through the rest sensibly converting the typename.
    int bracket_depth = 0;
    for(int i = 0; i < result.size(); i++) {
        switch (result[i])
        {
        case '[':
            result[i] = '_';
            bracket_depth++;
            break;
        case ']':
            result[i] = '_';
            bracket_depth--;
            break;
        
        case '.':
            if (bracket_depth > 0) {
                result[i] = '_';
            }
            break;

        default:
            break;
        }
    }
    replace(result.begin(), result.end(), '[', '_');
    replace(result.begin(), result.end(), ']', '_');
    return result;
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

// Look at the class see if this is a vector of some sort that
// can be iterated over. We also only return true if we are sure we can get
// an actual type out of (in short - container_of will return successfully).
bool is_collection(const class_info &ci) {
    // Look to see if there is a begin/end method. If so, then we will
    // assume that is good to go!

    try {
        container_of(ci);
        return true;
    } catch (std::runtime_error &e) {
        return false;
    }
}

map<string, string> _g_container_iterator_specials = {
    {"TIter", "TObject"},
    {"xAOD::JetConstituentVector::iterator", "xAOD::JetConstituent"},
};

typename_info container_of(const typename_info &ti) {
    throw runtime_error("ops");
}

// Given this is a container, as above, figure out
// what it is containing.
typename_info container_of(const class_info &ci) {
    // If this is a vector object, then we can grab from the argument
    if (ci.name_as_type.type_name == "vector") {
        return ci.name_as_type.template_arguments[0];
    }

    // If there is a begin/end object, lets lift that out.
    if (has_methods(ci, {"begin", "end"})) {
        auto rtn_type_name = get_method(ci, "begin")[0].return_type;
        auto rtn_type = _g_container_iterator_specials.find(rtn_type_name);
        if (rtn_type == _g_container_iterator_specials.end()) {
            throw runtime_error("Unable to find container type for iterator type " + rtn_type_name + " for container " + ci.name);
        }
        return parse_typename(rtn_type->second);
    }

    throw runtime_error("Do not know how to find container type for class " + ci.name);
}

// Return the C++ type as unqualified.
std::string unqualified_typename(const typename_info &ti)
{
    typename_info n_ti = ti;
    n_ti.is_const = false;
    n_ti.is_pointer = false;
    return typename_cpp_string(n_ti);
}

// Return the C++ type in a standard format
std::string typename_cpp_string(const typename_info &ti)
{
    bool first = true;
    ostringstream stream;

    if (ti.is_const) {
        stream << "const ";
    }

    for (auto &&ns : ti.namespace_list)
    {
        if (!first)
            stream << "::";
        first = false;
        stream << typename_cpp_string(ns);
    }
    if (!first)
        stream << "::";

    stream << ti.type_name;

    // And any template arguments
    first = true;
    for (auto &&t_arg : ti.template_arguments)
    {
        if (first) {
            stream << "<";
            first = false;
        } else {
            stream << ", ";
        }
        stream << typename_cpp_string(t_arg);
    }
    if (!first) {
        stream << ">";
    }

    if (ti.is_pointer) {
        stream << " *";
    }

    return stream.str();
}

set<string> _known_templates({
    "vector",
    // "ElementLink",
    // "DataVector",
});

// See if we can handle this type:
// Raw types in the known list are ok
// Vectors of known types are ok
// Element links of known types are ok
// DataVectors of known types are ok.
bool is_understood_type(const string &t_name, const set<std::string> &known_types)
{
    // First, check to see if the type is in the list of known types
    auto t = parse_typename(t_name);
    return is_understood_type(t, known_types);
}

bool is_understood_type(const typename_info &t, const set<std::string> &known_types)
{
    // First, check to see if the type is in the list of known types
    auto uq_name = unqualified_typename(t);
    if (known_types.find(uq_name) != known_types.end()) {
        return true;
    }

    return false;
}

// Convert C++ types into python types for return
typename_info py_typename(const string &t_name){
    return py_typename(parse_typename(t_name));
}

// Convert C++ types into python types, with some special
// handling.
typename_info py_typename(const typename_info &t)
{
    // If the type is either vector or DataVector, we
    // just convert it to Iterable.
    if (t.type_name == "vector" || t.type_name == "DataVector") {
        typename_info result(t);
        result.type_name = "Iterable";
        result.template_arguments[0] = py_typename(t.template_arguments[0]);
        result.nickname = typename_cpp_string(result);
        return result;
    }

    // If this is an element link, then we need to conver this to a pointer.
    // Element links can only be taken of data vectors, and we are interested in their
    // inside type.
    if (t.type_name == "ElementLink") {
        if (t.template_arguments[0].type_name != "DataVector") {
            throw runtime_error("Found element link type, but not of a DataVector: '" + t.nickname + "'.");
        }
        // Lift from twice deep - this is a link to a datavector.
        typename_info result = py_typename(t.template_arguments[0].template_arguments[0]);
        result.is_pointer = true;
        result.nickname = typename_cpp_string(result);
        return result;
    }
    return t;
}
