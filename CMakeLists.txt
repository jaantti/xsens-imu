cmake_minimum_required (VERSION 3.1)
project (XSensAPI)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(xstypes)
add_subdirectory(xcommunication)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("include")

add_executable(example example.cpp conio.c deviceclass.cpp xsensImu.cpp)
target_link_libraries(example Threads::Threads)
target_link_libraries(example xstypes)
target_link_libraries(example xcommunication)
