#ifndef MINIMAP_RENDERER_H
#define MINIMAP_RENDERER_H

#include "MazeGraph.h"

class MinimapRenderer {
public:
    MinimapRenderer(MazeGraph& maze);
    void Draw(bool showDebug, Room* currentRoom, float cameraAngle);
    
private:
    MazeGraph& maze;
    void DrawMinimap(Room* currentRoom, float cameraAngle);
    void DrawDebugMap(Room* currentRoom, float cameraAngle);
};

#endif