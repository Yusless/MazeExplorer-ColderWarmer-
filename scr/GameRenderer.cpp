#include "GameRenderer.h"
#include "MazeGraph.h"
#include "Room.h"
#include <iostream>
#include <raymath.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

GameRenderer::GameRenderer(int mazeWidth, int mazeHeight)
    : m_maze(mazeWidth, mazeHeight)
{
    m_maze.Generate();
    std::srand(std::time(nullptr));

    // Предметы для 3 этажей (в углах комнат)
    m_itemPositions.resize(3);
    for (int f = 0; f < 3; f++) {
        for (int i = 0; i < m_itemsRequired; i++) {
            int x = rand() % mazeWidth;
            int y = rand() % mazeHeight;
            Room* r = m_maze.GetRoom(x, y);
            if (r != m_maze.GetStartRoom() && r != m_maze.GetExitRoom()) {
                Vector3 pos = r->worldPosition;
                float offset = 2.5f;
                int corner = rand() % 4;
                float xoff = (corner == 0 || corner == 1) ? -offset : offset;
                float zoff = (corner == 0 || corner == 2) ? -offset : offset;
                m_itemPositions[f].push_back({pos.x + xoff, 0.7f, pos.z + zoff});
            }
        }
    }

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
}

GameRenderer::~GameRenderer()
{
    CloseWindow();
}

void GameRenderer::Run()
{
    // === СТАРТОВЫЙ ЭКРАН ===
    bool started = false;
    while (!started && !WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) started = true;
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("MAZE EXPLORER", GetScreenWidth()/2 - 150, GetScreenHeight()/2 - 100, 40, WHITE);
        DrawText("PRESS SPACE TO START", GetScreenWidth()/2 - 130, GetScreenHeight()/2 - 20, 20, GREEN);
        DrawText("Use MOUSE to look around, click on DOORS to move", 20, GetScreenHeight() - 60, 16, GRAY);
        DrawText("Click on GOLDEN SPHERES to collect items", 20, GetScreenHeight() - 35, 16, GRAY);
        DrawText("Press M - minimap, F1 - debug, P - pause", 20, GetScreenHeight() - 10, 16, GRAY);
        EndDrawing();
    }
    if (!started) return;
    // ===

    while (!WindowShouldClose()) {
        HandleInput();
        if (!m_paused) {
            Update();
        }
        Draw();
    }
}

void GameRenderer::HandleInput()
{
    if (IsKeyPressed(KEY_M)) m_showMinimap = !m_showMinimap;
    if (IsKeyPressed(KEY_F1)) m_showDebug = !m_showDebug;
    if (IsKeyPressed(KEY_P)) m_paused = !m_paused;

    if (m_paused) return;

    Vector2 mouseDelta = GetMouseDelta();
    m_cameraAngle -= mouseDelta.x * 0.003f;
    m_cameraPitch -= mouseDelta.y * 0.003f;

    while (m_cameraAngle < 0) m_cameraAngle += 2*PI;
    while (m_cameraAngle >= 2*PI) m_cameraAngle -= 2*PI;
    if (m_cameraPitch > 0.8f) m_cameraPitch = 0.8f;
    if (m_cameraPitch < -0.5f) m_cameraPitch = -0.5f;

    Vector3 forward = {
        sinf(m_cameraAngle) * cosf(m_cameraPitch),
        sinf(m_cameraPitch),
        cosf(m_cameraAngle) * cosf(m_cameraPitch)
    };
    m_camera.target = Vector3Add(m_camera.position, forward);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, m_camera);
        Direction clickedDoor = GetDoorFromRay(ray, m_currentRoom);
        if (clickedDoor != Direction::NONE) {
            MoveToRoom(m_currentRoom->neighbors[(int)clickedDoor]);
        } else {
            CheckItemPickup();
        }
    }
}

