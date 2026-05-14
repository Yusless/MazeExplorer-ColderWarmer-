#include "Game.h"
#include "Door.h"
#include "Constants.h"
#include <iostream>
#include <raymath.h>

Game::Game(int mazeWidth, int mazeHeight)
    : m_maze(mazeWidth, mazeHeight), m_minimap(m_maze),
      m_showMinimap(true), m_showDebug(false), m_devMode(true) {
    
    m_maze.Generate();
    m_currentRoom = m_maze.GetStartRoom();
    m_currentRoom->SetExplored(true);
    
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, "Maze Explorer");
    SetTargetFPS(60);
    DisableCursor();  // Важно! Захватываем мышь
}

Game::~Game() {
    CloseWindow();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();
    }
}

void Game::HandleInput() {
    // Получаем дельту мыши напрямую из raylib
    Vector2 mouseDelta = GetMouseDelta();
    
    // Передаём в камеру
    m_camera.HandleMouseInput(mouseDelta);
    
    // Обработка клавиш
    if (IsKeyPressed(KEY_M)) {
        m_showMinimap = !m_showMinimap;
        std::cout << "Minimap: " << (m_showMinimap ? "ON" : "OFF") << std::endl;
    }
    if (IsKeyPressed(KEY_F1)) {
        m_showDebug = !m_showDebug;
        std::cout << "Debug: " << (m_showDebug ? "ON" : "OFF") << std::endl;
    }
    if (IsKeyPressed(KEY_P)) {
        m_maze.PrintGraph();
    }
    
    // Клик мыши для прохода через дверь
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        std::cout << "Left mouse button clicked!" << std::endl;
        Direction door = GetHoveredDoor();
        if (door != Direction::NONE) {
            std::cout << "Door detected!" << std::endl;
            Room* next = m_currentRoom->GetNeighbor(door);
            if (next) {
                MoveToRoom(next);
            }
        }
    }
}

void Game::Update() {
    m_camera.Update(m_currentRoom);
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode3D(m_camera.GetCamera());
    m_renderer3D.DrawRoom(m_currentRoom);
    Direction hovered;
    m_renderer3D.DrawDoors(m_currentRoom, m_camera.GetCamera(), hovered);
    
    Vector3 startPos = m_maze.GetStartRoom()->GetWorldPosition();
    Vector3 exitPos = m_maze.GetExitRoom()->GetWorldPosition();
    DrawSphere(startPos, 0.5f, GREEN);
    DrawSphere(exitPos, 0.5f, RED);
    
    EndMode3D();
    
    int cx = GetScreenWidth()/2, cy = GetScreenHeight()/2;
    DrawCircle(cx, cy, 5, Fade(WHITE, 0.5f));
    DrawCircle(cx, cy, 2, WHITE);
    
    if (m_showMinimap) {
        m_minimap.Draw(m_showDebug, m_currentRoom, m_camera.GetAngle());
    }
    
    if (m_showDebug) {
        int y = 60;
        DrawText("DEBUG INFO (F1 to hide)", 10, y, 20, YELLOW); y += 25;
        DrawText(TextFormat("Angle: %.1f°", m_camera.GetAngle() * RAD2DEG), 10, y, 16, WHITE); y += 20;
        
        Direction looking = GetHoveredDoor();
        const char* hoverName = "NONE";
        if (looking == Direction::NORTH) hoverName = "NORTH";
        else if (looking == Direction::SOUTH) hoverName = "SOUTH";
        else if (looking == Direction::EAST) hoverName = "EAST";
        else if (looking == Direction::WEST) hoverName = "WEST";
        DrawText(TextFormat("Hover: %s", hoverName), 10, y, 16, WHITE); y += 20;
        
        DrawText(TextFormat("Doors: N:%d S:%d E:%d W:%d",
                 m_currentRoom->HasDoor(Direction::NORTH),
                 m_currentRoom->HasDoor(Direction::SOUTH),
                 m_currentRoom->HasDoor(Direction::EAST),
                 m_currentRoom->HasDoor(Direction::WEST)), 10, y, 16, WHITE);
    }
    
    if (m_currentRoom == m_maze.GetExitRoom()) {
        DrawText("YOU ESCAPED! Congratulations!", GetScreenWidth()/2-150, GetScreenHeight()/2, 30, GREEN);
    }
    
    EndDrawing();
}

void Game::MoveToRoom(Room* newRoom) {
    if (!newRoom) return;
    
    Direction cameFrom = Direction::NONE;
    for (int i = 0; i < 4; ++i) {
        if (m_currentRoom->GetNeighbor(static_cast<Direction>(i)) == newRoom) {
            cameFrom = static_cast<Direction>(i);
            break;
        }
    }
    
    m_currentRoom = newRoom;
    m_currentRoom->SetExplored(true);
    
    float newAngle = 0.0f;
    switch(cameFrom) {
        case Direction::NORTH: newAngle = 0.0f; break;
        case Direction::EAST:  newAngle = 3 * M_PI / 2; break;
        case Direction::SOUTH: newAngle = M_PI; break;
        case Direction::WEST:  newAngle = M_PI / 2; break;
        default: break;
    }
    m_camera.SetAngle(newAngle);
    m_camera.SetPitch(0.0f);
    m_camera.Update(m_currentRoom);
}

Direction Game::GetHoveredDoor() {
    Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, m_camera.GetCamera());
    Vector3 pos = m_currentRoom->GetWorldPosition();
    float half = Constants::ROOM_SIZE / 2.0f;
    
    if (m_currentRoom->HasDoor(Direction::NORTH)) {
        Door door({pos.x, Constants::DOOR_HEIGHT/2, pos.z + half}, Direction::NORTH);
        if (GetRayCollisionBox(ray, door.GetBoundingBox()).hit) return Direction::NORTH;
    }
    if (m_currentRoom->HasDoor(Direction::SOUTH)) {
        Door door({pos.x, Constants::DOOR_HEIGHT/2, pos.z - half}, Direction::SOUTH);
        if (GetRayCollisionBox(ray, door.GetBoundingBox()).hit) return Direction::SOUTH;
    }
    if (m_currentRoom->HasDoor(Direction::EAST)) {
        Door door({pos.x + half, Constants::DOOR_HEIGHT/2, pos.z}, Direction::EAST);
        if (GetRayCollisionBox(ray, door.GetBoundingBox()).hit) return Direction::EAST;
    }
    if (m_currentRoom->HasDoor(Direction::WEST)) {
        Door door({pos.x - half, Constants::DOOR_HEIGHT/2, pos.z}, Direction::WEST);
        if (GetRayCollisionBox(ray, door.GetBoundingBox()).hit) return Direction::WEST;
    }
    return Direction::NONE;
}