#include "GameRenderer.h"
#include "MazeGraph.h"
#include "Room.h"
#include <iostream>
#include <raymath.h>
#include <cmath>

GameRenderer::GameRenderer(int mazeWidth, int mazeHeight)
    : m_maze(mazeWidth, mazeHeight) {
    
    m_maze.Generate();
    m_currentRoom = m_maze.GetStartRoom();
    m_currentRoom->explored = true;
    
    InitWindow(1280, 720, "Maze Explorer");
    SetTargetFPS(60);
    
    Vector3 roomCenter = m_currentRoom->worldPosition;
    m_camera.position = {roomCenter.x, CAMERA_HEIGHT, roomCenter.z};
    m_camera.target = {roomCenter.x, CAMERA_HEIGHT, roomCenter.z + 5.0f};
    m_camera.up = {0.0f, 1.0f, 0.0f};
    m_camera.fovy = 90.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;
    
    m_cameraAngle = 0.0f;
    m_cameraPitch = 0.0f;
    
    for (int y = 0; y < m_maze.GetHeight(); ++y) {
        for (int x = 0; x < m_maze.GetWidth(); ++x) {
            Room* r = m_maze.GetRoom(x, y);
            std::cout << "Room (" << x << "," << y << "): ";
            std::cout << "N:" << r->hasDoor[0] << " ";
            std::cout << "S:" << r->hasDoor[1] << " ";
            std::cout << "E:" << r->hasDoor[2] << " ";
            std::cout << "W:" << r->hasDoor[3] << "\n";
        }
    }
}

GameRenderer::~GameRenderer() {
    CloseWindow();
}

void GameRenderer::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();
    }
}

void GameRenderer::HandleInput() {
    if (IsKeyPressed(KEY_M)) {
        m_showMinimap = !m_showMinimap;
        std::cout << "Minimap: " << (m_showMinimap ? "ON" : "OFF") << std::endl;
    }
    
    if (IsKeyPressed(KEY_F1)) {
        m_showDebug = !m_showDebug;
        std::cout << "Debug info: " << (m_showDebug ? "ON" : "OFF") << std::endl;
    }
    
    if (IsKeyPressed(KEY_P)) {
        m_maze.PrintGraph();
    }
    
    Vector2 mouseDelta = GetMouseDelta();
    m_cameraAngle -= mouseDelta.x * 0.003f;
    m_cameraPitch -= mouseDelta.y * 0.003f;
    
    while (m_cameraAngle < 0) m_cameraAngle += 2 * PI;
    while (m_cameraAngle >= 2 * PI) m_cameraAngle -= 2 * PI;
    
    if (m_cameraPitch > 0.8f) m_cameraPitch = 0.8f;
    if (m_cameraPitch < -0.5f) m_cameraPitch = -0.5f;
    
    Vector3 roomCenter = m_currentRoom->worldPosition;
    
    Vector3 forward = {
        sinf(m_cameraAngle) * cosf(m_cameraPitch),
        sinf(m_cameraPitch),
        cosf(m_cameraAngle) * cosf(m_cameraPitch)
    };
    
    m_camera.target = Vector3Add(m_camera.position, forward);
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Ray ray = GetMouseRay({GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}, m_camera);
        
            Direction clickedDoor = GetDoorFromRay(ray, m_currentRoom);
            
            if (clickedDoor != Direction::NONE) {
                int dirIndex = static_cast<int>(clickedDoor);
                               
                MoveToRoom(m_currentRoom->neighbors[dirIndex]);

        }
    }
}

void GameRenderer::Update() {
    Vector3 roomCenter = m_currentRoom->worldPosition;
    m_camera.position = {roomCenter.x, CAMERA_HEIGHT, roomCenter.z};
}

void GameRenderer::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode3D(m_camera);
    
    DrawRoom(m_currentRoom);
    DrawDoors3D(m_currentRoom);
    
    EndMode3D();
    
    DrawCrosshair();
    
    if (m_showMinimap) {
        if (m_showDebug) {
            DrawDebugMap();
        } else {
            DrawMinimap();
        }
    }
    
    DrawDebugInfo();
    
    DisableCursor();
    
    if (m_currentRoom == m_maze.GetExitRoom()) {
        DrawText("YOU ESCAPED! Congratulations!", 
                 GetScreenWidth()/2 - 150, GetScreenHeight()/2, 30, GREEN);
    }
    
    EndDrawing();
}

