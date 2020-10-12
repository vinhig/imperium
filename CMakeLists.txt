cmake_minimum_required(VERSION 3.16)
project(Imperium)

set(CMAKE_CXX_STANDARD 17)

# Platform independent dependencies
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(Threads REQUIRED)

# 3rd party includes
include_directories(libraries/GL/include/)

# Sub-projects
add_subdirectory(libraries/spirv-cross)

add_library(renderer
        source/renderer/BackendOgl.cpp
        source/renderer/BackendOglEs.cpp

        source/renderer/DeviceDesktop.cpp

        source/common/File.cpp

        libraries/GL/src/glad.c)

add_executable(Imperium source/main.cpp)

target_link_libraries(Imperium renderer)

target_link_libraries(renderer glfw OpenGL::OpenGL)
target_link_libraries(renderer spirv-cross-core spirv-cross-cpp spirv-cross-hlsl spirv-cross-glsl)

# Platform dependent dependencies
if (UNIX)
    target_link_libraries(renderer -ldl -lGLESv2)
endif (UNIX)

# Assets compilation
file(GLOB_RECURSE GLSL_SOURCE_FILES "source/shaders/*.glsl")

foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/assets/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND cmake -E make_directory "${PROJECT_BINARY_DIR}/assets/"
            COMMAND glslangValidator -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach ()

add_custom_target(shaders DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(Imperium shaders)