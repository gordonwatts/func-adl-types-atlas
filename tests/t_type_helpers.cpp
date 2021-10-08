#include <gtest/gtest.h>
#include "type_helpers.hpp"

// Make sure basic type comes back properly
TEST(t_type_helpers, type_int) {
    auto t = parse_typename("int");

    EXPECT_EQ(t.namespace_list.size(), 0);
    EXPECT_EQ(t.template_arguments.size(), 0);
    EXPECT_EQ(t.type_name, "int");
}

// Simple template
TEST(t_type_helpers, type_vector_int) {
    auto t = parse_typename("vector<int>");

    EXPECT_EQ(t.namespace_list.size(), 0);
    EXPECT_EQ(t.template_arguments.size(), 1);
    auto sub_t = t.template_arguments[0];
    EXPECT_EQ(sub_t.type_name, "int");
    EXPECT_EQ(sub_t.template_arguments.size(), 0);
    EXPECT_EQ(sub_t.namespace_list.size(), 0);
    EXPECT_EQ(t.type_name, "vector");
}


// Simple typename qualified by namespace
TEST(t_type_helpers, type_namespaced_type) {
    auto t = parse_typename("std::size_t");

    EXPECT_EQ(t.type_name, "size_t");

    EXPECT_EQ(t.namespace_list.size(), 1);
    auto t_ns = t.namespace_list[0];
    EXPECT_EQ(t_ns.type_name, "std");
    EXPECT_EQ(t_ns.template_arguments.size(), 0);
    EXPECT_EQ(t_ns.namespace_list.size(), 0);

    EXPECT_EQ(t.template_arguments.size(), 0);
}


// Simple typename qualified by double namespace
TEST(t_type_helpers, type_namespaced_type_twice) {
    auto t = parse_typename("std::org::size_t");

    EXPECT_EQ(t.type_name, "size_t");
    EXPECT_EQ(t.nickname, "std::org::size_t");

    EXPECT_EQ(t.namespace_list.size(), 2);
    auto t_ns = t.namespace_list[0];
    EXPECT_EQ(t_ns.type_name, "std");
    EXPECT_EQ(t_ns.template_arguments.size(), 0);
    EXPECT_EQ(t_ns.namespace_list.size(), 0);

    auto t_ns2 = t.namespace_list[1];
    EXPECT_EQ(t_ns2.type_name, "org");
    EXPECT_EQ(t_ns2.template_arguments.size(), 0);
    EXPECT_EQ(t_ns2.namespace_list.size(), 0);

    EXPECT_EQ(t.template_arguments.size(), 0);
}


// Simple typename qualified by namespace
TEST(t_type_helpers, type_namespaced_burried) {
    auto t = parse_typename("vector<std::size_t>");

    EXPECT_EQ(t.type_name, "vector");

    EXPECT_EQ(t.namespace_list.size(), 0);

    EXPECT_EQ(t.template_arguments.size(), 1);
    auto t_arg1 = t.template_arguments[0];
    EXPECT_EQ(t_arg1.type_name, "size_t");
    EXPECT_EQ(t_arg1.namespace_list.size(), 1);

    auto t_ns1 = t_arg1.namespace_list[0];
    EXPECT_EQ(t_ns1.type_name, "std");
    EXPECT_EQ(t_ns1.namespace_list.size(), 0);
    EXPECT_EQ(t_ns1.template_arguments.size(), 0);

    EXPECT_EQ(t_arg1.template_arguments.size(), 0);
}


// Qaualified name by template
TEST(t_type_helpers, type_qualified_typename) {
    auto t = parse_typename("vector<float>::size_t");

    EXPECT_EQ(t.nickname, "vector<float>::size_t");
    EXPECT_EQ(t.type_name, "size_t");
    EXPECT_EQ(t.template_arguments.size(), 0);
    EXPECT_EQ(t.namespace_list.size(), 1);
    EXPECT_EQ(t.namespace_list[0].nickname, "vector<float>");
}


// Whitespace check
TEST(t_type_helpers, type_whitespace) {
    auto t = parse_typename("vector<float>::size_t ");

    EXPECT_EQ(t.nickname, "vector<float>::size_t");
}


// Simple typename qualified by namespace
TEST(t_type_helpers, type_multple_template_args) {
    auto t = parse_typename("vector<std::size_t, std::allocate<std::size_t>>");

    EXPECT_EQ(t.type_name, "vector");

    EXPECT_EQ(t.namespace_list.size(), 0);

    EXPECT_EQ(t.template_arguments.size(), 2);
    EXPECT_EQ(t.template_arguments[0].type_name, "size_t");
    EXPECT_EQ(t.template_arguments[1].type_name, "allocate");
}
