cmake_minimum_required(VERSION 3.0.0)
project(func_adl_types_atlas VERSION 0.1.0)

find_package(ROOT REQUIRED COMPONENTS Core)

# Get Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/075810f7a20405ea09a93f68847d6e963212fa62.zip
#  DOWNLOAD_EXTRACT_TIMESTAMP TRUE - Needed to suppress warning in R21
)
FetchContent_MakeAvailable(googletest)

include(CTest)
enable_testing()

# YAML parsing
include(FetchContent)
FetchContent_Declare(
  yaml-cpp
  URL https://github.com/jbeder/yaml-cpp/archive/0579ae3d976091d7d664aa9d2527e0d0cff25763.zip
#  DOWNLOAD_EXTRACT_TIMESTAMP TRUE - Needed to suppress arning in R22
)
FetchContent_MakeAvailable(yaml-cpp)

# Boost
find_package(Boost COMPONENTS program_options REQUIRED)

# Package setup
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# One main library
add_library(wraper_generators
            src/normalize.cpp
            src/class_info.cpp
            src/type_helpers.cpp
            src/utils.cpp
            src/util_string.cpp
            src/xaod_helpers.cpp
            src/helper_files.cpp
            src/metadata_file_finder.cpp
            src/translate.cpp
            )
target_link_libraries(wraper_generators ROOT::Core yaml-cpp)

# Tests
add_executable(t_type_helpers tests/t_type_helpers.cpp)
target_link_libraries(t_type_helpers wraper_generators gtest_main)
add_executable(t_xaod_helpers tests/t_xaod_helpers.cpp)
target_link_libraries(t_xaod_helpers wraper_generators gtest_main)
add_executable(t_class_info tests/t_class_info.cpp)
target_link_libraries(t_class_info wraper_generators gtest_main)
add_executable(t_translate tests/t_translate.cpp)
target_link_libraries(t_translate wraper_generators gtest_main)

include(GoogleTest)
gtest_discover_tests(t_type_helpers)
gtest_discover_tests(t_xaod_helpers)
gtest_discover_tests(t_class_info)
gtest_discover_tests(t_translate)

# Executables for running the translation
add_executable(generate_types bin/generate_types.cpp)
target_link_libraries(generate_types ROOT::Core wraper_generators Boost::program_options)

# Some definitions for CPACK (incase we need those eventually)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
