#include <gtest/gtest.h>
#include "xaod_helpers.hpp"
#include "type_helpers.hpp"

#include <vector>

using namespace std;

// Make sure basic type comes back properly
TEST(t_xaod_helpers, normal_jet_collection) {

    class_info a;
    a.name = "DataVector<xAOD::Jet_v1>";
    a.name_as_type = parse_typename(a.name);
    a.aliases.push_back("xAOD::JetContainer");
    a.aliases.push_back("xAOD::JetContainer_v1");
    a.inherrited_class_names.push_back("DataVector<xAOD::IParticle>");

    vector<class_info> seq;
    seq.push_back(a);

    auto r = find_collections(seq);

    EXPECT_EQ(r.size(), 1);
    EXPECT_EQ(r[0].name, "Jets");

    EXPECT_EQ(r[0].iterator_type_info.type_name, "Iterator");
    EXPECT_EQ(r[0].iterator_type_info.namespace_list.size(), 0);
    EXPECT_EQ(r[0].iterator_type_info.template_arguments.size(), 1);
    EXPECT_EQ(r[0].iterator_type_info.template_arguments[0].type_name, "Jet_v1");

    EXPECT_EQ(r[0].type_info.nickname, "DataVector<xAOD::Jet_v1>");
}

// Make sure basic type comes back properly
TEST(t_xaod_helpers, normal_met_collection) {

    class_info a;
    a.name = "xAOD::MissingETContainer_v1";
    a.name_as_type = parse_typename(a.name);
    a.aliases.push_back("xAOD::MissingETContainer");
    a.inherrited_class_names.push_back("DataVector<xAOD::MissingET_v1>");

    vector<class_info> seq;
    seq.push_back(a);

    auto r = find_collections(seq);

    EXPECT_EQ(r.size(), 1);
    EXPECT_EQ(r[0].name, "MissingET");

    EXPECT_EQ(r[0].iterator_type_info.type_name, "Iterator");
    EXPECT_EQ(r[0].iterator_type_info.namespace_list.size(), 0);
    EXPECT_EQ(r[0].iterator_type_info.template_arguments.size(), 1);
    EXPECT_EQ(r[0].iterator_type_info.template_arguments[0].type_name, "MissingET_v1");

    EXPECT_EQ(r[0].type_info.nickname, "xAOD::MissingETContainer_v1");
}
