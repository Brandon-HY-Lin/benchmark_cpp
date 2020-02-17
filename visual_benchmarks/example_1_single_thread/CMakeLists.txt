cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(benchmark_example_2)

add_executable(benchmark_example_2 benchmark_example_2.cpp)

set_property(TARGET benchmark_example_2 PROPERTY CXX_STANDARD 11)
