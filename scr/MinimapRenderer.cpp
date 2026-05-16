#include "MinimapRenderer.h"
#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <iostream>

void MinimapRenderer::Draw(bool showDebug, Room* currentRoom, float fwdX, float fwdZ, MazeGraph* maze) {
    if (!maze) return;
    if (showDebug)
        DrawDebugMap(currentRoom, fwdX, fwdZ, maze);
    else
        DrawMinimap(currentRoom, fwdX, fwdZ, maze);
}

static void MinimapViewDirectionOnGrid(float fwdX, float fwdZ) {
    float len = sqrtf(fwdX * fwdX + fwdZ * fwdZ);
    if (len > 1e-6f) {
        fwdX /= len;
        fwdZ /= len;
    }
}

void MinimapRenderer::DrawMinimap(Room* currentRoom, float fwdX, float fwdZ, MazeGraph* maze) {
    if (!currentRoom) return;
    
    int mapSize = 250;
    int cellSize = mapSize / std::max(maze->GetWidth(), maze->GetHeight());
    int mapX = GetScreenWidth() - mapSize - 30;
    int mapY = 70;
    
    DrawRectangle(mapX - 10, mapY - 30, mapSize + 20, mapSize + 40, Fade(BLACK, 0.8f));
    DrawText("MINIMAP", mapX + mapSize/2 - 40, mapY - 25, 20, LIGHTGRAY);
    
    // Отрисовка всех комнат (цвета как обычно)
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
                else color = {100, 100, 200, 255};
            } else color = DARKGRAY;
            DrawRectangle(drawX, drawY, cellSize - 1, cellSize - 1, color);
        }
    }
    
    // ========== РИСУЕМ ДВЕРИ ДЛЯ ТЕКУЩЕЙ И ВСЕХ ИССЛЕДОВАННЫХ КОМНАТ ==========
    for (int y = 0; y < maze->GetHeight(); ++y) {
        for (int x = 0; x < maze->GetWidth(); ++x) {
            Room* room = maze->GetRoom(x, y);
            if (!room) continue;
            
            // Рисуем двери, если комната исследована ИЛИ это текущая комната
            if (room->IsExplored() || room == currentRoom) {
                int cx = mapX + room->GetGridX() * cellSize + cellSize/2;
                int cy = mapY + room->GetGridY() * cellSize + cellSize/2;
                int x0 = mapX + room->GetGridX() * cellSize;
                int y0 = mapY + room->GetGridY() * cellSize;
                
                Color doorColor = ORANGE;
                float thickness = 3.0f;
                int doorLen = 12;
                
                if (room->HasDoor(Direction::NORTH))
                    DrawLineEx({(float)cx, (float)y0}, {(float)cx, (float)y0 - doorLen}, thickness, doorColor);
                if (room->HasDoor(Direction::SOUTH))
                    DrawLineEx({(float)cx, (float)(y0 + cellSize)}, {(float)cx, (float)(y0 + cellSize + doorLen)}, thickness, doorColor);
                if (room->HasDoor(Direction::EAST))
                    DrawLineEx({(float)(x0 + cellSize), (float)cy}, {(float)(x0 + cellSize + doorLen), (float)cy}, thickness, doorColor);
                if (room->HasDoor(Direction::WEST))
                    DrawLineEx({(float)x0, (float)cy}, {(float)x0 - doorLen, (float)cy}, thickness, doorColor);
            }
        }
    }
    
    // Стрелка направления (синяя)
    int roomX = mapX + currentRoom->GetGridX() * cellSize + cellSize/2;
    int roomY = mapY + currentRoom->GetGridY() * cellSize + cellSize/2;
    MinimapViewDirectionOnGrid(fwdX, fwdZ);
    int arrowLen = cellSize * 3 / 5;
    int arrowX = roomX + (int)(fwdX * arrowLen);
    int arrowY = roomY + (int)(fwdZ * arrowLen);
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
            else if (room->IsExplored()) roomColor = {100, 100, 200, 255};
            else roomColor = DARKGRAY;
            DrawRectangle(drawX, drawY, cellSize - 1, cellSize - 1, roomColor);
            
            // Для отладки рисуем двери всех исследованных комнат (чтобы видеть)
            if (room->IsExplored() || room == currentRoom) {
                int cx = drawX + cellSize/2;
                int cy = drawY + cellSize/2;
                int doorSize = cellSize / 2;
                Color orange = {255, 128, 0, 255};
                if (room->HasDoor(Direction::NORTH))
                    DrawRectangle(cx - doorSize/2, drawY - doorSize/2, doorSize, doorSize, orange);
                if (room->HasDoor(Direction::SOUTH))
                    DrawRectangle(cx - doorSize/2, drawY + cellSize - doorSize/2, doorSize, doorSize, orange);
                if (room->HasDoor(Direction::EAST))
                    DrawRectangle(drawX + cellSize - doorSize/2, cy - doorSize/2, doorSize, doorSize, orange);
                if (room->HasDoor(Direction::WEST))
                    DrawRectangle(drawX - doorSize/2, cy - doorSize/2, doorSize, doorSize, orange);
            }
        }
    }
    
    // Направление взгляда
    int roomX = mapX + currentRoom->GetGridX() * cellSize + cellSize/2;
    int roomY = mapY + currentRoom->GetGridY() * cellSize + cellSize/2;
    MinimapViewDirectionOnGrid(fwdX, fwdZ);
    int arrowLen = cellSize * 3 / 5;
    int arrowX = roomX + (int)(fwdX * arrowLen);
    int arrowY = roomY + (int)(fwdZ * arrowLen);
    DrawCircle(roomX, roomY, cellSize / 4, Fade(YELLOW, 0.35f));
    DrawLine(roomX, roomY, arrowX, arrowY, Fade(BLUE, 0.95f));
}