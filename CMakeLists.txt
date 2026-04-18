cmake_minimum_required(VERSION 3.15)
project(MazeExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES main.cpp MazeGraph.cpp GameRenderer.cpp)
set(HEADERS Room.h MazeGraph.h GameRenderer.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)