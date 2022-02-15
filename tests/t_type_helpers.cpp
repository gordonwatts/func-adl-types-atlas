#include <gtest/gtest.h>
#include "type_helpers.hpp"

#include "TClass.h"

using namespace std;

// Make sure basic type comes back properly
TEST(t_type_helpers, type_int) {
    auto t = parse_typename("int");

    EXPECT_EQ(t.namespace_list.size(), 0);
    EXPECT_EQ(t.template_arguments.size(), 0);
    EXPECT_EQ(t.type_name, "int");
    EXPECT_EQ(t.is_pointer, false);
    EXPECT_EQ(t.is_const, false);
}

TEST(t_type_helpers, type_int_ptr) {
    auto t = parse_typename("int*");

    EXPECT_EQ(t.namespace_list.size(), 0);
    EXPECT_EQ(t.template_arguments.size(), 0);
    EXPECT_EQ(t.type_name, "int");
    EXPECT_EQ(t.nickname, "int*");
    EXPECT_EQ(t.is_pointer, true);
}

TEST(t_type_helpers, type_int_ref) {
    auto t = parse_typename("int&");

    EXPECT_EQ(t.namespace_list.size(), 0);
    EXPECT_EQ(t.template_arguments.size(), 0);
    EXPECT_EQ(t.type_name, "int");
    EXPECT_EQ(t.nickname, "int&");
    EXPECT_EQ(t.is_pointer, false);
}

TEST(t_type_helpers, type_int_ptr_space) {
    auto t = parse_typename("int *");

    EXPECT_EQ(t.namespace_list.size(), 0);
    EXPECT_EQ(t.template_arguments.size(), 0);
    EXPECT_EQ(t.type_name, "int");
    EXPECT_EQ(t.nickname, "int *");
    EXPECT_EQ(t.is_pointer, true);
}

