cmake_minimum_required(VERSION 3.1)
project(xstypes)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Bring the headers, such as Student.h into the project
#include_directories(${PROJECT_BINARY_DIR})
include_directories("../include/xsens")
include_directories("../include")

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cpp")
file(GLOB CSOURCES "*.c")
set(SOURCES "${SOURCES};${CSOURCES}")

#Generate the shared library from the sources
add_library(xstypes STATIC ${SOURCES})
