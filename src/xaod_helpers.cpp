#include "xaod_helpers.hpp"
#include "type_helpers.hpp"
#include "util_string.hpp"

#include <sstream>
#include <map>
#include <algorithm>

#include "TClass.h"

using namespace std;

// Rename the collections slightly to make a little more
// sense one way or the other.
map<string, string> g_mapped_collection_names = {
    {"MissingETs", "MissingET"},
};

// These are items in the xAOD that are single items, along with their
// returned type.
map<string, string> g_single_collection_names = {
    {"EventInfo", "xAOD::EventInfo"}
};

// Find the name that has "Container" in it, or return empty string.
string get_prefix_name(const class_info &c, const string &prefix) {
    if (hasEnding(c.name, prefix)) {
        return c.name;
    }
    for (auto &&a_name : c.aliases)
    {
        if (hasEnding(a_name, prefix)) {
            return a_name;
        }
    }

    return "";
}


// Extract a collection from the info.
collection_info get_collection_info(const class_info &c, const vector<class_info> &all_classes) {
    collection_info r;

    // Find the name that ends in collection - we'll use that
    // to base our other things
    string collection_name = get_prefix_name(c, "Container");

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
    auto item = get_first_class(c, "DataVector");
    it_name << "Iterable<" << item.nickname << ">";

    // And we need to fetch the include file for the collections too.
    r.include_file = c.include_file;

    // The library is just hte prefix on the include for the cpp item
    auto cls_ptr = find_if(all_classes.begin(), all_classes.end(),
        [item](const class_info &a_class_item){return a_class_item.name == item.nickname;});
    if (cls_ptr == all_classes.end()) {
        throw runtime_error("Cannot find class " + item.nickname + " in ROOT's class list");
    }
    r.link_libraries.push_back(cls_ptr->library_name);


    // And parse the iterator
    r.iterator_type_info = parse_typename(it_name.str());

    return r;
}


// See if this is a collection class or not
bool is_collection_class(const class_info &c) {
    if (get_first_class(c, "DataVector").type_name.size() == 0)
        return false;

    return get_prefix_name(c, "Container").size() != 0;
}

// Given the list of parsed classes, returns the class info for everything
// that is a collection.
vector<collection_info> find_collections(const vector<class_info> &all_classes)
{
    vector<collection_info> result;

    // From the list of all classes, generate the collections for those
    // that inherit correctly.
    set<string> seen_collections;
    for (auto &&c : all_classes)
    {
        if (is_collection_class(c)) {
            auto c_info = get_collection_info(c, all_classes);
            if (seen_collections.find(c_info.name) == seen_collections.end()) {
                result.push_back(c_info);
                seen_collections.insert(c_info.name);
            }
        }
    }

    return result;
}

// Find all single collections items. Only return ones where
// the types are listed in all classes.
vector<collection_info> get_single_object_collections(const vector<class_info> &all_classes)
{
    vector<collection_info> result;

    // We know ahead of time some collections should be accessed by just their first item as a handle, we
    // turn those into collections here.
    for (auto &&c : g_single_collection_names) {
        auto resolved_name = resolve_typedef(c.second);
        auto found_class = find_if(all_classes.begin(), all_classes.end(), [resolved_name](const class_info &cls){return cls.name == resolved_name;});
        if (found_class != all_classes.end()) {
            collection_info ci;
            ci.name = c.first;
            ci.type_info = parse_typename(resolved_name);
            ci.iterator_type_info = parse_typename(resolved_name);
            ci.include_file = found_class->include_file;
            ci.link_libraries.push_back(found_class->library_name);
            result.push_back(ci);
        }
    }

    return result;
}


// Output to the stream a collection
std::ostream& operator <<(std::ostream& stream, const collection_info& ci)
{
    stream << "Collection Name: " << ci.name << endl;
    stream << "  Full Type Info: " << ci.type_info << endl;
    stream << "  Iterable Info: " << ci.iterator_type_info << endl;
    stream << "  Include file: " << ci.include_file << endl;

    return stream;
}