cmake_minimum_required(VERSION 3.13)  # CMake version check
project(beatles_ai)               # Create project "beatles_ai"

set(CMAKE_C_COMPILER C:/msys64/ucrt64/bin/g++)
set(CMAKE_CXX_COMPILER C:/msys64/ucrt64/bin/g++)

# Add main.cpp file of the project root directory as a source file
set(SOURCE_FILES main.cpp audio.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(beatles_ai ${SOURCE_FILES})

set_target_properties(beatles_ai PROPERTIES OUTPUT_NAME "beatles_ai")