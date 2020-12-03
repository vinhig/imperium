cmake_minimum_required(VERSION 3.16)
project(Imperium)

set(CMAKE_CXX_STANDARD 17)

# Platform independent dependencies
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(Threads REQUIRED)

# Sub-projects
add_subdirectory(libraries/spirv-cross/)

add_library(openfbx
        libraries/openfbx/src/miniz.c libraries/openfbx/src/miniz.h
        libraries/openfbx/src/ofbx.cpp libraries/openfbx/src/ofbx.h
        )

add_executable(Imperium
        source/main.cpp

        source/renderer/BackendOgl.cpp
        source/renderer/BackendDx.cpp
        source/renderer/DeviceDesktop.cpp
        source/renderer/RenderingPass.cpp
        source/renderer/Frame.cpp
        libraries/gl/src/glad.c

        source/logic/CCamera.cpp
        source/logic/CDirectionalLight.cpp
        source/logic/CMaterial.cpp
        source/logic/CMeshInstance.cpp
        source/logic/CTransform.cpp
        source/logic/CViewport.cpp
        source/logic/Game.cpp
        source/logic/TryHarder.cpp

        source/loader/MeshLoader.cpp
        source/loader/TextureLoader.cpp
        source/loader/pch.cpp

        source/common/File.cpp
        )

# 3rd party libraries
target_link_libraries(Imperium openfbx glm glfw OpenGL::GL spirv-cross-core spirv-cross-cpp spirv-cross-hlsl spirv-cross-glsl)

if (UNIX)
    target_link_libraries(Imperium -ldl)
endif (UNIX)


# 3rd party includes
target_include_directories(Imperium PUBLIC libraries/)
target_include_directories(Imperium PUBLIC libraries/toml11/)
target_include_directories(Imperium PUBLIC libraries/GL/include/)
target_include_directories(Imperium PUBLIC libraries/openfbx/src/)
target_include_directories(Imperium PUBLIC libraries/bullet3/src/)
target_include_directories(Imperium PUBLIC libraries/imgui/)
target_include_directories(Imperium PUBLIC libraries/imgui/backends/)

# Shaders compilation
file(GLOB_RECURSE GLSL_SOURCE_FILES "source/shaders/*.glsl")

message(GLSL_SOURCE_FILES)

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

add_custom_target(ImperiumShaders DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(Imperium ImperiumShaders)