cmake_minimum_required(VERSION 3.16)
project(lucy LANGUAGES CXX)

add_library(lucy INTERFACE)
target_include_directories(lucy INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(lucy INTERFACE cxx_std_17)
