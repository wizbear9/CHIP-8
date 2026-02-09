cmake_minimum_required(VERSION 3.10.0)
project(CHIP-8 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

add_subdirectory(libs/SDL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL/include)
add_executable(CHIP-8
    include/chip8.h
    include/display.h
    src/main.cpp
    src/chip8.cpp
    src/display.cpp)

# Link to the actual SDL3 library.
target_link_libraries(CHIP-8 PRIVATE SDL3::SDL3)