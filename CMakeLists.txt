cmake_minimum_required(VERSION 3.21)
project(codegen CXX)

find_package(fmt REQUIRED)
add_executable(codegen)
target_compile_features(codegen PRIVATE cxx_std_20)
target_link_libraries(codegen PRIVATE fmt::fmt)
add_subdirectory(src)
