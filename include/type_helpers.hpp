#ifndef __type_helpers__
#define __type_helpers__

#include <string>
#include <vector>
#include <set>

// Return a typename without < or >, or namespace
std::string type_name(const std::string &qualified_type_name);
std::vector<std::string> type_name(const std::vector<std::string> &full_type_names);
std::set<std::string> type_name(const std::set<std::string> &full_type_names);


// Return a type name with & and * stripped off the end.
std::string unqualified_type_name(const std::string &full_type_name);
#endif
