cmake_minimum_required(VERSION 3.16)
project(Imperium)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(Threads REQUIRED)

include_directories(libraries/)

add_library(renderer source/renderer/BackendOgl.cpp libraries/glad/src/glad.c)

add_executable(Attempt source/main.cpp)