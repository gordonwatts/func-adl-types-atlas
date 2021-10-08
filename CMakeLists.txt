cmake_minimum_required(VERSION 3.0.0)
project(func_adl_types_atlas VERSION 0.1.0)

find_package(ROOT REQUIRED COMPONENTS Core)

# Get Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/075810f7a20405ea09a93f68847d6e963212fa62.zip
)
FetchContent_MakeAvailable(googletest)

include(CTest)
enable_testing()

# Package setup
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# One main library
add_library(wraper_generators
            src/normalize.cpp
            src/class_info.cpp
            src/type_helpers.cpp
            src/utils.cpp
            src/xaod_helpers.cpp
            src/translate.cpp)
target_link_libraries(wraper_generators ROOT::Core)

# Tests
add_executable(t_type_helpers tests/t_type_helpers.cpp)
target_link_libraries(t_type_helpers wraper_generators gtest_main)
add_executable(t_xaod_helpers tests/t_xaod_helpers.cpp)
target_link_libraries(t_xaod_helpers wraper_generators gtest_main)

include(GoogleTest)
gtest_discover_tests(t_type_helpers)
gtest_discover_tests(t_xaod_helpers)

# Executables for running the translation
add_executable(generate_types bin/generate_types.cpp)
target_link_libraries(generate_types ROOT::Core wraper_generators)

# Some definitions for CPACK (incase we need those eventually)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
