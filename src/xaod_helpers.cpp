#include "xaod_helpers.hpp"
#include "type_helpers.hpp"

#include <sstream>
#include <map>

using namespace std;

map<string, string> g_mapped_collection_names = {
    {"MissingETs", "MissingET"}
};

// see if the ending is right
// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding (string const &fullString, string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

// Returns the type info for the first class or inherrited class that
// has name as the name as a class. Can't climb the inherritance tree far,
// but it will try.
typename_info get_first_class(const class_info &c, const string &name) {
    // Check the class itself
    if (c.name_as_type.type_name == name) {
        return c.name_as_type.template_arguments[0];
    }

    // Go after the inherrited item
    for (auto &&i_name : c.inherrited_class_names)
    {
        auto t_info = parse_typename(i_name);
        if (t_info.type_name ==  name) {
            return t_info.template_arguments[0];
        }
    }

    throw runtime_error("No DataVector - programming error");
}

// Extract a collection from the info.
collection_info get_collection_info(const class_info &c) {
    collection_info r;

    // Find the name that ends in collection - we'll use that
    // to base our other things
    string collection_name = c.name;
    if (!hasEnding(collection_name, "Container")) {
        for (auto &&a_name : c.aliases)
        {
            if (hasEnding(a_name, "Container")) {
                collection_name = a_name;
            }
        }
    }

    // We need to pull out the name from this
    auto collection_ti = parse_typename(collection_name);
    r.name = collection_ti.type_name.substr(0, collection_ti.type_name.size()-string("Container").size()) + "s";

    if (g_mapped_collection_names.find(r.name) != g_mapped_collection_names.end()) {
        r.name = g_mapped_collection_names[r.name];
    }

    // Type info goes back to the very basic actual name.
    r.type_info = c.name_as_type;

    // The iterator looks at the thing we are looking at. This needs to be a DataVector.
    ostringstream it_name;
    it_name << "Iterator<" << get_first_class(c, "DataVector").nickname << ">";

    r.iterator_type_info = parse_typename(it_name.str());

    return r;
}


// Given the list of parsed classes, returns the class info for everything
// that is a collection.
vector<collection_info> find_collections(const vector<class_info> &all_classes)
{
    vector<collection_info> result;

    for (auto &&c : all_classes)
    {
        result.push_back(get_collection_info(c));
    }
    

    return result;
}

// Output to the stream a collection
std::ostream& operator <<(std::ostream& stream, const collection_info& ci)
{
    return stream;
}