TEST(t_type_helpers, type_int_const) {
    auto t = parse_typename("const int");

    EXPECT_EQ(t.namespace_list.size(), 0);
    EXPECT_EQ(t.template_arguments.size(), 0);
    EXPECT_EQ(t.type_name, "int");
    EXPECT_EQ(t.nickname, "const int");
    EXPECT_EQ(t.is_const, true);
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


// Qualified name by template
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

TEST(t_type_helpers, type_const_on_top) {
    auto t = parse_typename("const DataVector<xAOD::SlowMuon_v1, DataModel_detail::NoBase>::PtrVector");

    EXPECT_EQ(t.type_name, "PtrVector");
    EXPECT_EQ(t.is_const, true);
}

TEST(t_type_helpers, type_blank) {
    auto t = parse_typename("");

    EXPECT_EQ(t.type_name, "");
}

TEST(t_type_helpers, type_spaces) {
    auto t = parse_typename("  ");

    EXPECT_EQ(t.type_name, "");
}

// Simple typedef checks
TEST(t_type_helpers, typedef_resolve_simple) {
    TClass::GetClass("xAOD::EventInfo"); // Make sure the typedefs are loaded into root!
    EXPECT_EQ(resolve_typedef("xAOD::EventInfo"), "xAOD::EventInfo_v1");
}

TEST(t_type_helpers, typedef_resolve_size_t) {
    TClass::GetClass("xAOD::EventInfo"); // Make sure the typedefs are loaded into root!
    EXPECT_EQ(resolve_typedef("size_t"), "int");
}

TEST(t_type_helpers, typedef_resolve_blank) {
    EXPECT_EQ(resolve_typedef(""), "");
}

TEST(t_type_helpers, typedef_resolve_simple_ptr) {
    TClass::GetClass("xAOD::EventInfo"); // Make sure the typedefs are loaded into root!
    EXPECT_EQ(resolve_typedef("xAOD::EventInfo*"), "xAOD::EventInfo_v1*");
}

// Simple typedef checks - no change
TEST(t_type_helpers, typedef_resolve_none) {
    TClass::GetClass("xAOD::EventInfo"); // Make sure the typedefs are loaded into root!
    EXPECT_EQ(resolve_typedef("xAOD::EventInfo_v1"), "xAOD::EventInfo_v1");
}

TEST(t_type_helpers, typedef_resolve_utin32) {
    EXPECT_EQ(resolve_typedef("uint32_t"), "unsigned int");
}

TEST(t_type_helpers, typedef_resolve_ulong64) {
    EXPECT_EQ(resolve_typedef("ULong64_t"), "unsigned long long");
}

TEST(t_type_helpers, typedef_resolve_no_loops) {
    EXPECT_EQ(resolve_typedef("int"), "int");
}

TEST(t_type_helpers, typedef_fixup_methods) {
    method_info mi;
    mi.name = "test_method";
    mi.return_type = "ULong64_t";

    method_arg a1;
    a1.name = "arg1";
    a1.full_typename = "ULong64_t";
    mi.arguments.push_back(a1);

    class_info ci;
    ci.name = "class_1";
    ci.methods.push_back(mi);

    vector<class_info> all_classes;
    all_classes.push_back(ci);

    fixup_type_defs(all_classes);

    EXPECT_EQ(all_classes.size(), 1);
    auto new_ci = all_classes[0];
    EXPECT_EQ(new_ci.methods.size(), 1);
    auto new_mi = new_ci.methods[0];
    EXPECT_EQ(new_mi.return_type, "unsigned long long");
    EXPECT_EQ(new_mi.arguments.size(), 1);
    auto new_a1 = new_mi.arguments[0];
    EXPECT_EQ(new_a1.full_typename, "unsigned long long");
}

TEST(t_type_helpers, is_collection_int) {
    typename_info ti;
    ti.type_name = "int";
    ti.nickname = "int";
    EXPECT_EQ(is_collection(ti), false);
}

TEST(t_type_helpers, is_collection_vector) {
    typename_info ti_int;
    ti_int.type_name = "int";
    ti_int.nickname = "int";

    typename_info ti;
    ti.type_name = "vector";
    ti.nickname = "vector<int>";
    ti.template_arguments.push_back(ti_int);
    EXPECT_EQ(is_collection(ti), true);
}

TEST(t_type_helpers, is_collection_simple_class) {
    typename_info ti;
    ti.nickname = "dude";
    ti.nickname = "dude";

    class_info ci;
    ci.name = "dude";
    ci.name_as_type = ti;

    EXPECT_EQ(is_collection(ci), false);
}

TEST(t_type_helpers, container_of_begin_end_TIter) {
    typename_info ti;
    ti.nickname = "dude";
    ti.nickname = "dude";

    class_info ci;
    ci.name = "MyVector";
    ci.name_as_type.type_name = "MyVector";
    ci.name_as_type.nickname = "MyVector";

    // Add begin/end vector here
    method_info m_begin;
    m_begin.name = "begin";
    m_begin.return_type = "TIter";
    method_info m_end;
    m_end.name = "end";
    m_end.return_type = "TIter";
    ci.methods.push_back(m_begin);
    ci.methods.push_back(m_end);

    auto t = container_of(ci);

    EXPECT_EQ(t.nickname, "TObject");
}

TEST(t_type_helpers, container_of_begin_end_JetVertexConst) {
    typename_info ti;
    ti.nickname = "dude";
    ti.nickname = "dude";

    class_info ci;
    ci.name = "MyVector";
    ci.name_as_type.type_name = "MyVector";
    ci.name_as_type.nickname = "MyVector";

    // Add begin/end vector here
    method_info m_begin;
    m_begin.name = "begin";
    m_begin.return_type = "xAOD::JetConstituentVector::iterator";
    method_info m_end;
    m_end.name = "end";
    m_end.return_type = "xAOD::JetConstituentVector::iterator";
    ci.methods.push_back(m_begin);
    ci.methods.push_back(m_end);

    auto t = container_of(ci);

    EXPECT_EQ(t.nickname, "xAOD::JetConstituent*");
}

TEST(t_type_helpers, container_of_vector) {
    class_info ci;
    ci.name = "vector<int>";
    ci.name_as_type = parse_typename("vector<int>");
    
    method_info m_begin;
    m_begin.name = "begin";
    m_begin.return_type = "vector<int>::iterator";
    method_info m_end;
    m_end.name = "end";
    m_end.return_type = "vector<int>::iterator";
    ci.methods.push_back(m_begin);
    ci.methods.push_back(m_end);

    auto t = container_of(ci);

    EXPECT_EQ(t.nickname, "int");
}

TEST(t_type_helpers, cpp_string_simple) {
    EXPECT_EQ(typename_cpp_string(parse_typename("int")), "int");
}

TEST(t_type_helpers, cpp_string_simple_pointer) {
    EXPECT_EQ(typename_cpp_string(parse_typename("int*")), "int *");
}

TEST(t_type_helpers, cpp_string_simple_const) {
    EXPECT_EQ(typename_cpp_string(parse_typename("const int*")), "const int *");
}

TEST(t_type_helpers, cpp_string_simple_ns) {
    EXPECT_EQ(typename_cpp_string(parse_typename("int*")), "int *");
}

TEST(t_type_helpers, cpp_string_simple_template) {
    EXPECT_EQ(typename_cpp_string(parse_typename("vector<int>")), "vector<int>");
}


TEST(t_type_helpers, cpp_string_simple_multi_ns) {
    EXPECT_EQ(typename_cpp_string(parse_typename("fork::spoon::int")), "fork::spoon::int");
}

TEST(t_type_helpers, cpp_string_simple_template_args) {
    EXPECT_EQ(typename_cpp_string(parse_typename("vector<int, float>")), "vector<int, float>");
}

TEST(t_type_helpers, unqualified_pointer) {
    EXPECT_EQ(unqualified_typename(parse_typename("int*")), "int");
}

TEST(t_type_helpers, unqualified_const) {
    EXPECT_EQ(unqualified_typename(parse_typename("const int")), "int");
}

TEST(t_type_helpers, unqualified_template_args_untouched) {
    EXPECT_EQ(unqualified_typename(parse_typename("const vector<int*>")), "vector<int *>");
}

TEST(t_type_helpers, unqualified_template_args_untouched_with_const) {
    EXPECT_EQ(unqualified_typename(parse_typename("vector<const int*>")), "vector<const int *>");
}

TEST(t_type_helpers, unqualified_const_datavector) {
    EXPECT_EQ(unqualified_typename(parse_typename("const DataVector<xAOD::SlowMuon_v1, DataModel_detail::NoBase>::PtrVector")), "DataVector<xAOD::SlowMuon_v1, DataModel_detail::NoBase>::PtrVector");
}

TEST(t_type_helpers, understood_simple_yes) {
    EXPECT_EQ(is_understood_type("hi", set<string>({"hi"})), true);
}

TEST(t_type_helpers, understood_simple_no) {
    EXPECT_EQ(is_understood_type("hi", set<string>()), false);
}

TEST(t_type_helpers, understood_simple_vector_yes) {
    EXPECT_EQ(is_understood_type("vector<hi>", set<string>({"vector<hi>"})), true);
}

TEST(t_type_helpers, understood_simple_vector_no) {
    EXPECT_EQ(is_understood_type("vector<hi>", set<string>({"hit"})), false);
}

// TEST(t_type_helpers, understood_simple_datavector_yes) {
//     EXPECT_EQ(is_understood_type("DataVector<hi>", set<string>({"hi"})), true);
// }

// TEST(t_type_helpers, understood_simple_datavector_no) {
//     EXPECT_EQ(is_understood_type("DataVector<hi>", set<string>({"hit"})), false);
// }

// TEST(t_type_helpers, understood_simple_elementlink_yes) {
//     EXPECT_EQ(is_understood_type("ElementLink<hi>", set<string>({"hi"})), true);
// }

// TEST(t_type_helpers, understood_simple_elementlink_no) {
//     EXPECT_EQ(is_understood_type("ElementLink<hi>", set<string>({"hit"})), false);
// }

TEST(t_type_helpers, understood_method_simple) {
    method_info m;
    m.name = "fork";
    m.return_type = "int";

    EXPECT_EQ(is_understood_method(m, set<string>({"int"})), true);
}

TEST(t_type_helpers, understood_method_unkown) {
    method_info m;
    m.name = "fork";
    m.return_type = "U";

    EXPECT_EQ(is_understood_method(m, set<string>({"int"})), false);
}

TEST(t_type_helpers, understood_method_template) {
    method_info m;
    m.name = "fork";
    m.return_type = "U";

    method_arg ma;
    ma.name = "return_type";
    ma.raw_typename = "cpp_type<U>";
    ma.full_typename = "cpp_type<U>";
    m.parameter_arguments.push_back(ma);

    EXPECT_EQ(is_understood_method(m, set<string>({"int"})), true);
}

TEST(t_type_helpers, py_type_simple_type) {
    auto t = py_typename("int");
    EXPECT_EQ(t.type_name, "int");
    EXPECT_EQ(t.is_pointer, false);
}

TEST(t_type_helpers, py_type_vector) {
    auto t = py_typename("vector<int>");
    EXPECT_EQ(t.nickname, "Iterable<int>");
    EXPECT_EQ(t.is_pointer, false);
}

TEST(t_type_helpers, py_type_element_link) {
    auto t = py_typename("ElementLink<DataVector<int>>");
    EXPECT_EQ(t.nickname, "int *");
    EXPECT_EQ(t.is_pointer, true);
}

TEST(t_type_helpers, py_type_dv) {
    auto t = py_typename("DataVector<int>");
    EXPECT_EQ(t.nickname, "Iterable<int>");
    EXPECT_EQ(t.is_pointer, false);
}

TEST(t_type_helpers, py_type_dv_el) {
    auto t = py_typename("vector<ElementLink<DataVector<int>>>");
    EXPECT_EQ(t.nickname, "Iterable<int *>");
    EXPECT_EQ(t.is_pointer, false);
}
// TODO: Remove py_typename, not used.

TEST(t_type_helpers, normalized_int) {
    EXPECT_EQ(normalized_type_name("int"), "int");
}

TEST(t_type_helpers, normalized_vector) {
    EXPECT_EQ(normalized_type_name("vector<float>"), "vector_float_");
}

TEST(t_type_helpers, normalized_iterable) {
    EXPECT_EQ(normalized_type_name("Iterable<float>"), "Iterable[float]");
}

TEST(t_type_helpers, normalized_vector_ns) {
    EXPECT_EQ(normalized_type_name("vector<ROOT::Fit::ParameterSettings>"), "vector_ROOT_Fit_ParameterSettings_");
}

TEST(t_type_helpers, normalized_vector_front_ns) {
    EXPECT_EQ(normalized_type_name("std::vector<double>"), "std.vector_float_");
}

TEST(t_type_helpers, normalized_elPtr) {
    EXPECT_EQ(normalized_type_name("ElementLink<DataVector<xAOD::Truth>>"), "ElementLink_DataVector_xAOD_Truth__");
}

TEST(t_type_helpers, normalized_cpp_type) {
    EXPECT_EQ(normalized_type_name("cpp_type<U>"), "cpp_type[U]");
}
