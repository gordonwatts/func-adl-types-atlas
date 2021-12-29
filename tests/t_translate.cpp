#include <gtest/gtest.h>

#include "translate.hpp"

#include <algorithm>

using namespace std;

TEST(t_translate, include_files) {
    auto info = translate_class("xAOD::Jet_v1");

    EXPECT_EQ(info.include_file, "xAODJet/versions/Jet_v1.h");
    EXPECT_EQ(info.library_name, "xAODJet");
}

TEST(t_translate, include_files_no_shared_libs) {
    auto info = translate_class("DataModel_detail::NoBase");

    EXPECT_EQ(info.include_file, "");
}

TEST(t_translate, include_files_neutral_particle) {
    auto info = translate_class("xAOD::NeutralParticleContainer");

    EXPECT_EQ(info.include_file, "xAODTracking/NeutralParticleContainer.h");
    EXPECT_EQ(info.library_name, "xAODTracking");
}

TEST(t_translate, include_files_jet_container) {
    auto info = translate_class("xAOD::JetContainer");

    EXPECT_EQ(info.include_file, "xAODJet/JetContainer.h");
}

TEST(t_translate, include_files_jet_container_v1) {
    auto info = translate_class("xAOD::JetContainer_v1");

    EXPECT_EQ(info.include_file, "xAODJet/JetContainer.h");
}

TEST(t_translate, include_files_datavector) {
    auto info = translate_class("DataVector<xAOD::Jet_v1>");

    EXPECT_EQ(info.name, "DataVector<xAOD::Jet_v1>");
    EXPECT_EQ(info.include_file, "xAODJet/JetContainer.h");
}

TEST(t_translate, include_files_met) {
    auto info = translate_class("xAOD::MissingETContainer");

    EXPECT_EQ(info.include_file, "xAODMissingET/MissingETContainer.h");
}

TEST(t_translate, include_files_met_v1) {
    auto info = translate_class("xAOD::MissingETContainer_v1");

    EXPECT_EQ(info.include_file, "xAODMissingET/MissingETContainer.h");
}

TEST(t_translate, method_only_once) {
    auto info = translate_class("xAOD::Jet_v1");

    EXPECT_EQ(count_if(info.methods.begin(), info.methods.end(), [](const method_info &m) { return m.name == "pt";}), 1);
}

TEST(t_translate, truth_particle_methods) {
    auto info = translate_class("xAOD::TruthParticle_v1");
    EXPECT_EQ(count_if(info.methods.begin(), info.methods.end(), [](const method_info &m) { return m.name == "prodVtx";}), 1);
    EXPECT_EQ(count_if(info.methods.begin(), info.methods.end(), [](const method_info &m) { return m.name == "hasProdVtx";}), 1);
}

TEST(t_translate, vector_float) {
    auto info = translate_class("vector<float>");

    EXPECT_EQ(info.name, "vector<float>");
    EXPECT_EQ(info.methods.size(), 1);
    EXPECT_EQ(info.methods[0].name, "size");
    EXPECT_EQ(info.inherited_class_names.size(), 0);
    EXPECT_EQ(info.library_name, "");
}

TEST(t_translate, elementlink) {
    auto info = translate_class("ElementLink<DataVector<xAOD::Jet_v1> >");

    EXPECT_EQ(info.name, "ElementLink<DataVector<xAOD::Jet_v1>>");
    EXPECT_EQ(info.methods.size(), 1);
    EXPECT_EQ(info.methods[0].name, "isValid");
    EXPECT_EQ(info.inherited_class_names.size(), 0);
}
