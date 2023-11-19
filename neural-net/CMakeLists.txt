cmake_minimum_required(VERSION 3.13)  # CMake version check
project(beatles_ai)                    # Project name

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

set(CMAKE_FIND_ROOT_PATH "C:/msys64/mingw64")
set(BOOST_ROOT "C:/msys64/mingw64/lib/cmake") # full path just to figure this out

message(STATUS "BOOST_ROOT= ${BOOST_ROOT}")

include_directories(${BOOST_ROOT})

# Add the main.cpp file as a source file
set(SOURCE_FILES main.cpp audio.cpp neural_net.cpp helper.cpp)
# Create the executable
add_executable(beatles_ai ${SOURCE_FILES})

link_directories("C:/msys64/mingw64/lib")
link_directories(${Boost_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

target_include_directories(beatles_ai PRIVATE "C:/msys64/mingw64/include")
