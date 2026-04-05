file(GLOB_RECURSE EDITOR_SOURCES
    "src/*.cpp"
)

set(IMGUI_SOURCES
    "external/imgui/imgui.cpp"
    "external/imgui/imgui_draw.cpp"
    "external/imgui/imgui_tables.cpp"
    "external/imgui/imgui_widgets.cpp"
    "external/imgui/backends/imgui_impl_dx11.cpp"
    "external/imgui/backends/imgui_impl_glfw.cpp"
)

add_executable(Editor ${EDITOR_SOURCES} ${IMGUI_SOURCES})

target_include_directories(Editor PRIVATE
    src
    external/imgui
    external/imgui/backends
    external/UUID/include
    external/GLFW/include
    external
    ${CMAKE_SOURCE_DIR}/Engine/src
)

target_link_libraries(Editor PRIVATE 
    Engine
    "${CMAKE_CURRENT_SOURCE_DIR}/external/GLFW/lib-vc2022/glfw3.lib"
)

set_property(TARGET Editor PROPERTY CXX_STANDARD 23)