cmake_minimum_required(VERSION 3.15)
project(MazeExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)

add_subdirectory(third_party/raylib)
add_subdirectory(scr)