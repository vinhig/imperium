cmake_minimum_required(VERSION 3.16)
project(Imperium)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(Threads REQUIRED)

include_directories(libraries/GL/include/)

add_library(renderer
        source/renderer/BackendOgl.cpp
        source/renderer/BackendOglEs.cpp

        source/renderer/DeviceDesktop.cpp

        libraries/GL/src/glad.c)

add_executable(Attempt source/main.cpp)

target_link_libraries(Attempt renderer)

target_link_libraries(renderer glfw OpenGL::OpenGL)

if (UNIX)
    target_link_libraries(renderer -ldl -lGLESv2)
endif (UNIX)