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

if(WIN32)
    set(WIN32_CPP_RENDERER "source/renderer/BackendDx.cpp")
    message("Salut")
else()
    set(WIN32_CPP_RENDERER "")
endif(WIN32)

add_library(common
        source/common/File.cpp
        )

add_library(renderer
        source/renderer/BackendOgl.cpp
        source/renderer/BackendOglEs.cpp
        
        ${WIN32_CPP_RENDERER}

        source/renderer/DeviceDesktop.cpp

        libraries/GL/src/glad.c)

add_executable(Imperium source/main.cpp)

target_link_libraries(Imperium renderer common)

target_link_libraries(renderer glfw OpenGL::GL)
target_link_libraries(renderer spirv-cross-core spirv-cross-cpp spirv-cross-hlsl spirv-cross-glsl)

# Platform dependent dependencies
if(UNIX)
    target_link_libraries(renderer -ldl -lGLESv2)
endif(UNIX)

if(WIN32)
    # TODO: directx is linked in source code, maybe something cleaner than
    # #pragma comment(lib, "d3d11.lib")
    # #pragma comment(lib, "d3dx11.lib")
    # #pragma comment(lib, "dxguid.lib")
    # This doesn't work obviously
    # target_link_libraries(renderer d3d11 d3dx11 dxguid)
endif(WIN32)

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