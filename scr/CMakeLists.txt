set(SOURCES
    main.cpp
    Game.cpp
    InputHandler.cpp
    CameraController.cpp
    Renderer3D.cpp
    MinimapRenderer.cpp
    MazeGraph.cpp
    Room.cpp
    Wall.cpp
    Floor.cpp
    Door.cpp
)

add_executable(MazeExplorer ${SOURCES})

target_include_directories(MazeExplorer PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MazeExplorer PRIVATE raylib)

if(WIN32)
    target_link_libraries(MazeExplorer PRIVATE winmm)
endif()