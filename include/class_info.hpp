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

    // The full name of the type, including all qualifiers
    // int, int*, std::vector<std::jet>&, const int &, etc.
    std::string cpp_name;

    // Is this a const decl?
    bool is_const;

    // Is this a pointer?
    bool is_pointer;

    // And if this is a point, is a const pointer?
    // int * const
    bool is_const_pointer;
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

    // Parameterized callback arguments
    std::vector<method_arg> parameter_arguments;

    // The helper class if there are template arguments - otherwise it should
    // be empty. It is used as the method return type in order to
    // allow type-checking to proceed.
    std::string parameter_type_helper;

    // What should be the parameter method callback to process this if
    // template arguments are present?
    std::string param_method_callback;
};

struct enum_info {
    // The name of the enum
    std::string name;

    // List of the enum values
    std::vector<std::pair<std::string, int>> values;
};

struct class_info {
    // Fully qualified C++ class name, as known to ROOT
    std::string name;
    typename_info name_as_type;

    // Include file where this object is declared
    std::string include_file;

    // List of aliases for this class (other names).
    std::vector<std::string> aliases;

    // List of fully qualified C++ class names that this directly inherits from
    std::vector<std::string> inherited_class_names;

    // List of all methods
    std::vector<method_info> methods;

    // The library this is located in
    std::string library_name;

    // Other classes this can take on a the behavior of
    std::vector<std::string> class_behaviors;

    // The list of enums
    std::vector<enum_info> enums;
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

// Return true if there is a method in the class
bool has_methods(const class_info &ci, const std::vector<std::string> &names);
std::vector<method_info> get_method(const class_info &ci, const std::string &method);

#endif
