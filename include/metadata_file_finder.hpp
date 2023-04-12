#ifndef __metadata_file_finder__
#define __metadata_file_finder__

#include <string>

class metadata_file_finder
{
public:
    metadata_file_finder (const std::string &release_name);

    std::string operator() (const std::string &template_name) const;

private:
    std::string m_release_name;
};
#endif