void GameRenderer::Update()
{
    Vector3 roomCenter = m_currentRoom->worldPosition;
    m_camera.position = {roomCenter.x, CAMERA_HEIGHT, roomCenter.z};

    if (m_tempMessage.timer > 0) {
        m_tempMessage.timer -= GetFrameTime();
    }
}

void GameRenderer::Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (!m_paused) {
        BeginMode3D(m_camera);
        DrawRoom(m_currentRoom);
        DrawDoors3D(m_currentRoom);
        for (auto& pos : m_itemPositions[m_currentFloor]) {
            DrawSphere(pos, 0.5f, GOLD);
        }
        EndMode3D();

        DrawCrosshair();

        if (m_showMinimap) {
            if (m_showDebug) DrawDebugMap();
            else DrawMinimap();
        }
        DrawDebugInfo();

        DisableCursor();

        // Обычное сообщение
        if (m_tempMessage.timer > 0 && !m_tempMessage.text.empty() && !m_tempMessage.fullscreen) {
            int tw = MeasureText(m_tempMessage.text.c_str(), 30);
            DrawRectangle(GetScreenWidth()/2 - tw/2 - 20, GetScreenHeight() - 80, tw + 40, 50, Fade(BLACK, 0.8f));
            DrawText(m_tempMessage.text.c_str(), GetScreenWidth()/2 - tw/2, GetScreenHeight() - 65, 30, YELLOW);
        }

        // Полноэкранное сообщение
        if (m_tempMessage.timer > 0 && m_tempMessage.fullscreen) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.85f));
            int tw = MeasureText(m_tempMessage.text.c_str(), 40);
            DrawText(m_tempMessage.text.c_str(), GetScreenWidth()/2 - tw/2, GetScreenHeight()/2, 40, WHITE);
        }

        if (m_itemsCollected >= m_itemsRequired && m_currentRoom != m_maze.GetExitRoom()) {
            DrawText("All items collected! Find the EXIT", GetScreenWidth()/2 - 180, GetScreenHeight() - 40, 20, GREEN);
        }
    } else {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
        DrawText("PAUSE", GetScreenWidth()/2 - 60, GetScreenHeight()/2 - 50, 50, WHITE);
        DrawText("Press P to continue", GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 20, 30, WHITE);
    }

    EndDrawing();
}

