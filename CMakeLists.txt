cmake_minimum_required(VERSION 2.8.12)

project(isl-cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(pluto_dir "~/code/pluto-0.11.0")

include_directories(
  "${pluto_dir}/isl/include"
)
link_directories(
  "${pluto_dir}/isl/.libs"
)

add_subdirectory(test)
