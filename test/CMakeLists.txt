include_directories(..)

set(src
  test.cpp
)

add_executable(test ${src})
target_link_libraries(test isl-cpp)

add_executable(test-schedule EXCLUDE_FROM_ALL test-schedule.cpp)
target_link_libraries(test-schedule isl-cpp)

add_executable(test-schedule2 EXCLUDE_FROM_ALL test-schedule2.cpp)
target_link_libraries(test-schedule2 isl-cpp)

add_executable(test-buf-size EXCLUDE_FROM_ALL test-buf-size.cpp)
target_link_libraries(test-buf-size isl-cpp)

