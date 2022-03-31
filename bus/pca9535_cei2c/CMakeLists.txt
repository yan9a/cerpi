cmake_minimum_required(VERSION 2.8)
project (pca9535_cei2c)
# set(CMAKE_CXX_STANDARD 20)
message(STATUS "Base dir = ${CMAKE_SOURCE_DIR}")
find_package(wxWidgets COMPONENTS core net base REQUIRED)
include("${wxWidgets_USE_FILE}")
# include_directories( /usr/local/include )
add_executable(${PROJECT_NAME}
${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME}.cpp
)
target_link_libraries(${PROJECT_NAME} ceUtil
${wxWidgets_LIBRARIES})
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)