void GameRenderer::DrawRoom(Room* room)
{
    Vector3 pos = room->worldPosition;
    float half = ROOM_SIZE / 2.0f;
    Color wallColor = (Color){100,100,120,255};

    DrawPlane({pos.x, 0.0f, pos.z}, {ROOM_SIZE, ROOM_SIZE}, GRAY);
    DrawPlane({pos.x, WALL_HEIGHT, pos.z}, {ROOM_SIZE, ROOM_SIZE}, DARKGRAY);

    // Север
    if (!room->hasDoor[0]) {
        DrawCube({pos.x, WALL_HEIGHT/2, pos.z + half}, ROOM_SIZE, WALL_HEIGHT, WALL_THICKNESS, wallColor);
    } else {
        DrawCube({pos.x - ROOM_SIZE/4 - DOOR_WIDTH/4, WALL_HEIGHT/2, pos.z + half},
                 ROOM_SIZE/2 - DOOR_WIDTH/2, WALL_HEIGHT, WALL_THICKNESS, wallColor);
        DrawCube({pos.x + ROOM_SIZE/4 + DOOR_WIDTH/4, WALL_HEIGHT/2, pos.z + half},
                 ROOM_SIZE/2 - DOOR_WIDTH/2, WALL_HEIGHT, WALL_THICKNESS, wallColor);
        DrawCube({pos.x, WALL_HEIGHT - 0.25f, pos.z + half}, DOOR_WIDTH, 0.5f, WALL_THICKNESS, wallColor);
    }
    // Юг
    if (!room->hasDoor[1]) {
        DrawCube({pos.x, WALL_HEIGHT/2, pos.z - half}, ROOM_SIZE, WALL_HEIGHT, WALL_THICKNESS, wallColor);
    } else {
        DrawCube({pos.x - ROOM_SIZE/4 - DOOR_WIDTH/4, WALL_HEIGHT/2, pos.z - half},
                 ROOM_SIZE/2 - DOOR_WIDTH/2, WALL_HEIGHT, WALL_THICKNESS, wallColor);
        DrawCube({pos.x + ROOM_SIZE/4 + DOOR_WIDTH/4, WALL_HEIGHT/2, pos.z - half},
                 ROOM_SIZE/2 - DOOR_WIDTH/2, WALL_HEIGHT, WALL_THICKNESS, wallColor);
        DrawCube({pos.x, WALL_HEIGHT - 0.25f, pos.z - half}, DOOR_WIDTH, 0.5f, WALL_THICKNESS, wallColor);
    }
    // Запад
    if (!room->hasDoor[2]) {
        DrawCube({pos.x - half, WALL_HEIGHT/2, pos.z}, WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE, wallColor);
    } else {
        DrawCube({pos.x - half, WALL_HEIGHT/2, pos.z - ROOM_SIZE/4 - DOOR_WIDTH/4},
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE/2 - DOOR_WIDTH/2, wallColor);
        DrawCube({pos.x - half, WALL_HEIGHT/2, pos.z + ROOM_SIZE/4 + DOOR_WIDTH/4},
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE/2 - DOOR_WIDTH/2, wallColor);
        DrawCube({pos.x - half, WALL_HEIGHT - 0.25f, pos.z}, WALL_THICKNESS, 0.5f, DOOR_WIDTH, wallColor);
    }
    // Восток
    if (!room->hasDoor[3]) {
        DrawCube({pos.x + half, WALL_HEIGHT/2, pos.z}, WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE, wallColor);
    } else {
        DrawCube({pos.x + half, WALL_HEIGHT/2, pos.z - ROOM_SIZE/4 - DOOR_WIDTH/4},
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE/2 - DOOR_WIDTH/2, wallColor);
        DrawCube({pos.x + half, WALL_HEIGHT/2, pos.z + ROOM_SIZE/4 + DOOR_WIDTH/4},
                 WALL_THICKNESS, WALL_HEIGHT, ROOM_SIZE/2 - DOOR_WIDTH/2, wallColor);
        DrawCube({pos.x + half, WALL_HEIGHT - 0.25f, pos.z}, WALL_THICKNESS, 0.5f, DOOR_WIDTH, wallColor);
    }

    if (room == m_maze.GetStartRoom()) DrawSphere({pos.x, 0.5f, pos.z}, 0.5f, GREEN);
    if (room == m_maze.GetExitRoom()) DrawSphere({pos.x, 0.5f, pos.z}, 0.5f, RED);
}

