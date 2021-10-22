#ifndef __class_info__
#define __class_info__

#include <string>
#include <iostream>
#include <vector>

struct typename_info {
    // The list of identifiers separated by "::"
    std::vector<typename_info> namespace_list;

    // The actual type name
    std::string type_name;

    // The template arguments (if there are any)
    std::vector<typename_info> template_arguments;

    // The full name of the type
    std::string nickname;
};

struct method_arg {
    // Name of the argument
    std::string name;

    // The raw and full type name
    std::string raw_typename;
    std::string full_typename;
};

struct method_info {
    // The method name
    std::string name;

    // The return type
    std::string return_type;

    // Arguments
    std::vector<method_arg> arguments;
};

struct class_info {
    // Fully qualified C++ class name, as known to ROOT
    std::string name;
    typename_info name_as_type;

    // Include file where this object is declared
    std::string include_file;

    // List of aliases for this class (other names).
    std::vector<std::string> aliases;

    // List of fully qualified C++ class names that this directly inherrits from
    std::vector<std::string> inherrited_class_names;

    // List of all methods
    std::vector<method_info> methods;

    // The library this is located in
    std::string library_name;
};

std::ostream& operator <<(std::ostream& stream, const class_info& ci);
std::ostream& operator <<(std::ostream& stream, const method_info& mi);
std::ostream& operator <<(std::ostream& stream, const method_arg& ai);
std::ostream& operator <<(std::ostream& stream, const typename_info& ai);

// Return all types referenced
std::vector<std::string> referenced_types(const class_info &c_info);
std::vector<std::string> referenced_types(const method_info &m_info);
std::vector<std::string> referenced_types(const method_arg &a_info);
std::vector<std::string> referenced_types(const typename_info &a_info);

#endif