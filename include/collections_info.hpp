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
};


// Hardcode the config for ATLAS R21
std::map<std::string, collection_extra> _g_collection_config {
    {"Jets", {"Jets", {
        {"name", "str", ""}        
    }, {
        {"calibration", "str", "'NOSYS'", {
            {"'*None*'", {}, "{bank_name}"},
            {"'*Any*'", {"sys_error_tool", "pileup_tool", "jet_corrections", "add_calibration_to_job"}, "{bank_name}Calib_{calibration}"}
            }
        },
        {"truth_jets", "str", "'AntiKt4TruthDressedWZJets'"},
    }}},
    {"Muons", {"Muons", {}, {
        {"calibration", "str", "'NOSYS'", {
            {"'*None*'", {}, "{bank_name}"},
            {"'*Any*'", {"sys_error_tool", "pileup_tool", "muon_corrections", "add_calibration_to_job"}, "{bank_name}Calib_{working_point}{isolation}_{calibration}"}
            }
        },
        {"working_point", "str", "'Medium'", {}},
        {"isolation", "str", "'NonIso'", {}}
    }}},
    {"Electrons", {"Electrons", {}, {
        {"calibration", "str", "'NOSYS'", {
            {"'*None*'", {}, "{bank_name}"},
            {"'*Any*'", {"sys_error_tool", "pileup_tool", "electron_corrections", "add_calibration_to_job"}, "{bank_name}_{working_point}_{isolation}_{calibration}"}
            }
        },
        {"working_point", "str", "'MediumLHElectron'", {}},
        {"isolation", "str", "'NonIso'", {}}
    }}},
    {"Photons", {"Photons", {}, {
        {"calibration", "str", "'NOSYS'", {
            {"'*None*'", {}, "{bank_name}"},
            {"'*Any*'", {"sys_error_tool", "pileup_tool", "photon_corrections", "add_calibration_to_job"}, "{bank_name}_{working_point}_{isolation}_{calibration}"}
            }
        },
        {"working_point", "str", "'Tight'", {}},
        {"isolation", "str", "'NonIso'", {}}
    }}},
    {"TauJets", {"TauJets", {}, {
        {"calibration", "str", "'NOSYS'", {
            {"'*None*'", {}, "{bank_name}"},
            {"'*Any*'", {"sys_error_tool", "pileup_tool", "tau_corrections", "add_calibration_to_job"}, "{bank_name}_{working_point}_{calibration}"}
            }
        },
        {"working_point", "str", "'Tight'", {}},
    }}},
    {"DiTauJets", {"DiTauJets", {}, {
        {"calibration", "str", "'NOSYS'", {
            {"'*None*'", {}, "{bank_name}"},
            {"'*Any*'", {"sys_error_tool", "pileup_tool", "ditau_corrections", "add_calibration_to_job"}, "{bank_name}_{working_point}_{calibration}"}
            }
        },
        {"working_point", "str", "'Tight'", {}},
    }}}
};

#endif