void GameRenderer::DrawDoors3D(Room* room)
{
    Vector3 pos = room->worldPosition;
    float half = ROOM_SIZE / 2.0f;
    Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, m_camera);
    Direction hoveredDoor = GetDoorFromRay(ray, room);

    float doorThickness = 0.2f;
    Color frameColor = {101,67,33,255};
    Color handleColor = GOLD;

    auto getDoorColor = [&](Room* neighbor) -> Color {
        return (neighbor == m_maze.GetExitRoom()) ? (Color){255,20,147,255} : (Color){139,69,19,255};
    };

    if (room->hasDoor[0]) {
        Color doorColor = getDoorColor(room->neighbors[0]);
        Vector3 doorPos = {pos.x, DOOR_HEIGHT/2, pos.z + half};
        DrawCube(doorPos, DOOR_WIDTH, DOOR_HEIGHT, doorThickness, doorColor);
        DrawCubeWires(doorPos, DOOR_WIDTH, DOOR_HEIGHT, doorThickness+0.01f, frameColor);
        DrawSphere({doorPos.x - DOOR_WIDTH/3, DOOR_HEIGHT/2, doorPos.z}, 0.12f, handleColor);
        if (hoveredDoor == Direction::NORTH)
            DrawCubeWires(doorPos, DOOR_WIDTH+0.15f, DOOR_HEIGHT+0.15f, doorThickness+0.02f, YELLOW);
    }
    if (room->hasDoor[1]) {
        Color doorColor = getDoorColor(room->neighbors[1]);
        Vector3 doorPos = {pos.x, DOOR_HEIGHT/2, pos.z - half};
        DrawCube(doorPos, DOOR_WIDTH, DOOR_HEIGHT, doorThickness, doorColor);
        DrawCubeWires(doorPos, DOOR_WIDTH, DOOR_HEIGHT, doorThickness+0.01f, frameColor);
        DrawSphere({doorPos.x + DOOR_WIDTH/3, DOOR_HEIGHT/2, doorPos.z}, 0.12f, handleColor);
        if (hoveredDoor == Direction::SOUTH)
            DrawCubeWires(doorPos, DOOR_WIDTH+0.15f, DOOR_HEIGHT+0.15f, doorThickness+0.02f, YELLOW);
    }
    if (room->hasDoor[2]) {
        Color doorColor = getDoorColor(room->neighbors[2]);
        Vector3 doorPos = {pos.x - half, DOOR_HEIGHT/2, pos.z};
        DrawCube(doorPos, doorThickness, DOOR_HEIGHT, DOOR_WIDTH, doorColor);
        DrawCubeWires(doorPos, doorThickness+0.01f, DOOR_HEIGHT, DOOR_WIDTH, frameColor);
        DrawSphere({doorPos.x, DOOR_HEIGHT/2, doorPos.z + DOOR_WIDTH/3}, 0.12f, handleColor);
        if (hoveredDoor == Direction::WEST)
            DrawCubeWires(doorPos, doorThickness+0.02f, DOOR_HEIGHT+0.15f, DOOR_WIDTH+0.15f, YELLOW);
    }
    if (room->hasDoor[3]) {
        Color doorColor = getDoorColor(room->neighbors[3]);
        Vector3 doorPos = {pos.x + half, DOOR_HEIGHT/2, pos.z};
        DrawCube(doorPos, doorThickness, DOOR_HEIGHT, DOOR_WIDTH, doorColor);
        DrawCubeWires(doorPos, doorThickness+0.01f, DOOR_HEIGHT, DOOR_WIDTH, frameColor);
        DrawSphere({doorPos.x, DOOR_HEIGHT/2, doorPos.z - DOOR_WIDTH/3}, 0.12f, handleColor);
        if (hoveredDoor == Direction::EAST)
            DrawCubeWires(doorPos, doorThickness+0.02f, DOOR_HEIGHT+0.15f, DOOR_WIDTH+0.15f, YELLOW);
    }
}

void GameRenderer::DrawMinimap()
{
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
            } else if (room == m_maze.GetStartRoom()) {
                color = GREEN;   // старт всегда зелёный
            } else if (room == m_maze.GetExitRoom()) {
                color = RED;
            } else if (room->explored) {
                color = (Color){100, 100, 200, 255};
            } else {
                color = DARKGRAY;
            }
            DrawRectangle(drawX, drawY, cellSize - 1, cellSize - 1, color);

            // Двери на миникарте
            if (room->explored || room == m_currentRoom) {
                int cx = drawX + cellSize / 2;
                int cy = drawY + cellSize / 2;
                if (room->hasDoor[0]) DrawLine(cx, drawY, cx, drawY - 3, WHITE);
                if (room->hasDoor[1]) DrawLine(cx, drawY + cellSize, cx, drawY + cellSize + 3, WHITE);
                if (room->hasDoor[2]) DrawLine(drawX + cellSize, cy, drawX + cellSize + 3, cy, WHITE);
                if (room->hasDoor[3]) DrawLine(drawX, cy, drawX - 3, cy, WHITE);
            }
        }
    }

    // Жирная розовая стрелка
    if (m_currentRoom) {
        int roomX = mapX + m_currentRoom->gridX * cellSize + cellSize / 2;
        int roomY = mapY + m_currentRoom->gridY * cellSize + cellSize / 2;
        float angle = m_cameraAngle;
        float arrowDirX = sinf(angle);
        float arrowDirY = -cosf(angle);
        arrowDirX = -arrowDirX;
        int arrowLen = cellSize / 2;
        int arrowX = roomX + (int)(arrowDirX * arrowLen);
        int arrowY = roomY + (int)(arrowDirY * arrowLen);

        DrawCircle(roomX, roomY, cellSize / 4, Fade(YELLOW, 0.3f));
        DrawLineEx({(float)roomX, (float)roomY}, {(float)arrowX, (float)arrowY}, 3.0f, MAGENTA);
        DrawCircle(arrowX, arrowY, 5, MAGENTA);

        Vector2 tip = {(float)arrowX, (float)arrowY};
        Vector2 dir = {(float)arrowDirX, (float)arrowDirY};
        Vector2 perp = {-dir.y, dir.x};
        Vector2 left = {tip.x - perp.x * 6, tip.y - perp.y * 6};
        Vector2 right = {tip.x + perp.x * 6, tip.y + perp.y * 6};
        DrawTriangle(tip, left, right, MAGENTA);
    }
}

