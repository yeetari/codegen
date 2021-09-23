cmake_minimum_required(VERSION 3.21)
project(codegen CXX)

option(CODEGEN_BUILD_EXAMPLE "Build example" OFF)

find_package(fmt REQUIRED)
add_library(codegen)
add_subdirectory(sources)
target_compile_features(codegen PRIVATE cxx_std_20)
target_include_directories(codegen PUBLIC include)
target_link_libraries(codegen PRIVATE fmt::fmt)

if(CODEGEN_BUILD_EXAMPLE)
    add_executable(codegen-example)
    add_subdirectory(example)
    target_compile_features(codegen-example PRIVATE cxx_std_20)
    target_link_libraries(codegen-example PRIVATE codegen)
endif()
