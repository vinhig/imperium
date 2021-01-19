cmake_minimum_required(VERSION 3.16)
project(Imperium)

set(CMAKE_CXX_STANDARD 17)

# Platform independent dependencies
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(libraries/vk-boostrap)

add_library(openfbx
        libraries/openfbx/src/miniz.c libraries/openfbx/src/miniz.h
        libraries/openfbx/src/ofbx.cpp libraries/openfbx/src/ofbx.h
        )

add_executable(Imperium
        source/main.cpp

        source/loader/Loader.cpp
        source/loader/Model.cpp

        source/logic/Game.cpp

        source/render/backend/BackendVulkan.cpp
        source/render/Context.cpp
        source/render/DeviceDesktop.cpp
        )

# 3rd party libraries
target_link_libraries(Imperium openfbx Vulkan::Vulkan vk-bootstrap OpenGL::GL SDL2)

if (UNIX)
    target_link_libraries(Imperium -ldl)
endif (UNIX)


# 3rd party includes
target_include_directories(Imperium PUBLIC source/)
target_include_directories(Imperium PUBLIC libraries/)
target_include_directories(Imperium PUBLIC libraries/vma/src)

# Shaders compilation
file(GLOB_RECURSE GLSL_SOURCE_FILES "source/shaders/*.glsl")

message(GLSL_SOURCE_FILES)

foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/assets/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND cmake -E make_directory "${PROJECT_BINARY_DIR}/assets/"
            COMMAND glslangValidator -V100 ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach ()

add_custom_target(ImperiumShaders DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(Imperium ImperiumShaders)