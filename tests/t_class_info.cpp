#include <gtest/gtest.h>
#include "class_info.hpp"
#include "type_helpers.hpp"

using namespace std;

// Make sure basic type comes back properly
TEST(t_class_info, referenced_class_inherrited_class) {
    class_info ci;
    ci.name = "SubClass";
    ci.name_as_type = parse_typename(ci.name);

    ci.inherrited_class_names.push_back("fork1");
    ci.inherrited_class_names.push_back("fork2");

    auto ref_list = referenced_types(ci);
    EXPECT_EQ(set<string>(ref_list.begin(), ref_list.end()), set<string>({"SubClass", "fork1", "fork2"}));
}


// A simple top level template
TEST(t_class_info, referenced_class_template_argument) {
    class_info ci;
    ci.name = "vector<SubClass>";
    ci.name_as_type = parse_typename(ci.name);

    auto ref_list = referenced_types(ci);
    EXPECT_EQ(set<string>(ref_list.begin(), ref_list.end()), set<string>({"SubClass", "vector<SubClass>"}));
}

// Template argument
TEST(t_class_info, referenced_typeinfo_template) {
    auto tn = parse_typename("vector<SubClass>");

    auto ref_list = referenced_types(tn);
    EXPECT_EQ(set<string>(ref_list.begin(), ref_list.end()), set<string>({"SubClass", "vector<SubClass>"}));
}

TEST(t_class_info, referenced_typeinfo_template_with_namespace) {
    auto tn = parse_typename("vector<std::SubClass>");

    auto ref_list = referenced_types(tn);
    EXPECT_EQ(set<string>(ref_list.begin(), ref_list.end()), set<string>({"std::SubClass", "vector<std::SubClass>"}));
}

TEST(t_class_info, referenced_typeinfo_qualified_name) {
    auto tn = parse_typename("vector<std::SubClass>::size_t");

    auto ref_list = referenced_types(tn);
    EXPECT_EQ(set<string>(ref_list.begin(), ref_list.end()), set<string>({"vector<std::SubClass>::size_t"}));
}

TEST(t_class_info, referenced_method_return_type) {
    method_info mi;
    mi.name = "do_it";
    mi.return_type = "vector<std::SubClass>";

    auto ref_list = referenced_types(mi);
    EXPECT_EQ(set<string>(ref_list.begin(), ref_list.end()), set<string>({"vector<std::SubClass>", "std::SubClass"}));
}

TEST(t_class_info, referenced_method_args) {
    method_info mi;
    mi.name = "do_it";
    mi.return_type = "vector<std::SubClass>";

    method_arg ma;
    ma.name = "a1";
    ma.raw_typename = "xAOD::Jet";
    ma.full_typename = "xAOD::Jet";
    mi.arguments.push_back(ma);

    auto ref_list = referenced_types(mi);
    EXPECT_EQ(set<string>(ref_list.begin(), ref_list.end()), set<string>({"vector<std::SubClass>", "std::SubClass", "xAOD::Jet"}));
}

TEST(t_class_info, referenced_typename_ignore_integers) {
    auto tn = parse_typename("Eigen::DenseBase<Eigen::Matrix<double,6,1,0,6,-1> >");

    auto ref_list = referenced_types(tn);
    EXPECT_EQ(set<string>(ref_list.begin(), ref_list.end()), set<string>({
        "Eigen::DenseBase<Eigen::Matrix<double,6,1,0,6,-1> >",
        "Eigen::Matrix<double,6,1,0,6,-1>",
        "double"}));
}
