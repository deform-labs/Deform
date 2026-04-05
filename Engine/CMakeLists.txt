file(GLOB_RECURSE ENGINE_SOURCES
    "src/Engine/*.cpp"
    "src/Engine/*.h"
    "src/*.h"
)

add_library(Engine SHARED ${ENGINE_SOURCES})

target_include_directories(Engine PUBLIC
    src
    external/GLFW/include
    external/UUID/include
    external/DDSTextureLoader
)

target_link_libraries(Engine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/GLFW/lib-vc2022/glfw3.lib"
    d3d11.lib
    dxgi.lib
    d3dcompiler.lib
    opengl32.lib
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Engine/$<CONFIG>/Runtime)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Engine/$<CONFIG>/Library)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Engine/$<CONFIG>/Archive)

target_compile_definitions(Engine PRIVATE DEFORM_ENGINE_EXPORTS)