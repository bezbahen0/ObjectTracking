cmake_minimum_required(VERSION 3.6.0)

project(grabWindowName LANGUAGES C CXX)

find_package(X11 REQUIRED)

include_directories(${X11_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR})
add_executable(grabWindowName  grabWindowName.cpp)
target_link_libraries( grabWindowName ${X11_LIBRARIES})
