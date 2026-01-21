cmake_minimum_required(VERSION 3.15) # 3.15+ is better
project(TrailEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------
# Opzioni globali utili
# --------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --------------------
# GLAD
# --------------------
add_library(glad STATIC
    src/external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/external/glad/include
)

# --------------------
# GLFW (build from source)
# --------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)  # dont install
add_subdirectory(src/external/GLFW)

# --------------------
# OpenGL (per GLRender)
# --------------------
find_package(OpenGL REQUIRED)

# --------------------
# TrailEngine Executable
# --------------------
add_executable(TrailEngine
    src/main.cpp
    src/graphics/Window.cpp
    src/core/render/d3d11/DXRender.cpp
    # src/core/render/opengl/GLRender.cpp BREAKS the compiler
    # src/core/render/vulkan/VKRender.cpp  # uncomment when ready
)


#file(GLOB_RECURSE EXTRA_SOURCES
#    "src/*.cpp"
#    "src/*.h"
#)
#target_sources(TrailEngine PRIVATE ${EXTRA_SOURCES})

# Include directories
target_include_directories(TrailEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/external/GLFW/include   # ← FIX PRINCIPALE per GLFW/glfw3.h
)

# Link libraries
target_link_libraries(TrailEngine PRIVATE
    glfw                # GLFW static/dynamic
    glad                # GL loader
    OpenGL::GL          # OpenGL per GLRender
)

# DirectX 11 dependencies
if(WIN32)
    target_link_libraries(TrailEngine PRIVATE
        d3d11
        dxgi
        dxguid
        d3dcompiler
    )
endif()

# --------------------
# Defines utili (opzionale ma raccomandato)
# --------------------
target_compile_definitions(TrailEngine PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    GLFW_INCLUDE_NONE   # utile se usi GLAD + GLFW (evita che GLFW includa gl.h)
)

# --------------------
# Copia runtime files (opzionale: DLL di GLFW se build dynamic)
# --------------------
if(WIN32 AND NOT GLFW_STATIC)
    add_custom_command(TARGET TrailEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:glfw>
            $<TARGET_FILE_DIR:TrailEngine>
    )
endif()