cmake_minimum_required(VERSION 3.0)

project(isl-cpp)

option(ISL_CPP_BUILD_TESTING "Build tests." OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_path(ISL_INCLUDE_DIR isl/ctx.h)
find_library(ISL_LIBRARY isl)
if(NOT ISL_INCLUDE_DIR OR NOT ISL_LIBRARY)
  message(FATAL_ERROR "isl-cpp: isl not found")
endif()

set(sources
  context.cpp
  matrix.cpp
)

add_library(isl-cpp STATIC ${sources})
target_link_libraries(isl-cpp ${ISL_LIBRARY})
target_include_directories(isl-cpp PUBLIC ${ISL_INCLUDE_DIR})

if(ISL_CPP_BUILD_TESTING)
  add_subdirectory(test)
endif()
