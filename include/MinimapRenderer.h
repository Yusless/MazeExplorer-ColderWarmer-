#ifndef MINIMAP_RENDERER_H
#define MINIMAP_RENDERER_H

#include "MazeGraph.h"

class MinimapRenderer {
public:
    MinimapRenderer(MazeGraph& maze);
    void Draw(bool showDebug, Room* currentRoom, float fwdX, float fwdZ);
    
private:
    MazeGraph& maze;
    void DrawMinimap(Room* currentRoom, float fwdX, float fwdZ);
    void DrawDebugMap(Room* currentRoom, float fwdX, float fwdZ);
};

#endif