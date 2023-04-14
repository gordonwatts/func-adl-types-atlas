#include <gtest/gtest.h>
#include "metadata_file_finder.hpp"

using namespace std;

// Make sure basic type comes back properly
TEST(t_metadata_file_finder, find_basic_files)
{
    metadata_file_finder finder("22.1.113", "../metadata/");

    EXPECT_EQ(finder("corrections_tau.py"), "../metadata/corrections_tau.py");
}

TEST(t_metadata_file_finder, find_from_base)
{
    metadata_file_finder finder("20.1.1", "../tests/metadata/");

    EXPECT_EQ(finder("base.py"), "../tests/metadata/base.py");
}

TEST(t_metadata_file_finder, find_matched_file)
{
    metadata_file_finder finder("21.2.100", "../tests/metadata/");

    EXPECT_EQ(finder("base.py"), "../tests/metadata/21/2/100/base.py");
}

TEST(t_metadata_file_finder, find_back_up)
{
    metadata_file_finder finder("21.2.99", "../tests/metadata/");

    EXPECT_EQ(finder("base.py"), "../tests/metadata/base.py");
}

TEST(t_metadata_file_finder, find_in_earlier_release)
{
    metadata_file_finder finder("21.2.110", "../tests/metadata/");

    EXPECT_EQ(finder("base.py"), "../tests/metadata/21/2/100/base.py");
}

TEST(t_metadata_file_finder, find_in_later_series)
{
    metadata_file_finder finder("22.2.110", "../tests/metadata/");

    EXPECT_EQ(finder("base.py"), "../tests/metadata/21/2/100/base.py");
}

TEST(t_metadata_file_finder, release_name_parsing)
{
    auto r = parse_release("22.1.110");

    EXPECT_EQ(r[0], 22);
    EXPECT_EQ(r[1], 1);
    EXPECT_EQ(r[2], 110);
}
