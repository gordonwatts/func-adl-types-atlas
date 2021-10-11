#include "util_string.hpp"

using namespace std;

// see if the ending is right
// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding (string const &fullString, string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

// If present, remove the prefix from the string
string remove_prefix(const string &orig_string, const string &beginning)
{
    if (beginning.size() > orig_string.size()) {
        return orig_string;
    }
    if (beginning == orig_string.substr(0, beginning.size())) {
        return orig_string.substr(beginning.size());
    }
    return orig_string;
}

// If present, remove the post-fix
string remove_postfix(const string &orig_string, const string &postfix)
{
    string result (orig_string);
    if (hasEnding(result, postfix)) {
        result = result.substr(0, result.size() - postfix.size());
    }
    return result;
}

// Remove whitespace around a string.
string trim(const string& str)
{
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