void GameRenderer::DrawRoom(Room* room) {
    Vector3 pos = room->worldPosition;
    float half = ROOM_SIZE / 2.0f;
    
    DrawPlane({pos.x, 0.0f, pos.z}, {ROOM_SIZE, ROOM_SIZE}, GRAY);
    
    DrawPlane({pos.x, WALL_HEIGHT, pos.z}, {ROOM_SIZE, ROOM_SIZE}, DARKGRAY);
    
    Color wallColor = (Color){80, 80, 100, 255};
    
    if (!room->hasDoor[0]) {
        DrawCube({pos.x, WALL_HEIGHT/2, pos.z + half}, 
                 ROOM_SIZE, WALL_HEIGHT, WALL_THICKNESS, wallColor);
    } else {
        DrawCube({pos.x - ROOM_SIZE/4 - DOOR_WIDTH/4, WALL_HEIGHT/2, pos.z + half}, 
                 ROOM_SIZE/2 - DOOR_WIDTH/2, WALL_HEIGHT, WALL_THICKNESS, wallColor);
        DrawCube({pos.x + ROOM_SIZE/4 + DOOR_WIDTH/4, WALL_HEIGHT/2, pos.z + half}, 
                 ROOM_SIZE/2 - DOOR_WIDTH/2, WALL_HEIGHT, WALL_THICKNESS, wallColor);
        DrawCube({pos.x, WALL_HEIGHT - 0.25f, pos.z + half}, 
                 DOOR_WIDTH, 0.5f, WALL_THICKNESS, wallColor);
    }
    
    if (!room->hasDoor[1]) {
        DrawCube({pos.x, WALL_HEIGHT/2, pos.z - half}, 
                 ROOM_SIZE, WALL_HEIGHT, WALL_THICKNESS, wallColor);
    } else {
        DrawCube({pos.x - ROOM_SIZE/4 - DOOR_WIDTH/4, WALL_HEIGHT/2, pos.z - half}, 
                 ROOM_SIZE/2 - DOOR_WIDTH/2, WALL_HEIGHT, WALL_THICKNESS, wallColor);
        DrawCube({pos.x + ROOM_SIZE/4 + DOOR_WIDTH/4, WALL_HEIGHT/2, pos.z - half}, 
                 ROOM_SIZE/2 - DOOR_WIDTH/2, WALL_HEIGHT, WALL_THICKNESS, wallColor);
        DrawCube({pos.x, WALL_HEIGHT - 0.25f, pos.z - half}, 
                 DOOR_WIDTH, 0.5f, WALL_THICKNESS, wallColor);
    }
    
    if (!room->hasDoor[3]) {
        DrawCube({pos.x + half, WALL_HEIGHT/2, pos.z}, 
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE, wallColor);
    } else {
        DrawCube({pos.x + half, WALL_HEIGHT/2, pos.z - ROOM_SIZE/4 - DOOR_WIDTH/4}, 
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE/2 - DOOR_WIDTH/2, wallColor);
        DrawCube({pos.x + half, WALL_HEIGHT/2, pos.z + ROOM_SIZE/4 + DOOR_WIDTH/4}, 
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE/2 - DOOR_WIDTH/2, wallColor);
        DrawCube({pos.x + half, WALL_HEIGHT - 0.25f, pos.z}, 
                 WALL_THICKNESS, 0.5f, DOOR_WIDTH, wallColor);
    }
    
    if (!room->hasDoor[2]) {
        DrawCube({pos.x - half, WALL_HEIGHT/2, pos.z}, 
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE, wallColor);
    } else {
        DrawCube({pos.x - half, WALL_HEIGHT/2, pos.z - ROOM_SIZE/4 - DOOR_WIDTH/4}, 
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE/2 - DOOR_WIDTH/2, wallColor);
        DrawCube({pos.x - half, WALL_HEIGHT/2, pos.z + ROOM_SIZE/4 + DOOR_WIDTH/4}, 
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE/2 - DOOR_WIDTH/2, wallColor);
        DrawCube({pos.x - half, WALL_HEIGHT - 0.25f, pos.z}, 
                 WALL_THICKNESS, 0.5f, DOOR_WIDTH, wallColor);
    }
    
    if (room == m_maze.GetStartRoom()) {
        DrawSphere({pos.x, 0.5f, pos.z}, 0.5f, GREEN);
    }
    if (room == m_maze.GetExitRoom()) {
        DrawSphere({pos.x, 0.5f, pos.z}, 0.5f, RED);
    }
}

