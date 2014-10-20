
set(src
  test.cpp
)

add_executable(test ${src})

target_link_libraries(test isl-cpp)
