#include "metadata_file_finder.hpp"

metadata_file_finder::metadata_file_finder (const std::string &release_name)
{
    m_release_name = release_name;
}

std::string metadata_file_finder::operator() (const std::string &template_name) const
{
    return "metadata/" + template_name;
}
