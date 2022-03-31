cmake_minimum_required(VERSION 2.8)
project (gpiod_eg)
message(STATUS "Base dir = ${CMAKE_SOURCE_DIR}")
add_executable(${PROJECT_NAME}
${CMAKE_SOURCE_DIR}/gpiod_eg.cpp
)
target_link_libraries(${PROJECT_NAME}
gpiod)
