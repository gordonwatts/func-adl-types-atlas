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

// Types that inherrit from a particular class
std::vector<std::string> all_that_inherrit_from(const std::string &c_name);

// Add typedef's to the list of classes we already know about.
void fixup_type_aliases(std::vector<class_info> &classes);

// Return the typdefs we know about
std::map<std::string, std::vector<std::string>> root_typedef_map();


// Return a type name with & and * stripped off the end.
std::string unqualified_type_name(const std::string &full_type_name);

// Parse a fully qualified typename
typename_info parse_typename(const std::string &type_name);

// Return type info for the first class or inherrited class that
// has name as the name of a template class.
typename_info get_first_class(const class_info &c, const std::string &name);

#endif
