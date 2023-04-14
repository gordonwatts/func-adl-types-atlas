#include "metadata_file_finder.hpp"

#include <stdexcept>
#include <filesystem>
#include <algorithm>

using namespace std;

namespace
{
    vector<string> split(string s, string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        string token;
        vector<string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

    // Given our current path, and remaining release indicies, find the best matching directories
    // further down the line.
    vector<filesystem::path> find_best_match(const filesystem::path prefix, const vector<int> &indicies)
    {
        vector<filesystem::path> results;

        // End our recursion if we are done!
        if (indicies.size() == 0)
        {
            return results;
        }

        // Exact match makes life really easy!
        auto exact_dir = prefix / to_string(indicies[0]);
        if (filesystem::exists(exact_dir))
        {
            auto r_t = find_best_match(exact_dir, vector<int>(indicies.begin() + 1, indicies.end()));
            results.insert(results.end(), r_t.begin(), r_t.end());
            // And append last this directory to search!
            results.push_back(exact_dir);
        }
        else
        {
            // Ok - not exact. We now need to look at all the directories in here and figure out
            // what is the best match (if there is one). We use the less-than algorithm to count
            // down to find the best one.
            vector<int> found_directories;
            for (auto const &dir_entry : filesystem::directory_iterator{prefix})
            {
                if (dir_entry.is_directory())
                {
                    found_directories.push_back(stoi(dir_entry.path().filename()));
                }
            }
            if (!found_directories.empty())
            {
                sort(found_directories.begin(), found_directories.end());
                auto best_dir = upper_bound(found_directories.begin(), found_directories.end(), indicies[0]);
                if (best_dir != found_directories.begin())
                {
                    // Ok - we found one! Move back a space to get it.
                    best_dir--;
                    auto lower_bound_dir = prefix / to_string(*best_dir);
                    auto t_s = find_best_match(lower_bound_dir, vector<int>{1000000000});
                    results.insert(results.end(), t_s.begin(), t_s.end());
                    results.push_back(lower_bound_dir);
                }
            }
        }

        return results;
    }

    /// @brief Generate list of directories to search for metadata files for this guy
    /// @param prefix
    /// @param rel_info
    /// @return
    vector<filesystem::path> find_directory_list(const string &prefix, const vector<int> &rel_info)
    {
        filesystem::path prefix_path(prefix);

        // Find most recent release that matches
        auto result = find_best_match(prefix_path, rel_info);

        // Finally, add the root directory as the last place to search
        result.push_back(prefix);

        return result;
    }
}

// Return the release name as 3 digits.
vector<int> parse_release(const std::string &release_name)
{
    auto release_info = split(release_name, ".");
    if (release_info.size() == 3)
    {
        return {
            stoi(release_info[0]),
            stoi(release_info[1]),
            stoi(release_info[2])};
    }
    throw runtime_error("Invalid release name: " + release_name + " - must be in the form NN.MM.OO");
}

// Constructor - takes the release name in the form "xx.xx.xx"
metadata_file_finder::metadata_file_finder(const std::string &release_name, const string &default_prefix)
{
    auto release_info = parse_release(release_name);
    m_search_directories = find_directory_list(default_prefix, release_info);
}

string metadata_file_finder::operator()(const string &template_name) const
{
    // Find the first place this file exists in the list of directories we
    // cached.
    for (auto &&p : m_search_directories)
    {
        auto file = p / template_name;
        if (filesystem::exists(file))
        {
            return file;
        }
    }

    throw runtime_error("Template file " + template_name + " not found! Internal error!");
}
