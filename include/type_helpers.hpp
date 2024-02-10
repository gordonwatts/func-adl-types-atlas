#ifndef __type_helpers__
#define __type_helpers__

#include "class_info.hpp"

#include <string>
#include <vector>
#include <set>
#include <map>

// Return a typename without < or >, or namespace
std::string type_name(const std::string &qualified_type_name);
std::vector<std::string> type_name(const std::vector<std::string> &full_type_names);
std::set<std::string> type_name(const std::set<std::string> &full_type_names);

// Types that inherit from a particular class
std::vector<std::string> all_that_inherit_from(const std::string &c_name);

// Add typedef's to the list of classes we already know about.
void fixup_type_aliases(std::vector<class_info> &classes);

// Resolve all typedefs on all types
void fixup_type_defs(std::vector<class_info> &classes);

// Return the typdefs we know about
std::map<std::string, std::vector<std::string>> root_typedef_map();

// Return the actual name of a class, peering through all typedefs
std::string resolve_typedef(const std::string &c_name);

// Return a type name with & and * stripped off the end.
std::string unqualified_type_name(const std::string &full_type_name);

// Parse a fully qualified typename
typename_info parse_typename(const std::string &type_name);

// Return type info for the first class or inherited class that
// has name as the name of a template class.
typename_info get_first_class(const class_info &c, const std::string &name);

// Return the type as a string, but normalized
// for just C++.
std::string normalized_type_name(const typename_info &ti);
std::string normalized_type_name(const std::string &ti);

// Return a set of enums that the class defines, returning
// a set of their fully qualified names
std::vector<std::string> class_enums(const class_info &c);

// Return the complete type without modifiers (like * or const)
std::string unqualified_typename(const typename_info &ti);

// Return the C++ type in a standard format
std::string typename_cpp_string(const typename_info &ti);

// Is this a collection? What does it contain?
bool is_collection(const typename_info &ti);
typename_info container_of(const typename_info &ti);
bool is_collection(const class_info &ci);
typename_info container_of(const class_info &ci);

// Is this type something we can deal with?
bool is_understood_type(const std::string &t_name, const std::set<std::string> &known_types);
bool is_understood_type(const typename_info &t, const std::set<std::string> &known_types);

// Is this method something we can deal with?
bool is_understood_method(const method_info &meth, const std::set<std::string> &classes_to_emit);

// Return the python version of the typename.
typename_info py_typename(const std::string &t_name);
typename_info py_typename(const typename_info &t);
#endif
