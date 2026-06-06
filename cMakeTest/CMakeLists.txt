cmake_minimum_required(VERSION 3.29)
project(getting_started)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    "${PROJECT_NAME}"
    main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_EXECUTABLE_SUFFIX ".html")