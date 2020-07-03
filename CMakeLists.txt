cmake_minimum_required(VERSION 3.6.0)

project(ObjectTracking LANGUAGES C CXX)

find_package( OpenCV REQUIRED )
find_package(X11 REQUIRED)

add_compile_options(
    # -Wall
)

file(GLOB HEADERS "include/*.hpp")
file(GLOB SOURCES  "src/*.cpp")

if(OpenCV_FOUND)
    include_directories( ${OpenCV_INCLUDE_DIRS} )
    include_directories(${X11_INCLUDE_DIR})
    include_directories(${PROJECT_SOURCE_DIR})
    add_executable(  ObjectTracking ${SOURCES} ${HEADERS} )
    target_link_libraries( ObjectTracking ${OpenCV_LIBS} ${X11_LIBRARIES})
else()
    message(STATUS "OpenCV not found...")
endif()
