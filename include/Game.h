#ifndef GAME_H
#define GAME_H

#include "MazeGraph.h"
#include "CameraController.h"
#include "Renderer3D.h"
#include "MinimapRenderer.h"

class Game {
public:
    Game(int mazeWidth, int mazeHeight);
    ~Game();
    void Run();
    
private:
    void HandleInput();
    void Update();
    void Draw();
    void MoveToRoom(Room* newRoom);
    Direction GetHoveredDoor();
    
    MazeGraph m_maze;
    Room* m_currentRoom;
    CameraController m_camera;
    Renderer3D m_renderer3D;
    MinimapRenderer m_minimap;
    
    bool m_showMinimap;
    bool m_showDebug;
    bool m_devMode;
};

#endif