void GameRenderer::DrawCrosshair()
{
    int cx = GetScreenWidth()/2, cy = GetScreenHeight()/2;
    DrawCircle(cx, cy, 5, Fade(WHITE,0.5f));
    DrawCircle(cx, cy, 2, WHITE);
}

void GameRenderer::DrawDebugInfo()
{
    if (!m_showDebug) return;
    int y = 60;
    DrawText("DEBUG (F1 hide)", 10, y, 20, YELLOW); y += 25;
    DrawText(TextFormat("Angle: %.1f", m_cameraAngle*RAD2DEG), 10, y, 16, WHITE); y += 20;
    DrawText(TextFormat("Items: %d/%d", m_itemsCollected, m_itemsRequired), 10, y, 16, WHITE); y += 20;
    DrawText(TextFormat("Floor: %d/3", m_currentFloor+1), 10, y, 16, WHITE); y += 20;
    DrawText(TextFormat("Item count in vector: %d", (int)m_itemPositions[m_currentFloor].size()), 10, y, 16, WHITE);
}

Direction GameRenderer::GetDoorFromRay(Ray ray, Room* room)
{
    Vector3 pos = room->worldPosition;
    float half = ROOM_SIZE/2.0f;
    BoundingBox doors[4];
    bool valid[4] = {false};
    if (room->hasDoor[0]) {
        doors[0] = {{pos.x - DOOR_WIDTH/2, 0.1f, pos.z+half-0.3f}, {pos.x + DOOR_WIDTH/2, DOOR_HEIGHT, pos.z+half+0.3f}};
        valid[0]=true;
    }
    if (room->hasDoor[1]) {
        doors[1] = {{pos.x - DOOR_WIDTH/2, 0.1f, pos.z-half-0.3f}, {pos.x + DOOR_WIDTH/2, DOOR_HEIGHT, pos.z-half+0.3f}};
        valid[1]=true;
    }
    if (room->hasDoor[2]) {
        doors[2] = {{pos.x - half - 0.3f, 0.1f, pos.z - DOOR_WIDTH/2}, {pos.x - half + 0.3f, DOOR_HEIGHT, pos.z + DOOR_WIDTH/2}};
        valid[2]=true;
    }
    if (room->hasDoor[3]) {
        doors[3] = {{pos.x + half - 0.3f, 0.1f, pos.z - DOOR_WIDTH/2}, {pos.x + half + 0.3f, DOOR_HEIGHT, pos.z + DOOR_WIDTH/2}};
        valid[3]=true;
    }
    for (int i=0;i<4;i++) if (valid[i]) {
        RayCollision col = GetRayCollisionBox(ray, doors[i]);
        if (col.hit) return (Direction)i;
    }
    return Direction::NONE;
}

