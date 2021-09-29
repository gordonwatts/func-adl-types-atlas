#ifndef __class_info__
#define __class_info__

#include <string>
#include <iostream>
#include <vector>

struct method_info {
    // The method name
    std::string name;

    // The return type
    std::string return_type;
};

struct class_info {
    // Fully qualified C++ class name, as known to ROOT
    std::string name;

    // List of fully qualified C++ class names that this directly inherrits from
    std::vector<std::string> inherrited_class_names;

    // List of all methods
    std::vector<method_info> methods;
};

std::ostream& operator <<(std::ostream& stream, const class_info& ci);
std::ostream& operator <<(std::ostream& stream, const method_info& mi);

// Return all types referenced
std::vector<std::string> referenced_types(const class_info &c_info);
std::vector<std::string> referenced_types(const method_info &m_info);

#endif