void GameRenderer::DrawDoors3D(Room* room) {
    Vector3 pos = room->worldPosition;
    float half = ROOM_SIZE / 2.0f;
    
    Ray ray = GetMouseRay({GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}, m_camera);
    Direction hoveredDoor = GetDoorFromRay(ray, room);
    
    float doorThickness = 0.2f;
    Color frameColor = (Color){101, 67, 33, 255};
    Color handleColor = GOLD;
    
    // Северная дверь (+Z)
    if (room->hasDoor[0]) {
        Vector3 doorPos = {pos.x, DOOR_HEIGHT/2, pos.z + half};
        
        Color doorColor = (Color){139, 69, 19, 255};
        
        DrawCube(doorPos, DOOR_WIDTH, DOOR_HEIGHT, doorThickness, doorColor);
        DrawCubeWires(doorPos, DOOR_WIDTH, DOOR_HEIGHT, doorThickness + 0.01f, frameColor);
        DrawSphere({doorPos.x - DOOR_WIDTH/3, DOOR_HEIGHT/2, doorPos.z}, 0.12f, handleColor);
        
        if (hoveredDoor == Direction::NORTH) {
            DrawCubeWires(doorPos, DOOR_WIDTH + 0.15f, DOOR_HEIGHT + 0.15f, doorThickness + 0.02f, YELLOW);
        }
    }
    
    if (room->hasDoor[1]) {
        Vector3 doorPos = {pos.x, DOOR_HEIGHT/2, pos.z - half};
        
        Color doorColor = (Color){139, 69, 19, 255};
        
        DrawCube(doorPos, DOOR_WIDTH, DOOR_HEIGHT, doorThickness, doorColor);
        DrawCubeWires(doorPos, DOOR_WIDTH, DOOR_HEIGHT, doorThickness + 0.01f, frameColor);
        DrawSphere({doorPos.x + DOOR_WIDTH/3, DOOR_HEIGHT/2, doorPos.z}, 0.12f, handleColor);
        
        if (hoveredDoor == Direction::SOUTH) {
            DrawCubeWires(doorPos, DOOR_WIDTH + 0.15f, DOOR_HEIGHT + 0.15f, doorThickness + 0.02f, YELLOW);
        }
    }
    
    if (room->hasDoor[3]) {
        Vector3 doorPos = {pos.x + half, DOOR_HEIGHT/2, pos.z};
        
        Color doorColor = (Color){139, 69, 19, 255};
        
        DrawCube(doorPos, doorThickness, DOOR_HEIGHT, DOOR_WIDTH, doorColor);
        DrawCubeWires(doorPos, doorThickness + 0.01f, DOOR_HEIGHT, DOOR_WIDTH, frameColor);
        DrawSphere({doorPos.x, DOOR_HEIGHT/2, doorPos.z - DOOR_WIDTH/3}, 0.12f, handleColor);
        
        if (hoveredDoor == Direction::EAST) {
            DrawCubeWires(doorPos, doorThickness + 0.02f, DOOR_HEIGHT + 0.15f, DOOR_WIDTH + 0.15f, YELLOW);
        }
    }
    
    if (room->hasDoor[2]) {
        Vector3 doorPos = {pos.x - half, DOOR_HEIGHT/2, pos.z};
        
        Color doorColor = (Color){139, 69, 19, 255};
        
        DrawCube(doorPos, doorThickness, DOOR_HEIGHT, DOOR_WIDTH, doorColor);
        DrawCubeWires(doorPos, doorThickness + 0.01f, DOOR_HEIGHT, DOOR_WIDTH, frameColor);
        DrawSphere({doorPos.x, DOOR_HEIGHT/2, doorPos.z + DOOR_WIDTH/3}, 0.12f, handleColor);
        
        if (hoveredDoor == Direction::WEST) {
            DrawCubeWires(doorPos, doorThickness + 0.02f, DOOR_HEIGHT + 0.15f, DOOR_WIDTH + 0.15f, YELLOW);
        }
    }
}

