# Copyright (c) 2026 MidnightHammer-code
# This source code is licensed under the GPL 3.0 license
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.10)

project(voxel_Engine VERSION 2.0)

set(SOURCE_FILES
    test.cpp
    ../../src/engine.cpp
    ../../src/utility.cpp
    ../../src/camera.cpp
    ../../src/rect.cpp
    ../../src/buffers.cpp
    ../../src/pipeline.cpp
)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

find_package(glfw3 REQUIRED)

include_directories(
    ${GLFW3_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_definitions(voxel_Engine PUBLIC GLM_ENABLE_EXPERIMENTAL)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
    vulkan
    glfw
)

if(WIN32 AND NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
    
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
endif()
