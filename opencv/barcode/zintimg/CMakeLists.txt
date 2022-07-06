cmake_minimum_required(VERSION 2.8)
project( zintimg )
message(STATUS "Base dir = ${CMAKE_SOURCE_DIR}")
find_package(wxWidgets COMPONENTS core net base REQUIRED)
find_package(OpenCV REQUIRED)
include("${wxWidgets_USE_FILE}")
include_directories(${OpenCV_INCLUDE_DIR})
add_executable( ${PROJECT_NAME} 
    ${CMAKE_SOURCE_DIR}/zintimg.cpp
)
target_link_libraries(${PROJECT_NAME} 
    ${wxWidgets_LIBRARIES} 
    ${OpenCV_LIBS}
    zint
)
