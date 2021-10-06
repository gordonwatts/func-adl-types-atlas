#include "class_info.hpp"
#include "type_helpers.hpp"

#include <vector>
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

    if (ci.inherrited_class_names.size() > 0) {
        stream << "  Inherrits from:" << endl;
        for (auto &&i_name : ci.inherrited_class_names)
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

    stream << ti.type_name;

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
    set<string> result(c_info.inherrited_class_names.begin(), c_info.inherrited_class_names.end());

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

    if (m_info.return_type.size() > 0) {
        result.insert(unqualified_type_name(m_info.return_type));
    }

    return vector<string>(result.begin(), result.end());
}

std::vector<std::string> referenced_types(const typename_info &t_info)
{
    set<string> result;

    for (auto &&ti : t_info.namespace_list)
    {
        auto new_types = referenced_types(ti);
        result.insert(new_types.begin(), new_types.end());
    }
    for (auto &&ti : t_info.template_arguments)
    {
        auto new_types = referenced_types(ti);
        result.insert(new_types.begin(), new_types.end());
    }
    result.insert(t_info.nickname);

    return vector<string>(result.begin(), result.end());
}
