#ifndef RENDERER_3D_H
#define RENDERER_3D_H

#include "MazeGraph.h"
#include "Room.h"
#include "CameraController.h"
#include <raylib.h>

class Renderer3D {
public:
    Renderer3D();
    void DrawRoom(Room* room);
    void DrawDoors(Room* room, const Camera3D& camera, Direction& outHoveredDoor);
    
private:
    void DrawWall(Vector3 pos, Vector3 size, Color color);
    void DrawFloor(Vector3 center);
    void DrawCeiling(Vector3 center);
};

#endif