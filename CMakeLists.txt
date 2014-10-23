cmake_minimum_required(VERSION 2.8.12)

project(isl-cpp)

option(ISL_CPP_BUILD_TESTING "Build tests." OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(pluto_dir "~/code/pluto-0.11.0")
#set(pluto_dir "~/code/pluto")

include_directories(
  "${pluto_dir}/isl/include"
)
link_directories(
  "${pluto_dir}/isl/.libs"
)

set(sources
  context.cpp
)

add_library(isl-cpp STATIC ${sources})
target_link_libraries(isl-cpp isl)

if(ISL_CPP_BUILD_TESTING)
  add_subdirectory(test)
endif()
