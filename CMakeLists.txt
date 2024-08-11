cmake_minimum_required(VERSION 3.10)

project(wordle VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(wordle src/main.cpp)

target_include_directories(wordle PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

target_sources(wordle PRIVATE ${SOURCES})

