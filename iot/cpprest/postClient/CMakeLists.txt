cmake_minimum_required(VERSION 3.9)
project(postClient)

# for RPi
# set ( CMAKE_PREFIX_PATH /usr/lib/arm-linux-gnueabihf/cmake/ )

# for x86_64
# set ( CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake/)

# find_package(Boost COMPONENTS filesystem system REQUIRED)

find_package(cpprestsdk REQUIRED)
add_executable(postClient postClient.cpp)
target_link_libraries(postClient 
 cpprestsdk::cpprest 
 # ${Boost_SYSTEM_LIBRARY}
 )
