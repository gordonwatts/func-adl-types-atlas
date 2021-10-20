#ifndef __xaod_helpers__
#define __xaod_helpers__

#include "type_helpers.hpp"
#include "class_info.hpp"

#include <vector>

struct collection_info {
    // The C++ type name of the collection.
    std::string name;

    // The type info of the collection
    typename_info type_info;

    // The include file for the collection
    std::string include_file;

    // The simplified type-info for the collection
    typename_info iterator_type_info;
};

// Given the list of parsed classes, returns the class info for everything
// that is a collection.
std::vector<collection_info> find_collections(const std::vector<class_info> &all_classes);

std::vector<collection_info> get_single_object_collections(const std::vector<class_info> &all_classes);

std::ostream& operator <<(std::ostream& stream, const collection_info& ci);
#endif
