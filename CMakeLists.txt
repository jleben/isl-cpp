cmake_minimum_required(VERSION 2.8.12)

project(isl-cpp)

option(ISL_CPP_BUILD_TESTING "Build tests." OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(pluto_dir "~/code/pluto-0.11.0")
#set(pluto_dir "~/code/pluto")

set(isl_include_dir "${pluto_dir}/isl/include" CACHE PATH "ISL include directory")
set(isl_library_dir "${pluto_dir}/isl/.libs" CACHE PATH "ISL library directory")

include_directories(
  ${isl_include_dir}
)
link_directories(
  ${isl_library_dir}
)

set(sources
  context.cpp
  matrix.cpp
)

add_library(isl-cpp STATIC ${sources})
target_link_libraries(isl-cpp isl)

if(ISL_CPP_BUILD_TESTING)
  add_subdirectory(test)
endif()
