#include "helper_files.hpp"
#include <string>
#include <fstream>
#include <vector>

using namespace std;


struct helper_file {
    // Name of the file
    string name;

    // Lines to add to the init
    vector<string> init_lines;
};

// Files to add in!
vector<helper_file> _g_helper_files {
    { "trigger.py", {"from .trigger import tdt_chain_fired", "from .trigger import tmt_match_object"} },
    { "type_support.py", {"from .type_support import cpp_float, cpp_double, cpp_vfloat", "from . import type_support"}}
};

const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

// Write out all helper information.
void emit_helper_files(YAML::Emitter &out)
{
    bool header_written = false;
    for (auto &&hf : _g_helper_files)
    {
        if (!header_written) {
            header_written = true;
            out << YAML::Key << "files" << YAML::Value << YAML::BeginSeq;
        }

        out << YAML::BeginMap;

        // Header info
        out << YAML::Key << "name" << YAML::Value << hf.name;
        out << YAML::Key << "init_lines" << YAML::Value << YAML::BeginSeq;
        for (auto &&line : hf.init_lines) {
            out << line;
        }
        out << YAML::EndSeq;

        // Now file contents
        ifstream file_contents("metadata/" + hf.name);
        string line;
        out << YAML::Key << "contents" << YAML::Value << YAML::BeginSeq;
        if (!file_contents.is_open()) {
            throw runtime_error("could not find helper file metadata/" + hf.name);
        }
        out.SetStringFormat(YAML::DoubleQuoted);
        while (getline(file_contents, line)) {
            out << rtrim(line);
        }
        out << YAML::EndSeq;

        out << YAML::EndMap;

    }

    if (header_written) {
        out << YAML::EndSeq;
    }
}