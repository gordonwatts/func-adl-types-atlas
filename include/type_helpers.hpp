#ifndef __type_helpers__
#define __type_helpers__

#include "class_info.hpp"

#include <string>
#include <vector>
#include <set>

// Return a typename without < or >, or namespace
std::string type_name(const std::string &qualified_type_name);
std::vector<std::string> type_name(const std::vector<std::string> &full_type_names);
std::set<std::string> type_name(const std::set<std::string> &full_type_names);

// Add typedef's to the list of classes we already know about.
void fixup_type_aliases(std::vector<class_info> &classes);

// Return a type name with & and * stripped off the end.
std::string unqualified_type_name(const std::string &full_type_name);
#endif
