
set(src
  ../context.cpp
  test.cpp
)

add_executable(test ${src})
target_link_libraries(test isl)
