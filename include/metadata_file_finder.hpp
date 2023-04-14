#ifndef __metadata_file_finder__
#define __metadata_file_finder__

#include <string>
#include <vector>
#include <filesystem>

class metadata_file_finder
{
public:
    // Constructor - takes the release name in the form "xx.xx.xx"
    // where each "xx" is a number.
    metadata_file_finder (const std::string &release_name, const std::string &default_prefix = "metadata/");

    std::string operator() (const std::string &template_name) const;

private:
    std::vector<std::filesystem::path> m_search_directories;
};

// Return the release name as 3 digits.
std::vector<int> parse_release (const std::string &release_name);

#endif
