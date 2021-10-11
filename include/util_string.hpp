#include <string>

// True if the string ends with these characters
bool hasEnding (std::string const &fullString, std::string const &ending);

// If present, remove the prefix from the string
std::string remove_prefix(const std::string &orig_string, const std::string &beginning);

// If present, remove the post-fix
std::string remove_postfix(const std::string &orig_string, const std::string &postfix);

// Remove whitespace around a string
std::string trim(const std::string& str);
