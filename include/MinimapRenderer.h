#ifndef MINIMAP_RENDERER_H
#define MINIMAP_RENDERER_H

#include "MazeGraph.h"

class MinimapRenderer {
public:
    MinimapRenderer(MazeGraph& maze);
    void Draw(bool showDebug, Room* currentRoom, float cameraAngle, float cameraPitch);
    
private:
    MazeGraph& maze;
    void DrawMinimap(Room* currentRoom, float cameraAngle, float cameraPitch);
    void DrawDebugMap(Room* currentRoom, float cameraAngle, float cameraPitch);
};

#endif