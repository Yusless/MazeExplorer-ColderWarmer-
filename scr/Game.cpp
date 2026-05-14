#include "Game.h"
#include "Door.h"
#include "Constants.h"
#include "Menu.h"
#include <iostream>
#include <raymath.h>

Game::Game(int mazeWidth, int mazeHeight)
    : m_floorManager(mazeWidth, mazeHeight),
      m_state(MENU), m_musicLoaded(false), m_musicStarted(false),
      m_showMinimap(true), m_showDebug(false), m_devMode(true) {
    
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, "Maze Explorer");
    InitAudioDevice();
    SetTargetFPS(60);
    
    if (!IsAudioDeviceReady()) {
        std::cerr << "ERROR: Failed to initialize audio device!" << std::endl;
    } else {
        std::cout << "Audio device initialized successfully" << std::endl;
    }
    
    // Поиск музыкального файла в разных папках
    const char* musicPaths[] = {
        "menu_music.ogg",
        "assets/menu_music.ogg",
        "../menu_music.ogg",
        "../../menu_music.ogg",
        "../../assets/menu_music.ogg"
    };
    bool found = false;
    for (const char* path : musicPaths) {
        if (FileExists(path)) {
            m_music = LoadMusicStream(path);
            m_music.looping = false;
            m_musicLoaded = true;
            found = true;
            std::cout << "Music loaded from: " << path << std::endl;
            break;
        }
    }
    if (!found) {
        std::cerr << "Music file not found. Place menu_music.ogg in build/scr/ or assets/ folder." << std::endl;
    }
}

Game::~Game() {
    if (m_musicLoaded) UnloadMusicStream(m_music);
    CloseAudioDevice();
    CloseWindow();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        if (m_state == MENU) {
            m_menu.Update();
            if (m_musicLoaded && IsMusicStreamPlaying(m_music)) {
                UpdateMusicStream(m_music);
            }
            BeginDrawing();
            m_menu.Draw();
            EndDrawing();
            if (m_menu.ShouldStart()) {
                StartGame();
            }
        } else {
            HandleInput();
            Update();
            Draw();
        }
    }
}

void Game::StartGame() {
    if (m_state != MENU) return;
    m_state = PLAYING;
    DisableCursor();
    
    m_floorManager.GenerateFirstFloor();
    m_currentRoom = m_floorManager.GetStartRoom();
    m_currentRoom->SetExplored(true);
    
    if (m_musicLoaded && !m_musicStarted) {
        PlayMusicStream(m_music);
        m_musicStarted = true;
        std::cout << "Music started (plays once)" << std::endl;
    }
}

void Game::HandleInput() {
    Vector2 mouseDelta = GetMouseDelta();
    m_camera.HandleMouseInput(mouseDelta);
    
    if (IsKeyPressed(KEY_M)) {
        m_showMinimap = !m_showMinimap;
        std::cout << "Minimap: " << (m_showMinimap ? "ON" : "OFF") << std::endl;
    }
    if (IsKeyPressed(KEY_F1)) {
        m_showDebug = !m_showDebug;
        std::cout << "Debug: " << (m_showDebug ? "ON" : "OFF") << std::endl;
    }
    if (IsKeyPressed(KEY_P)) {
        m_floorManager.PrintGraph();
    }
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Direction door = GetHoveredDoor();
        if (door != Direction::NONE) {
            Room* next = m_currentRoom->GetNeighbor(door);
            if (next) {
                MoveToRoom(next);
                if (m_currentRoom == m_floorManager.GetExitRoom()) {
                    m_floorManager.NextFloor();
                    m_currentRoom = m_floorManager.GetStartRoom();
                    m_currentRoom->SetExplored(true);
                    m_camera.SetAngle(0.0f);
                    m_camera.SetPitch(0.0f);
                    m_camera.Update(m_currentRoom);
                    std::cout << "=== Entered floor " << m_floorManager.GetCurrentFloor() << " ===" << std::endl;
                }
            }
        }
    }
}

void Game::Update() {
    if (m_musicLoaded && IsMusicStreamPlaying(m_music)) {
        UpdateMusicStream(m_music);
        if (!IsMusicStreamPlaying(m_music)) {
            std::cout << "Music finished playing" << std::endl;
        }
    }
    if (m_state == PLAYING) {
        m_camera.Update(m_currentRoom);
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode3D(m_camera.GetCamera());
    m_renderer3D.DrawRoom(m_currentRoom);
    Direction hovered;
    m_renderer3D.DrawDoors(m_currentRoom, m_camera.GetCamera(), hovered);
    
    Vector3 startPos = m_floorManager.GetStartRoom()->GetWorldPosition();
    Vector3 exitPos = m_floorManager.GetExitRoom()->GetWorldPosition();
    DrawSphere(startPos, 0.5f, GREEN);
    DrawSphere(exitPos, 0.5f, RED);
    
    EndMode3D();
    
    int cx = GetScreenWidth()/2, cy = GetScreenHeight()/2;
    DrawCircle(cx, cy, 5, Fade(WHITE, 0.5f));
    DrawCircle(cx, cy, 2, WHITE);
    
    if (m_showMinimap) {
        Vector2 f = m_camera.GetPlanarForwardXZ();
        m_minimap.Draw(m_showDebug, m_currentRoom, f.x, f.y, m_floorManager.GetCurrentMaze());
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
        DrawText(TextFormat("Floor: %d", m_floorManager.GetCurrentFloor()), 10, y+20, 16, SKYBLUE);
    }
    
    if (m_currentRoom == m_floorManager.GetExitRoom()) {
        DrawText("YOU ESCAPED! Go to next floor!", GetScreenWidth()/2-200, GetScreenHeight()/2, 30, GREEN);
    }
    
    EndDrawing();
}

void Game::MoveToRoom(Room* newRoom) {
    if (!newRoom) return;
    Room* oldRoom = m_currentRoom;
    m_currentRoom = newRoom;
    m_currentRoom->SetExplored(true);
    
    Vector3 delta = Vector3Subtract(m_currentRoom->GetWorldPosition(), oldRoom->GetWorldPosition());
    delta.y = 0.0f;
    const float len = Vector3Length(delta);
    if (len > 0.01f) {
        Vector3 deltaN = Vector3Scale(delta, 1.0f / len);
        float newAngle = atan2f(deltaN.x, deltaN.z);
        const float TWO_PI = 2.0f * static_cast<float>(M_PI);
        while (newAngle < 0.0f) newAngle += TWO_PI;
        while (newAngle >= TWO_PI) newAngle -= TWO_PI;
        m_camera.SetAngle(newAngle);
        m_camera.SetPitch(0.0f);
        m_camera.Update(m_currentRoom);
        
        Vector3 f = m_camera.GetForward();
        Vector3 flatF = {f.x, 0.0f, f.z};
        if (Vector3Length(flatF) > 1e-5f) {
            flatF = Vector3Normalize(flatF);
            if (Vector3DotProduct(flatF, deltaN) < 0.0f) {
                newAngle += (float)M_PI;
                if (newAngle >= TWO_PI) newAngle -= TWO_PI;
                m_camera.SetAngle(newAngle);
            }
        }
    }
    m_camera.SetPitch(0.0f);
    m_camera.Update(m_currentRoom);
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
}

Direction Game::GetHoveredDoor() {
    Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, m_camera.GetCamera());
    return PickClosestDoorAlongRay(m_currentRoom, ray);
}