cmake_minimum_required(VERSION 3.0)
project(networking)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(networking_sources 
  src/networking.cpp src/networking.h
  src/main.cpp
)

add_executable(networking ${networking_sources})
target_include_directories(networking PRIVATE src)
