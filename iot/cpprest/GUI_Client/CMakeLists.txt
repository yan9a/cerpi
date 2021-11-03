cmake_minimum_required(VERSION 3.9)
project(GUI_Client)

# for RPi
# set ( CMAKE_PREFIX_PATH /usr/lib/arm-linux-gnueabihf/cmake/ )

# for x86_64
# set ( CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake/)

# find_package(Boost COMPONENTS filesystem system REQUIRED)

find_package(cpprestsdk REQUIRED)
find_package(wxWidgets COMPONENTS core net base REQUIRED)
include("${wxWidgets_USE_FILE}")
add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
target_link_libraries(${PROJECT_NAME} 
 ${wxWidgets_LIBRARIES}
 cpprestsdk::cpprest 
 # ${Boost_SYSTEM_LIBRARY}
 )
