#include "class_info.hpp"
#include "type_helpers.hpp"

#include <vector>
#include <set>

using namespace std;

std::ostream& operator <<(std::ostream& stream, const class_info& ci) {
    stream << "Class: " << ci.name << endl;

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

    return stream;
}

///
// Find all classes that this class references.
std::vector<std::string> referenced_types(const class_info &c_info)
{
    set<string> result(c_info.inherrited_class_names.begin(), c_info.inherrited_class_names.end());

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
