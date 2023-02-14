cmake_minimum_required(VERSION 3.20)
project(glbToUnlitGlb)

set(CMAKE_CXX_STANDARD 14)
add_executable(glbToUnlitGlb  cgltf_write.h main.cpp)
