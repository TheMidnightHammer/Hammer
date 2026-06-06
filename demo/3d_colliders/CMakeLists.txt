# Copyright (c) 2026 MidnightHammer-code
# This source code is licensed under the GPL 3.0 license

cmake_minimum_required(VERSION 3.10)
project(voxel_Engine VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    test.cpp
    ../../src/engine.cpp
    ../../src/utility.cpp
    ../../src/camera.cpp
    ../../src/rect.cpp
    ../../src/buffers.cpp
    ../../src/pipeline.cpp
    ../../src/ssbo.cpp
    ../../src/AsyncLogger.cpp
)

find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED) # Standard for most package managers
find_package(glfw3 REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_definitions(voxel_Engine PUBLIC GLM_ENABLE_EXPERIMENTAL)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
)

if(WIN32 AND NOT MSVC)
    # Static link standard libs so the .exe runs without needing MSYS2 DLLs
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
    
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
endif()