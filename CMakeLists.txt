cmake_minimum_required(VERSION 3.25)
project(ray)

set(CMAKE_CXX_STANDARD 20)

# raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/raylib)

# imgui
add_library(imgui STATIC
        lib/imgui/imgui.cpp
        lib/imgui/imgui_demo.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
)
target_include_directories(imgui PUBLIC lib/imgui)

# rlImGui
add_library(rlImGui STATIC
        lib/rlImGui/rlImGui.cpp
)
target_include_directories(rlImGui PUBLIC lib/rlImGui)
target_link_libraries(rlImGui PUBLIC raylib imgui)

add_executable(ray main.cpp main.h)
target_link_libraries(ray PRIVATE raylib imgui rlImGui)
target_include_directories(ray PRIVATE lib/raylib/src lib/rlImGui lib/imgui)
