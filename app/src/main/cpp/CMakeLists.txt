cmake_minimum_required(VERSION 3.4.1)
project(Imperium)

set(CMAKE_CXX_STANDARD 17)

add_library(
        native-lib

        SHARED

        native-lib.cpp
        ../../../../source/renderer/BackendOglEs.cpp
        ../../../../source/renderer/DeviceAndroid.cpp
)

# Sub-projects
add_subdirectory(../../../../libraries/spirv-cross/ tmp-build)

target_include_directories(native-lib PRIVATE ../../../../source/)
target_include_directories(native-lib PRIVATE C:/Users/vinhi/source/repos/vcpkg/packages/glm_x64-windows/include/)

find_library(log-lib log)

target_link_libraries(native-lib ${log-lib} -lGLESv3 -landroid)
target_link_libraries(native-lib spirv-cross-core spirv-cross-cpp spirv-cross-hlsl spirv-cross-glsl)

# Compile and copy shaders
file(GLOB_RECURSE GLSL_SOURCE_FILES "../../../../source/shaders/*.glsl")

foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_CURRENT_SOURCE_DIR}/../../debug/assets/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND cmake -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../../debug/assets/"
            COMMAND glslangValidator -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach ()

add_custom_target(shaders DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(native-lib shaders)