void GameRenderer::DrawMinimap() {
    int mapSize = 200;
    int cellSize = mapSize / std::max(m_maze.GetWidth(), m_maze.GetHeight());
    int mapX = GetScreenWidth() - mapSize - 30;
    int mapY = 70;
    
    DrawRectangle(mapX - 10, mapY - 30, mapSize + 20, mapSize + 40, Fade(BLACK, 0.8f));
    DrawText("MINIMAP", mapX + mapSize/2 - 40, mapY - 25, 20, LIGHTGRAY);
    
    for (int y = 0; y < m_maze.GetHeight(); ++y) {
        for (int x = 0; x < m_maze.GetWidth(); ++x) {
            Room* room = m_maze.GetRoom(x, y);
            int drawX = mapX + x * cellSize;
            int drawY = mapY + y * cellSize;
            
            Color color;
            if (room == m_currentRoom) {
                color = YELLOW;
            } else if (room->explored) {
                if (room == m_maze.GetStartRoom()) color = GREEN;
                else if (room == m_maze.GetExitRoom()) color = RED;
                else color = (Color){100, 100, 200, 255};
            } else {
                color = DARKGRAY;
            }
            
            DrawRectangle(drawX, drawY, cellSize - 1, cellSize - 1, color);
            
            if (room->explored || room == m_currentRoom) {
                int cx = drawX + cellSize/2;
                int cy = drawY + cellSize/2;

                if (room->hasDoor[0]) DrawLine(cx, drawY, cx, drawY - 3, WHITE);
                if (room->hasDoor[3]) DrawLine(drawX, cy, drawX - 3, cy, WHITE);
                if (room->hasDoor[1]) DrawLine(cx, drawY + cellSize, cx, drawY + cellSize + 3, WHITE);
                if (room->hasDoor[2]) DrawLine(drawX + cellSize, cy, drawX + cellSize + 3, cy, WHITE);
            }
        }
    }
    
    if (m_currentRoom) {
        int roomX = mapX + m_currentRoom->gridX * cellSize + cellSize/2;
        int roomY = mapY + m_currentRoom->gridY * cellSize + cellSize/2;
        float dirX = sinf(m_cameraAngle);
        float dirY = -cosf(m_cameraAngle);

        float angle = m_cameraAngle;
        float arrowDirX = sinf(angle);
        float arrowDirY = -cosf(angle);

        arrowDirX = -arrowDirX;
        
        int arrowLen = cellSize / 2;
        int arrowX = roomX + (int)(arrowDirX * arrowLen);
        int arrowY = roomY + (int)(arrowDirY * arrowLen);
        
        DrawCircle(roomX, roomY, cellSize/4, Fade(YELLOW, 0.3f));
        DrawLine(roomX, roomY, arrowX, arrowY, YELLOW);
        DrawCircle(arrowX, arrowY, 3, YELLOW);
        
        Vector2 tip = {(float)arrowX, (float)arrowY};
        Vector2 left = {tip.x - arrowDirY * 4, tip.y + arrowDirX * 4};
        Vector2 right = {tip.x + arrowDirY * 4, tip.y - arrowDirX * 4};
        DrawTriangle(tip, left, right, Fade(YELLOW, 0.7f));
    }
}

void GameRenderer::DrawCrosshair() {
    int cx = GetScreenWidth() / 2;
    int cy = GetScreenHeight() / 2;
    DrawCircle(cx, cy, 5, Fade(WHITE, 0.5f));
    DrawCircle(cx, cy, 2, WHITE);
}

void GameRenderer::DrawDebugInfo() {
    if (!m_showDebug) return;
    
    int y = 60;
    DrawText("DEBUG INFO (F1 to hide):", 10, y, 20, YELLOW); y += 25;
    
    float angleDeg = m_cameraAngle * RAD2DEG;
    DrawText(TextFormat("Camera Angle: %.1f°", angleDeg), 10, y, 16, WHITE); y += 20;
    
    Direction lookingDir = GetDirectionFromAngle(m_cameraAngle);
    const char* dirText = "";
    switch(lookingDir) {
        case Direction::NORTH: dirText = "NORTH (0°)"; break;
        case Direction::WEST:  dirText = "WEST (90°)"; break;
        case Direction::SOUTH: dirText = "SOUTH (180°)"; break;
        case Direction::EAST:  dirText = "EAST (270°)"; break;
        default: dirText = "UNKNOWN"; break;
    }
    DrawText(TextFormat("Looking: %s", dirText), 10, y, 16, WHITE); y += 20;
    
    Ray ray = GetMouseRay({GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}, m_camera);
    Direction hoveredDoor = GetDoorFromRay(ray, m_currentRoom);
    const char* hoverText = "";
    switch(hoveredDoor) {
        case Direction::NORTH: hoverText = "NORTH"; break;
        case Direction::WEST:  hoverText = "WEST"; break;
        case Direction::SOUTH: hoverText = "SOUTH"; break;
        case Direction::EAST:  hoverText = "EAST"; break;
        case Direction::NONE:  hoverText = "NONE"; break;
        default: hoverText = "???"; break;
    }
    DrawText(TextFormat("Hovered door: %s", hoverText), 10, y, 16, WHITE); y += 20;
    
    DrawText(TextFormat("Available doors: N:%d W:%d S:%d E:%d",
             m_currentRoom->hasDoor[0], m_currentRoom->hasDoor[3],
             m_currentRoom->hasDoor[1], m_currentRoom->hasDoor[2]), 
             10, y, 16, WHITE);
}

