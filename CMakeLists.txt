cmake_minimum_required(VERSION 3.0.0)
project(func_adl_types_atlas VERSION 0.1.0)

find_package(ROOT REQUIRED COMPONENTS Core)

# include(CTest)
# enable_testing()

# Package setup
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# One main library
add_library(wraper_generators
            src/normalize.cpp
            src/translate.cpp)
target_link_libraries(wraper_generators ROOT::Core)

# Executables for running the translation
add_executable(generate_types bin/generate_types.cpp)
target_link_libraries(generate_types ROOT::Core wraper_generators)

# Some definitions for CPACK (incase we need those eventually)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
