cmake_minimum_required(VERSION 3.10.0)
project(Engine VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Engine
    src/main.cpp
    src/core/render/render.cpp
    src/win32/window/Window.cpp
)

target_include_directories(Engine PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(Engine PROPERTIES
    WIN32_EXECUTABLE TRUE          # ← This is the key line!
)
