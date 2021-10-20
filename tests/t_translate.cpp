#include <gtest/gtest.h>

#include "translate.hpp"

#include <algorithm>

using namespace std;

TEST(t_translate, include_files) {
    auto info = translate_class("xAOD::Jet_v1");

    EXPECT_EQ(info.include_file, "xAODJet/versions/Jet_v1.h");
}

TEST(t_translate, include_files_no_shared_libs) {
    auto info = translate_class("DataModel_detail::NoBase");

    EXPECT_EQ(info.include_file, "");
}

TEST(t_translate, include_files_neutral_particle) {
    auto info = translate_class("xAOD::NeutralParticleContainer");

    EXPECT_EQ(info.include_file, "xAODTracking/NeutralParticleContainer.h");
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
