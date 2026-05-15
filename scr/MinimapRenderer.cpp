#include "MinimapRenderer.h"
#include <raylib.h>
#include <algorithm>
#include <cmath>



void MinimapRenderer::Draw(bool showDebug, Room* currentRoom, float fwdX, float fwdZ, MazeGraph* maze) {
    if (!maze) return;
    if (showDebug)
        DrawDebugMap(currentRoom, fwdX, fwdZ, maze);
    else
        DrawMinimap(currentRoom, fwdX, fwdZ, maze);
}

static void MinimapViewDirectionOnGrid(float fwdX, float fwdZ, float& outDirX, float& outDirY) {
    outDirX = fwdX;
    outDirY = fwdZ;
    float len = sqrtf(outDirX * outDirX + outDirY * outDirY);
    if (len > 1e-6f) {
        outDirX /= len;
        outDirY /= len;
    }
}

void MinimapRenderer::DrawMinimap(Room* currentRoom, float fwdX, float fwdZ, MazeGraph* maze) {
    int mapSize = 200;
    int cellSize = mapSize / std::max(maze->GetWidth(), maze->GetHeight());
    int mapX = GetScreenWidth() - mapSize - 30;
    int mapY = 70;
    
    DrawRectangle(mapX - 10, mapY - 30, mapSize + 20, mapSize + 40, Fade(BLACK, 0.8f));
    DrawText("MINIMAP", mapX + mapSize/2 - 40, mapY - 25, 20, LIGHTGRAY);
    
    for (int y = 0; y < maze->GetHeight(); ++y) {
        for (int x = 0; x < maze->GetWidth(); ++x) {
            Room* room = maze->GetRoom(x, y);
            if (!room) continue;
            int drawX = mapX + x * cellSize;
            int drawY = mapY + y * cellSize;
            
            Color color;
            if (room == currentRoom) color = YELLOW;
            else if (room->IsExplored()) {
                if (room == maze->GetStartRoom()) color = GREEN;
                else if (room == maze->GetExitRoom()) color = RED;
                else color = {100,100,200,255};
            } else color = DARKGRAY;
            
            DrawRectangle(drawX, drawY, cellSize - 1, cellSize - 1, color);
            
            if (room->IsExplored() || room == currentRoom) {
                int cx = drawX + cellSize/2;
                int cy = drawY + cellSize/2;
                if (room->HasDoor(Direction::NORTH)) DrawLine(cx, drawY, cx, drawY - 3, WHITE);
                if (room->HasDoor(Direction::SOUTH)) DrawLine(cx, drawY + cellSize, cx, drawY + cellSize + 3, WHITE);
                if (room->HasDoor(Direction::EAST)) DrawLine(drawX + cellSize, cy, drawX + cellSize + 3, cy, WHITE);
                if (room->HasDoor(Direction::WEST)) DrawLine(drawX, cy, drawX - 3, cy, WHITE);
            }
        }
    }
    
    // Направление взгляда
    int roomX = mapX + currentRoom->GetGridX() * cellSize + cellSize/2;
    int roomY = mapY + currentRoom->GetGridY() * cellSize + cellSize/2;
    float arrowDirX = 0.0f, arrowDirY = 0.0f;
    MinimapViewDirectionOnGrid(fwdX, fwdZ, arrowDirX, arrowDirY);
    int arrowLen = cellSize * 3 / 5;
    int arrowX = roomX + (int)(arrowDirX * arrowLen);
    int arrowY = roomY + (int)(arrowDirY * arrowLen);
    DrawCircle(roomX, roomY, cellSize / 4, Fade(YELLOW, 0.35f));
    DrawLine(roomX, roomY, arrowX, arrowY, Fade(BLUE, 0.95f));
}

void MinimapRenderer::DrawDebugMap(Room* currentRoom, float fwdX, float fwdZ, MazeGraph* maze) {
    int mapSize = 250;
    int cellSize = mapSize / std::max(maze->GetWidth(), maze->GetHeight());
    int mapX = GetScreenWidth() - mapSize - 30;
    int mapY = 70;
    
    DrawRectangle(mapX - 10, mapY - 30, mapSize + 20, mapSize + 40, Fade(BLACK, 0.8f));
    DrawText("DEBUG MAP", mapX + mapSize/2 - 50, mapY - 25, 16, YELLOW);
    
    for (int y = 0; y < maze->GetHeight(); ++y) {
        for (int x = 0; x < maze->GetWidth(); ++x) {
            Room* room = maze->GetRoom(x, y);
            if (!room) continue;
            int drawX = mapX + x * cellSize;
            int drawY = mapY + y * cellSize;
            Color roomColor;
            if (room == currentRoom) roomColor = YELLOW;
            else if (room == maze->GetStartRoom()) roomColor = GREEN;
            else if (room == maze->GetExitRoom()) roomColor = RED;
            else if (room->IsExplored()) roomColor = {100,100,200,255};
            else roomColor = DARKGRAY;
            DrawRectangle(drawX, drawY, cellSize - 1, cellSize - 1, roomColor);
            
            int cx = drawX + cellSize/2;
            int cy = drawY + cellSize/2;
            int doorSize = cellSize / 4;
            if (room->HasDoor(Direction::NORTH)) DrawRectangle(cx - doorSize/2, drawY - doorSize/2, doorSize, doorSize, WHITE);
            if (room->HasDoor(Direction::SOUTH)) DrawRectangle(cx - doorSize/2, drawY + cellSize - doorSize/2, doorSize, doorSize, WHITE);
            if (room->HasDoor(Direction::EAST)) DrawRectangle(drawX + cellSize - doorSize/2, cy - doorSize/2, doorSize, doorSize, WHITE);
            if (room->HasDoor(Direction::WEST)) DrawRectangle(drawX - doorSize/2, cy - doorSize/2, doorSize, doorSize, WHITE);
        }
    }
    
    // Направление взгляда
    int roomX = mapX + currentRoom->GetGridX() * cellSize + cellSize/2;
    int roomY = mapY + currentRoom->GetGridY() * cellSize + cellSize/2;
    float arrowDirX = 0.0f, arrowDirY = 0.0f;
    MinimapViewDirectionOnGrid(fwdX, fwdZ, arrowDirX, arrowDirY);
    int arrowLen = cellSize * 3 / 5;
    int arrowX = roomX + (int)(arrowDirX * arrowLen);
    int arrowY = roomY + (int)(arrowDirY * arrowLen);
    DrawCircle(roomX, roomY, cellSize / 4, Fade(YELLOW, 0.35f));
    DrawLine(roomX, roomY, arrowX, arrowY, Fade(WHITE, 0.95f));
    DrawTriangle({(float)arrowX, (float)arrowY},
                 {(float)arrowX - arrowDirY * 5.0f, (float)arrowY + arrowDirX * 5.0f},
                 {(float)arrowX + arrowDirY * 5.0f, (float)arrowY - arrowDirX * 5.0f},
                 Fade(WHITE, 0.9f));
}