Direction GameRenderer::GetDirectionFromAngle(float angle)
{
    while (angle<0) angle+=2*PI; while (angle>=2*PI) angle-=2*PI;
    if (angle>=7*PI/4 || angle<PI/4) return Direction::NORTH;
    if (angle>=PI/4 && angle<3*PI/4) return Direction::WEST;
    if (angle>=3*PI/4 && angle<5*PI/4) return Direction::SOUTH;
    return Direction::EAST;
}

bool GameRenderer::IsLookingAtDoor(Direction dir, float toleranceDegrees)
{
    float expected=0;
    switch(dir) {
        case Direction::NORTH: expected=0; break;
        case Direction::EAST:  expected=PI/2; break;
        case Direction::SOUTH: expected=PI; break;
        case Direction::WEST:  expected=3*PI/2; break;
        default: return false;
    }
    float diff = fabs(m_cameraAngle - expected);
    if (diff>PI) diff=2*PI-diff;
    return diff <= toleranceDegrees*DEG2RAD;
}

void GameRenderer::CheckItemPickup()
{
    Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, m_camera);
    float bestDist = 3.0f;
    Vector3 pickedPos = {0,0,0};
    bool found = false;

    for (auto& pos : m_itemPositions[m_currentFloor]) {
        Vector3 dir = Vector3Subtract(pos, ray.position);
        float t = Vector3DotProduct(dir, ray.direction);
        if (t > 0) {
            Vector3 point = Vector3Add(ray.position, Vector3Scale(ray.direction, t));
            float dist = Vector3Distance(point, pos);
            if (dist < 1.0f) { // увеличен радиус
                float full = Vector3Distance(ray.position, pos);
                if (full < bestDist) {
                    bestDist = full;
                    pickedPos = pos;
                    found = true;
                }
            }
        }
    }

    if (found) {
        for (auto it = m_itemPositions[m_currentFloor].begin(); it != m_itemPositions[m_currentFloor].end(); ++it) {
            if (Vector3Distance(*it, pickedPos) < 0.3f) {
                m_itemsCollected++;
                m_itemPositions[m_currentFloor].erase(it);
                std::cout << "Item collected! Now " << m_itemsCollected << "/" << m_itemsRequired << std::endl;
                break;
            }
        }
        std::string msg = "Item collected! (" + std::to_string(m_itemsCollected) + "/" + std::to_string(m_itemsRequired) + ")";
        if (m_itemsCollected >= m_itemsRequired) {
            msg = "All items collected! Find the EXIT!";
        }
        ShowMessage(msg, 2.0f, false);
    } else {
        // Необязательно, но можно вывести в консоль для отладки
        // std::cout << "Missed item" << std::endl;
    }
}

void GameRenderer::MoveToRoom(Room* newRoom)
{
    if (!newRoom) return;

    Direction cameFrom = Direction::NORTH;
    for (int i=0;i<4;i++) {
        if (m_currentRoom->neighbors[i] == newRoom) {
            cameFrom = (Direction)i;
            break;
        }
    }

    m_currentRoom = newRoom;
    m_currentRoom->explored = true;

    Vector3 roomCenter = m_currentRoom->worldPosition;
    m_camera.position = {roomCenter.x, CAMERA_HEIGHT, roomCenter.z};

    switch(cameFrom) {
        case Direction::NORTH: m_cameraAngle = PI; break;
        case Direction::SOUTH: m_cameraAngle = 0; break;
        case Direction::WEST:  m_cameraAngle = PI/2; break;
        case Direction::EAST:  m_cameraAngle = 3*PI/2; break;
        default: break;
    }
    m_cameraPitch = 0.0f;
    Vector3 forward = { sinf(m_cameraAngle), 0, cosf(m_cameraAngle) };
    m_camera.target = Vector3Add(m_camera.position, forward);

    if (m_currentRoom == m_maze.GetExitRoom() && m_itemsCollected >= m_itemsRequired) {
        NextFloor();
    }
}

