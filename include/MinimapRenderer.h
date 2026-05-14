#ifndef MINIMAP_RENDERER_H
#define MINIMAP_RENDERER_H

#include "MazeGraph.h"

class MinimapRenderer {
public:
    MinimapRenderer() = default;
    void Draw(bool showDebug, Room* currentRoom, float fwdX, float fwdZ, MazeGraph* maze);
    
private:
    void DrawMinimap(Room* currentRoom, float fwdX, float fwdZ, MazeGraph* maze);
    void DrawDebugMap(Room* currentRoom, float fwdX, float fwdZ, MazeGraph* maze);
};

#endif