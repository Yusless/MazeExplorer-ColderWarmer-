cmake_minimum_required(VERSION 3.15)
cmake_minimum_required(VERSION 3.15)
project(MazeExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES main.cpp MazeGraph.cpp GameRenderer.cpp)
set(HEADERS Room.h MazeGraph.h GameRenderer.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    raylib
    opengl32
    gdi32
    winmm
)

target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)

# Никакого автоматического копирования DLL!