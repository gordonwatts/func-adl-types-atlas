#include "class_info.hpp"
#include "type_helpers.hpp"

#include <vector>
#include <algorithm>
#include <set>

using namespace std;

std::ostream& operator <<(std::ostream& stream, const class_info& ci) {
    stream << "Class: " << ci.name << endl;
    stream << "  Type: " << ci.name_as_type << endl;

    if (ci.aliases.size() > 0) {
        stream << "  Aliases: ";
        for (auto &&an : ci.aliases)
        {
            stream << an << " ";
        }
        stream << endl;
    }

    if (ci.inherited_class_names.size() > 0) {
        stream << "  inherits from:" << endl;
        for (auto &&i_name : ci.inherited_class_names)
        {
            stream << "    " << i_name << endl;
        }
        if (ci.methods.size() > 0) {
            stream << "  Methods:" << endl;
            for (auto &&i_method : ci.methods)
            {
                stream << i_method;
            }
        }
    }
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const method_info& mi)
{
    stream << "    Method: " << mi.name << endl;
    if (mi.return_type.size() > 0) {
        stream << "      Return Type: " << mi.return_type << endl;
    }

    for (auto &&a : mi.arguments)
    {
        stream << "      Arg: " << a << endl;
    }

    return stream;
}

std::ostream& operator <<(std::ostream& stream, const method_arg& ai)
{
    stream << ai.full_typename << " " << ai.name;
    return stream;
}

map<string, string> python_type_conversions =
    {
        {"double", "float"},
        {"unsigned int", "int"},
        {"short", "int"},
        {"unsigned short", "int"},
        {"long", "int"},
        {"unsigned long", "int"},
        {"long long", "int"},
        {"unsigned long long", "int"},
        {"size_t", "int"},
        {"string", "str"},
    };

// Reconstruct the full type name
std::ostream& operator <<(std::ostream& stream, const typename_info& ti)
{
    bool first = true;
    for (auto &&ns : ti.namespace_list)
    {
        if (!first)
            stream << ".";
        first = false;
        stream << ns;
    }
    if (!first)
        stream << ".";

    // Some typenames get a conversion, others not-so-much
    auto type_convert = python_type_conversions.find(ti.type_name);
    if (type_convert != python_type_conversions.end()) {
        stream << type_convert->second;
    } else {
        stream << ti.type_name;
    }

    // And any template arguments
    first = true;
    for (auto &&t_arg : ti.template_arguments)
    {
        if (first) {
            stream << "[";
            first = false;
        } else {
            stream << ",";
        }
        stream << t_arg;
    }
    if (!first) {
        stream << "]";
    }
    return stream;
}

///
// Find all classes that this class references.
std::vector<std::string> referenced_types(const class_info &c_info)
{
    set<string> result(c_info.inherited_class_names.begin(), c_info.inherited_class_names.end());

    auto named_types = referenced_types(c_info.name_as_type);
    result.insert(named_types.begin(), named_types.end());

    for (auto &&m : c_info.methods)
    {
        auto types = referenced_types(m);
        result.insert(types.begin(), types.end());
    }

    return vector<string>(result.begin(), result.end());
}

///
// Find all types that are referenced in this method.
std::vector<std::string> referenced_types(const method_info &m_info)
{
    set<string> result;

    // Take care of the return types.
    if (m_info.return_type.size() > 0) {
        auto ti = parse_typename(m_info.return_type);
        auto refed_types = referenced_types(ti);
        result.insert(refed_types.begin(), refed_types.end());
    }

    // And the arguments
    for (auto &&a : m_info.arguments)
    {
        auto ti = parse_typename(a.full_typename);
        auto refed_types = referenced_types(ti);
        result.insert(refed_types.begin(), refed_types.end());
    }
    

    return vector<string>(result.begin(), result.end());
}

// This type might refer to several different classes. Dig them out.
std::vector<std::string> referenced_types(const typename_info &t_info)
{
    set<string> result;

    // We do not look at the namespace arguments since they are
    // qualifiers and not actually referenced.

    // Special case the ElementLink. Turns out the DataVector type is
    // not important for references or resolution.
    if (t_info.type_name == "ElementLink" && t_info.template_arguments[0].type_name == "DataVector") {
        auto new_types = referenced_types(t_info.template_arguments[0].template_arguments[0]);        
        result.insert(new_types.begin(), new_types.end());
    } else {
        for (auto &&ti : t_info.template_arguments)
        {
            auto new_types = referenced_types(ti);
            result.insert(new_types.begin(), new_types.end());
        }
    }

    // And the top level name, but make sure it is "legal"
    if (!all_of(t_info.nickname.begin(), t_info.nickname.end(), ::isdigit)
        && (t_info.nickname[0] != '-')
        )
        result.insert(unqualified_typename(t_info));

    // Return as a list.
    return vector<string>(result.begin(), result.end());
}

// Make sure all listed methods are part of this class
bool has_methods(const class_info &ci, const vector<string> &names)
{
    for (auto &&m_name : names)
    {
        bool found = false;
        for (auto &&m : ci.methods)
        {
            if (m.name == m_name) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }        
    }
    
    return true;
}

// Return any methods of a given name.
// Zero length vector returned if the method isn't found.
vector<method_info> get_method(const class_info &ci, const string &name)
{
    vector<method_info> result;

    for (auto &&m: ci.methods)
    {
        if (m.name == name) {
            result.push_back(m);
        }
    }

    return result;
}