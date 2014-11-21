include_directories(..)

set(src
  test.cpp
)

add_executable(test ${src})
target_link_libraries(test isl-cpp)

add_executable(test-schedule EXCLUDE_FROM_ALL test-schedule.cpp)
target_link_libraries(test-schedule isl-cpp)
