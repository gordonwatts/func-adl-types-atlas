#include <gtest/gtest.h>
#include "xaod_helpers.hpp"
#include "type_helpers.hpp"

#include "TClass.h"

#include <vector>

using namespace std;

// Make sure basic type comes back properly
TEST(t_xaod_helpers, normal_jet_collection) {

    class_info a;
    a.name = "DataVector<xAOD::Jet_v1>";
    a.name_as_type = parse_typename(a.name);
    a.aliases.push_back("xAOD::JetContainer");
    a.aliases.push_back("xAOD::JetContainer_v1");
    a.inherited_class_names.push_back("DataVector<xAOD::IParticle>");

    vector<class_info> seq;
    seq.push_back(a);

    class_info a_j;
    a_j.name = "xAOD::Jet_v1";
    a_j.library_name = "xAODJet";
    seq.push_back(a_j);

    auto r = find_collections(seq);

    EXPECT_EQ(r.size(), 1);
    EXPECT_EQ(r[0].name, "Jets");

    EXPECT_EQ(r[0].iterator_type_info.type_name, "Iterable");
    EXPECT_EQ(r[0].iterator_type_info.namespace_list.size(), 0);
    EXPECT_EQ(r[0].iterator_type_info.template_arguments.size(), 1);
    EXPECT_EQ(r[0].iterator_type_info.template_arguments[0].type_name, "Jet_v1");

    EXPECT_EQ(r[0].type_info.cpp_name, "DataVector<xAOD::Jet_v1>");
    
    EXPECT_EQ(r[0].link_libraries.size(), 1);
    EXPECT_EQ(r[0].link_libraries[0], "xAODJet");
}

// Make sure basic type comes back properly
TEST(t_xaod_helpers, normal_met_collection) {

    class_info a;
    a.name = "xAOD::MissingETContainer_v1";
    a.name_as_type = parse_typename(a.name);
    a.aliases.push_back("xAOD::MissingETContainer");
    a.inherited_class_names.push_back("DataVector<xAOD::MissingET_v1>");

    vector<class_info> seq;
    seq.push_back(a);

    class_info a_j;
    a_j.name = "xAOD::MissingET_v1";
    a_j.library_name = "xAODMissingET";
    seq.push_back(a_j);

    auto r = find_collections(seq);

    EXPECT_EQ(r.size(), 1);
    EXPECT_EQ(r[0].name, "MissingET");

    EXPECT_EQ(r[0].iterator_type_info.type_name, "Iterable");
    EXPECT_EQ(r[0].iterator_type_info.namespace_list.size(), 0);
    EXPECT_EQ(r[0].iterator_type_info.template_arguments.size(), 1);
    EXPECT_EQ(r[0].iterator_type_info.template_arguments[0].type_name, "MissingET_v1");

    EXPECT_EQ(r[0].type_info.cpp_name, "xAOD::MissingETContainer_v1");

    EXPECT_EQ(r[0].link_libraries.size(), 1);
    EXPECT_EQ(r[0].link_libraries[0], "xAODMissingET");
}

// Collection isn't a container
TEST(t_xaod_helpers, no_good_collection) {

    class_info a;
    a.name = "xAOD::Jet_v1";
    a.name_as_type = parse_typename(a.name);
    a.aliases.push_back("xAOD::Jet");
    a.inherited_class_names.push_back("xAOD::IParticle");

    vector<class_info> seq;
    seq.push_back(a);

    auto r = find_collections(seq);

    EXPECT_EQ(r.size(), 0);
}

// Single copies only 
TEST(t_xaod_helpers, no_duplicates) {

    class_info a;
    a.name = "DataVector<xAOD::Jet_v1>";
    a.name_as_type = parse_typename(a.name);
    a.aliases.push_back("xAOD::JetContainer");
    a.aliases.push_back("xAOD::JetContainer_v1");
    a.inherited_class_names.push_back("DataVector<xAOD::IParticle>");

    vector<class_info> seq;
    seq.push_back(a);
    seq.push_back(a);

    class_info a_j;
    a_j.name = "xAOD::Jet_v1";
    a_j.library_name = "xAODJet";
    seq.push_back(a_j);

    auto r = find_collections(seq);

    EXPECT_EQ(r.size(), 1);
}


TEST(t_xaod_helpers, unqualified_straight) {
    EXPECT_EQ(unqualified_type_name("int"), "int");
}

TEST(t_xaod_helpers, unqualified_straight_pointer) {
    EXPECT_EQ(unqualified_type_name("int*"), "int");
}

TEST(t_xaod_helpers, unqualified_straight_ref) {
    EXPECT_EQ(unqualified_type_name("int&"), "int");
}

TEST(t_xaod_helpers, unqualified_straight_ref2) {
    EXPECT_EQ(unqualified_type_name("int&&"), "int");
}

TEST(t_xaod_helpers, unqualified_straight_ref_ptr_ref) {
    EXPECT_EQ(unqualified_type_name("int&*&"), "int");
}

TEST(t_xaod_helpers, unqualified_const) {
    EXPECT_EQ(unqualified_type_name("const int*"), "int");
}

TEST(t_xaod_helpers, single_object_collections_no_classes) {
    vector<class_info> classes;
    auto collections = get_single_object_collections(classes);
    EXPECT_EQ(collections.size(), 0);
}

TEST(t_xaod_helpers, single_object_collections) {
    vector<class_info> classes;
    class_info ci_ei;
    ci_ei.name = "xAOD::EventInfo_v1";
    ci_ei.name_as_type = parse_typename(ci_ei.name);
    ci_ei.library_name = "xAODEventInfo";
    ci_ei.include_file = "include.h";
    classes.push_back(ci_ei);
    TClass::GetClass("xAOD::EventInfo"); // Make sure the typdefs are loaded

    auto collections = get_single_object_collections(classes);

    EXPECT_EQ(collections.size(), 1);
    EXPECT_EQ(collections[0].link_libraries.size(), 1);
    EXPECT_EQ(collections[0].link_libraries[0], "xAODEventInfo");
    EXPECT_EQ(collections[0].include_file, "include.h");
}
