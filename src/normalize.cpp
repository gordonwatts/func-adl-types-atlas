#include "normalize.hpp"

#include <regex>

// Type names are seperated from name spaces with ".", and C++
// uses "::". Fix up the typenames here.
// Note this is a type name, not a decl, so it won't fix up a templated
// type here. Nor will it touch the templated type.
std::string normalize_typename(const std::string &type_name)
{
    return std::regex_replace(type_name, std::regex("\\::"), ".");
}
