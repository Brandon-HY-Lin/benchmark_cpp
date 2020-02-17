cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(visual_benchmark_multi_thread)

find_package(Threads)

add_executable(visual_benchmark_multi_thread visual_benchmark_multi_thread.cpp)

target_link_libraries(visual_benchmark_multi_thread ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET visual_benchmark_multi_thread PROPERTY CXX_STANDARD 11)