Direction GameRenderer::GetDoorFromRay(Ray ray, Room* room) {
    Vector3 pos = room->worldPosition;
    float half = ROOM_SIZE / 2.0f;
    
    BoundingBox doors[4];
    bool hasValidDoor[4] = {false};
    
    if (room->hasDoor[0]) {
        doors[0] = {{pos.x - DOOR_WIDTH/2, 0.1f, pos.z + half - 0.3f},
                    {pos.x + DOOR_WIDTH/2, DOOR_HEIGHT, pos.z + half + 0.3f}};
        hasValidDoor[0] = true;
    }

    if (room->hasDoor[1]) {
        doors[1] = {{pos.x - DOOR_WIDTH/2, 0.1f, pos.z - half - 0.3f},
                    {pos.x + DOOR_WIDTH/2, DOOR_HEIGHT, pos.z - half + 0.3f}};
        hasValidDoor[1] = true;
    }
    
if (room->hasDoor[2]) { 
    doors[2] = {{pos.x - half - 0.3f, 0.1f, pos.z - DOOR_WIDTH/2},
                {pos.x - half + 0.3f, DOOR_HEIGHT, pos.z + DOOR_WIDTH/2}};
    hasValidDoor[2] = true;
}

if (room->hasDoor[3]) {  // WEST - инвертируем X
    doors[3] = {{pos.x + half - 0.3f, 0.1f, pos.z - DOOR_WIDTH/2},
                {pos.x + half + 0.3f, DOOR_HEIGHT, pos.z + DOOR_WIDTH/2}};
    hasValidDoor[3] = true;
}
    
    RayCollision collision;
    Direction closestDoor = Direction::NONE;
    bool foundDoor = false;
    
    for (int i = 0; i < 4; i++) {
        if (hasValidDoor[i]) {
            collision = GetRayCollisionBox(ray, doors[i]);
            if (collision.hit) {
                closestDoor = static_cast<Direction>(i);
                foundDoor = true;
            }
        }
    }
    
    return closestDoor;
}

Direction GameRenderer::GetDirectionFromAngle(float angle) {
    while (angle < 0) angle += 2 * PI;
    while (angle >= 2 * PI) angle -= 2 * PI;
    
    if (angle >= 7 * PI / 4 || angle < PI / 4) {
        return Direction::NORTH; 
    } else if (angle >= PI / 4 && angle < 3 * PI / 4) {
        return Direction::WEST;
    } else if (angle >= 3 * PI / 4 && angle < 5 * PI / 4) {
        return Direction::SOUTH;
    } else {
        return Direction::EAST;
    }
}

bool GameRenderer::IsLookingAtDoor(Direction dir, float toleranceDegrees) {
    float expectedAngle = 0.0f;
    switch(dir) {
        case Direction::NORTH: expectedAngle = 0.0f; break;
        case Direction::EAST:  expectedAngle = PI / 2; break;
        case Direction::SOUTH: expectedAngle = PI; break;
        case Direction::WEST:  expectedAngle = 3 * PI / 2; break;
        default: return false;
    }
    
    float diff = fabs(m_cameraAngle - expectedAngle);
    if (diff > PI) diff = 2 * PI - diff;
    
    return diff <= toleranceDegrees * DEG2RAD;
}

void GameRenderer::MoveToRoom(Room* newRoom) {
    if (!newRoom) return;
    
    Direction cameFrom = Direction::NORTH;
    for (int i = 0; i < 4; i++) {
        if (m_currentRoom->neighbors[i] == newRoom) {
            cameFrom = static_cast<Direction>(i);
            break;
        }
    }
    
    m_currentRoom = newRoom;
    m_currentRoom->explored = true;
    
    Vector3 roomCenter = m_currentRoom->worldPosition;
    m_camera.position = {roomCenter.x, CAMERA_HEIGHT, roomCenter.z};
    
    switch(cameFrom) {
        case Direction::NORTH:
            m_cameraAngle = 0.0f;
            break;
        case Direction::EAST:
            m_cameraAngle = 3* PI / 2;
            break;
        case Direction::SOUTH:
            m_cameraAngle = PI;
            break;
        case Direction::WEST:
            m_cameraAngle = PI / 2;
            break;
        default:
            break;
    }
    
    m_cameraPitch = 0.0f;
    
    Vector3 forward = {
        sinf(m_cameraAngle) * cosf(m_cameraPitch),
        sinf(m_cameraPitch),
        cosf(m_cameraAngle) * cosf(m_cameraPitch)
    };
    m_camera.target = Vector3Add(m_camera.position, forward);
}