void GameRenderer::DrawDebugMap()
{
    int mapSize=250, cellSize=mapSize/std::max(m_maze.GetWidth(),m_maze.GetHeight());
    int mapX=GetScreenWidth()-mapSize-30, mapY=70;
    DrawRectangle(mapX-10, mapY-30, mapSize+20, mapSize+40, Fade(BLACK,0.8f));
    DrawText("DEBUG MAP", mapX+mapSize/2-50, mapY-25, 16, YELLOW);
    for (int y=0; y<m_maze.GetHeight(); y++) for (int x=0; x<m_maze.GetWidth(); x++) {
        Room* room = m_maze.GetRoom(x,y);
        int drawX=mapX+x*cellSize, drawY=mapY+y*cellSize;
        Color roomColor;
        if (room==m_currentRoom) roomColor=YELLOW;
        else if (room==m_maze.GetStartRoom()) roomColor=GREEN;
        else if (room==m_maze.GetExitRoom()) roomColor=RED;
        else if (room->explored) roomColor=(Color){100,100,200,255};
        else roomColor=DARKGRAY;
        DrawRectangle(drawX, drawY, cellSize-1, cellSize-1, roomColor);
    }
    if (m_currentRoom) {
        int roomX=mapX+m_currentRoom->gridX*cellSize+cellSize/2;
        int roomY=mapY+m_currentRoom->gridY*cellSize+cellSize/2;
        float arrowDirX = -sinf(m_cameraAngle);
        float arrowDirY = -cosf(m_cameraAngle);
        int arrowX=roomX+(int)(arrowDirX*cellSize/2);
        int arrowY=roomY+(int)(arrowDirY*cellSize/2);
        DrawCircle(roomX, roomY, cellSize/4, Fade(YELLOW,0.3f));
        DrawLineEx({(float)roomX, (float)roomY}, {(float)arrowX, (float)arrowY}, 3.0f, MAGENTA);
        DrawCircle(arrowX, arrowY, 5, MAGENTA);
        Vector2 tip={(float)arrowX,(float)arrowY};
        Vector2 dir = {(float)arrowDirX, (float)arrowDirY};
        Vector2 perp = {-dir.y, dir.x};
        Vector2 left = {tip.x - perp.x * 6, tip.y - perp.y * 6};
        Vector2 right = {tip.x + perp.x * 6, tip.y + perp.y * 6};
        DrawTriangle(tip, left, right, MAGENTA);
    }
}

void GameRenderer::NextFloor()
{
    m_currentFloor++;
    if (m_currentFloor >= 3) {
        ShowMessage("GAME COMPLETED! You escaped!", 4.0f, true);
        return;
    }

    m_maze.Generate();
    m_currentRoom = m_maze.GetStartRoom();
    m_currentRoom->explored = true;
    m_itemsCollected = 0;

    // Пересоздаём предметы для нового этажа
    m_itemPositions[m_currentFloor].clear();
    for (int i = 0; i < m_itemsRequired; i++) {
        int x = rand() % m_maze.GetWidth();
        int y = rand() % m_maze.GetHeight();
        Room* r = m_maze.GetRoom(x, y);
        if (r != m_maze.GetStartRoom() && r != m_maze.GetExitRoom()) {
            Vector3 pos = r->worldPosition;
            float offset = 2.5f;
            int corner = rand() % 4;
            float xoff = (corner == 0 || corner == 1) ? -offset : offset;
            float zoff = (corner == 0 || corner == 2) ? -offset : offset;
            m_itemPositions[m_currentFloor].push_back({pos.x + xoff, 0.7f, pos.z + zoff});
        }
    }

    ShowMessage("You passed to floor " + std::to_string(m_currentFloor + 1) + "!", 3.0f, true);
}

void GameRenderer::ShowMessage(const std::string& text, float duration, bool fullscreen)
{
    m_tempMessage.text = text;
    m_tempMessage.timer = duration;
    m_tempMessage.fullscreen = fullscreen;
}