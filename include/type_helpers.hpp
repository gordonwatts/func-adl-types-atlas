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
// for nto just C++.
std::string normalized_type_name(const typename_info &ti);
std::string normalized_type_name(const std::string &ti);

#endif
