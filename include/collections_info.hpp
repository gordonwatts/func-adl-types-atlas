#ifndef __collections_info__
#define __collections_info__

#include <string>
#include <map>
#include <vector>

// Collections can have options which download metadata and do other things. This file tracks that.

// Action that can be taken
struct var_action {
    // The value for this action
    std::string value;

    // When the variable is this value, these are the metadata names that
    // should be run.
    std::vector<std::string> metadata_names;
};

// Info for a parameter
struct parameter_info {
    // The variable name
    std::string name;

    // The python type
    std::string p_type;

    // The default value
    std::string p_default;

    // Actions for the values
    std::vector<var_action> variable_actions;
};

struct collection_extra {
    // The collection name
    std::string name;

    // List of extra parameters
    std::vector<parameter_info> parameters;
};


// Hardcode the config for ATLAS R21
std::map<std::string, collection_extra> _g_collection_config {
    {"Jets", {"Jets", {
        {"calibrated", "bool", "True", {{"True", {"sys_error_tool", "pileup_tool", "jet_corrections"}}}}
    }}}
};

#endif