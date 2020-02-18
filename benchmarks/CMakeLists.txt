cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(benchmarks)

add_executable(benchmarks benchmarks.cpp)
set_property(TARGET benchmarks PROPERTY CXX_STANDARD 11)
