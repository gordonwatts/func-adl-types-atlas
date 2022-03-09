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

    // The bank name can be renamed too. Leave this empty
    // if the bank name shouldn't be renamed. Otherwise follows usual
    // replacement guidelines.
    std::string bank_rename;
};

// Info for a parameter
struct parameter_info {
    // The variable name
    std::string name;

    // The python type
    std::string p_type;

    // The default value
    std::string p_default;
};

struct parameter_info_extra: public parameter_info {

    // Actions for the values
    std::vector<var_action> variable_actions;
};

struct collection_extra {
    // The collection name
    std::string name;

    // List of parameters
    std::vector<parameter_info_extra> parameters;

    // List of extra parameters
    std::vector<parameter_info_extra> extra_parameters;

    // method decoration/callback
    std::string method_callback;
};


// Hardcode the config for ATLAS R21
std::map<std::string, collection_extra> _g_collection_config {
    {"Jets", {"Jets", {
        {"calibrated_collection", "Optional[str]", "None"},
        {"uncalibrated_collection", "Optional[str]", "None"},
    }, {}, "lambda s, a: {{package_name}}.calibration_support.fixup_collection_call(s, a, 'jet_collection')"}},
    {"Electrons", {"Electrons", {
        {"calibrated_collection", "Optional[str]", "None"},
        {"uncalibrated_collection", "Optional[str]", "None"},
    }, {}, "lambda s, a: {{package_name}}.calibration_support.fixup_collection_call(s, a, 'electron_collection')"}},
    {"Muons", {"Muons", {
        {"calibrated_collection", "Optional[str]", "None"},
        {"uncalibrated_collection", "Optional[str]", "None"},
    }, {}, "lambda s, a: {{package_name}}.calibration_support.fixup_collection_call(s, a, 'muon_collection')"}},
    {"Photons", {"Photons", {
        {"calibrated_collection", "Optional[str]", "None"},
        {"uncalibrated_collection", "Optional[str]", "None"},
    }, {}, "lambda s, a: {{package_name}}.calibration_support.fixup_collection_call(s, a, 'photon_collection')"}},
    {"TauJets", {"TauJets", {
        {"calibrated_collection", "Optional[str]", "None"},
        {"uncalibrated_collection", "Optional[str]", "None"},
    }, {}, "lambda s, a: {{package_name}}.calibration_support.fixup_collection_call(s, a, 'tau_collection')"}},
    {"MissingET", {"MissingET", {
        {"calibrated_collection", "Optional[str]", "None"},
        {"uncalibrated_collection", "Optional[str]", "None"},
    }, {}, "lambda s, a: {{package_name}}.calibration_support.fixup_collection_call(s, a, 'met_collection')"}},
    {"DiTauJets", {"DiTauJets", {}, {
        {"calibration", "str", "'NOSYS'", {
            {"'*None*'", {}, "{bank_name}"},
            {"'*Any*'", {"sys_error_tool", "pileup_tool", "ditau_corrections", "add_calibration_to_job"}, "{bank_name}_{working_point}_{calibration}"}
            }
        },
        {"working_point", "str", "'Tight'", {}},
    }, ""}}
};

#endif