void GameRenderer::DrawDebugMap() {
    int mapSize = 250;
    int cellSize = mapSize / std::max(m_maze.GetWidth(), m_maze.GetHeight());
    int mapX = GetScreenWidth() - mapSize - 30;
    int mapY = 70;
    
    // Фон
    DrawRectangle(mapX - 10, mapY - 30, mapSize + 20, mapSize + 40, Fade(BLACK, 0.8f));
    DrawText("DEBUG MAP", mapX + mapSize/2 - 50, mapY - 25, 16, YELLOW);
    
    for (int y = 0; y < m_maze.GetHeight(); ++y) {
        for (int x = 0; x < m_maze.GetWidth(); ++x) {
            Room* room = m_maze.GetRoom(x, y);
            int drawX = mapX + x * cellSize;
            int drawY = mapY + y * cellSize;
            
            Color roomColor;
            if (room == m_currentRoom) {
                roomColor = YELLOW;
            } else if (room == m_maze.GetStartRoom()) {
                roomColor = GREEN;
            } else if (room == m_maze.GetExitRoom()) {
                roomColor = RED;
            } else if (room->explored) {
                roomColor = (Color){100, 100, 200, 255};
            } else {
                roomColor = DARKGRAY;
            }
            
            DrawRectangle(drawX, drawY, cellSize - 1, cellSize - 1, roomColor);
            
            int cx = drawX + cellSize/2;
            int cy = drawY + cellSize/2;
            int doorSize = cellSize / 4;
            
            if (room->hasDoor[0]) {
                Color doorColor = WHITE;
                DrawRectangle(cx - doorSize/2, drawY - doorSize/2, doorSize, doorSize, doorColor);
                DrawRectangleLines(cx - doorSize/2, drawY - doorSize/2, doorSize, doorSize, BLACK);
            }
            if (room->hasDoor[1]) {
                Color doorColor =  WHITE;
                DrawRectangle(cx - doorSize/2, drawY + cellSize - doorSize/2, doorSize, doorSize, doorColor);
                DrawRectangleLines(cx - doorSize/2, drawY + cellSize - doorSize/2, doorSize, doorSize, BLACK);
            }
            if (room->hasDoor[2]) {
                Color doorColor = WHITE;
                DrawRectangle(drawX + cellSize - doorSize/2, cy - doorSize/2, doorSize, doorSize, doorColor);
                DrawRectangleLines(drawX + cellSize - doorSize/2, cy - doorSize/2, doorSize, doorSize, BLACK);
            }
            if (room->hasDoor[3]) {
                Color doorColor = WHITE;
                DrawRectangle(drawX - doorSize/2, cy - doorSize/2, doorSize, doorSize, doorColor);
                DrawRectangleLines(drawX - doorSize/2, cy - doorSize/2, doorSize, doorSize, BLACK);
            }
        }
    }
    if (m_currentRoom) {
        int roomX = mapX + m_currentRoom->gridX * cellSize + cellSize/2;
        int roomY = mapY + m_currentRoom->gridY * cellSize + cellSize/2;
        
        float arrowDirX = -sinf(m_cameraAngle);
        float arrowDirY = -cosf(m_cameraAngle);
        
        int arrowLen = cellSize / 2;
        int arrowX = roomX + (int)(arrowDirX * arrowLen);
        int arrowY = roomY + (int)(arrowDirY * arrowLen);
        
        DrawCircle(roomX, roomY, cellSize/4, Fade(YELLOW, 0.3f));
        DrawLine(roomX, roomY, arrowX, arrowY, YELLOW);
        DrawCircle(arrowX, arrowY, 3, YELLOW);
        
        Vector2 tip = {(float)arrowX, (float)arrowY};
        Vector2 left = {tip.x - arrowDirY * 4, tip.y + arrowDirX * 4};
        Vector2 right = {tip.x + arrowDirY * 4, tip.y - arrowDirX * 4};
        DrawTriangle(tip, left, right, Fade(YELLOW, 0.7f));
    };
}