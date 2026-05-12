#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <raylib.h>
#include <vector>
#include <string>
#include "MazeGraph.h"

class GameRenderer {
public:
    GameRenderer(int mazeWidth, int mazeHeight);
    ~GameRenderer();
    void Run();

private:
    void HandleInput();
    void Update();
    void Draw();

    void DrawRoom(Room* room);
    void DrawDoors3D(Room* room);
    void DrawMinimap();
    void DrawDebugMap();
    void DrawCrosshair();
    void DrawDebugInfo();

    void MoveToRoom(Room* newRoom);
    Direction GetDoorFromRay(Ray ray, Room* room);
    Direction GetDirectionFromAngle(float angle);
    bool IsLookingAtDoor(Direction dir, float toleranceDegrees);
    void NextFloor();
    void ShowMessage(const std::string& text, float duration, bool fullscreen = false);
    void CheckItemPickup();

    MazeGraph m_maze;
    Room* m_currentRoom;

    Camera3D m_camera;
    float m_cameraAngle = 0.0f;
    float m_cameraPitch = 0.0f;

    bool m_showMinimap = true;
    bool m_showDebug = false;
    bool m_paused = false;

    int m_currentFloor = 0;
    int m_itemsCollected = 0;
    int m_itemsRequired = 3;
    std::vector<std::vector<Vector3>> m_itemPositions;

    struct TempMessage {
        std::string text;
        float timer;
        bool fullscreen;
    };
    TempMessage m_tempMessage{"", 0.0f, false};

    const float ROOM_SIZE = 8.0f;
    const float WALL_HEIGHT = 3.0f;
    const float CAMERA_HEIGHT = 1.7f;
    const float DOOR_WIDTH = 3.0f;
    const float DOOR_HEIGHT = 2.8f;
    const float WALL_THICKNESS = 0.3f;
};

#endif