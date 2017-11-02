# project basic config
cmake_minimum_required(VERSION 2.8)
project(ENSeg)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -W -Wall -Wextra -O3 -fPIC -c")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pipe -g3 -fPIC")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

file(GLOB_RECURSE src_HEADERS "src/*.h")
file(GLOB_RECURSE src_SRCS "src/*.cc")

# compile
add_library(en_seg SHARED ${src_HEADERS} ${src_SRCS})
add_executable(en_seg_test ${src_HEADERS} ${src_SRCS})
target_link_libraries(en_seg_test boost_regex)