#include "Game.h"
#include "Door.h"
#include "Constants.h"
#include "Menu.h"
#include <iostream>
#include <raymath.h>

Game::Game(int mazeWidth, int mazeHeight)
    : m_floorManager(mazeWidth, mazeHeight),
      m_state(MENU),
      m_musicLoaded(false), m_musicStarted(false),
      m_coinSoundLoaded(false), m_totalCoins(0),
      m_errorMessageTimer(0.0f),
      m_showMinimap(true), m_showDebug(false), m_devMode(true)
{
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, "Maze Explorer");
    InitAudioDevice();
    SetTargetFPS(60);
    
    if (!IsAudioDeviceReady()) {
        std::cerr << "ERROR: Failed to initialize audio device!" << std::endl;
    } else {
        std::cout << "Audio device initialized successfully" << std::endl;
    }
    
    // Загрузка музыки (OGG)
    const char* musicPaths[] = {
        "menu_music.ogg",
        "assets/menu_music.ogg",
        "../menu_music.ogg",
        "../../menu_music.ogg",
        "../../assets/menu_music.ogg"
    };
    bool musicFound = false;
    for (const char* path : musicPaths) {
        if (FileExists(path)) {
            m_music = LoadMusicStream(path);
            m_music.looping = false;
            m_musicLoaded = true;
            musicFound = true;
            std::cout << "Music loaded from: " << path << std::endl;
            break;
        }
    }
    if (!musicFound) {
        std::cerr << "Music file not found. Place menu_music.ogg in build/scr/ or assets/ folder." << std::endl;
    }
    
    // Загрузка звука монетки (OGG или WAV)
    const char* coinPaths[] = {
        "coin.ogg", "coin.wav",
        "assets/coin.ogg", "assets/coin.wav",
        "../coin.ogg", "../../coin.ogg"
    };
    bool coinFound = false;
    for (const char* path : coinPaths) {
        if (FileExists(path)) {
            m_coinSound = LoadSound(path);
            m_coinSoundLoaded = true;
            coinFound = true;
            std::cout << "Coin sound loaded from: " << path << std::endl;
            break;
        }
    }
    if (!coinFound) {
        std::cerr << "Coin sound not found. Place coin.ogg or coin.wav in build/scr/ or assets/ folder." << std::endl;
    }
    
    // Настройка колбэка для автомата
    m_slotMachine.SetAddCoinsCallback([this](int amount) { AddCoins(amount); });
}

Game::~Game() {
    if (m_musicLoaded) UnloadMusicStream(m_music);
    if (m_coinSoundLoaded) UnloadSound(m_coinSound);
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
        } 
        else if (m_state == SLOT_MACHINE) {
            m_slotMachine.Update();
            BeginDrawing();
            ClearBackground(BLACK);
            m_slotMachine.Draw();
            EndDrawing();
            if (!m_slotMachine.IsActive()) {
                m_state = PLAYING;
                DisableCursor();
            }
        }
        else { // PLAYING
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
        Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, m_camera.GetCamera());
        
        // 1. Проверка на игровой автомат
        if (m_currentRoom->HasSlotMachine()) {
            Vector3 slotPos = m_currentRoom->GetWorldPosition();
            slotPos.y = 0.6f;
            BoundingBox slotBox = {
                {slotPos.x - 1.25f, slotPos.y - 0.8f, slotPos.z - 1.0f},
                {slotPos.x + 1.25f, slotPos.y + 0.8f, slotPos.z + 1.0f}
            };
            if (GetRayCollisionBox(ray, slotBox).hit) {
                // Проверка на наличие монет
                if (m_totalCoins <= 0) {
                    m_errorMessage = "NOT ENOUGH COINS! GET A JOB!";
                    m_errorMessageTimer = 5.0f;
                    return;
                }
                std::cout << "Slot machine activated!" << std::endl;
                m_slotMachine.Start(m_totalCoins);
                m_state = SLOT_MACHINE;
                EnableCursor();
                return;
            }
        }
        
        // 2. Проверка на дверь
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
        // 3. Сбор монеток
        else {
            auto& coins = m_floorManager.GetCoins();
            for (auto& coin : coins) {
                if (!coin.IsCollected()) {
                    RayCollision coll = GetRayCollisionBox(ray, coin.GetBoundingBox());
                    if (coll.hit) {
                        coin.Collect();
                        AddCoins(1);
                        if (m_coinSoundLoaded) PlaySound(m_coinSound);
                        std::cout << "Coin collected! Total: " << m_totalCoins << std::endl;
                        break;
                    }
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
    
    if (m_state == PLAYING) {
        BeginMode3D(m_camera.GetCamera());
        m_renderer3D.DrawRoom(m_currentRoom);
        Direction hovered;
        m_renderer3D.DrawDoors(m_currentRoom, m_camera.GetCamera(), hovered);
        
        // Отрисовка монеток
        for (const auto& coin : m_floorManager.GetCoins()) {
            coin.Draw();
        }
        
        // Отрисовка старта и выхода
        Vector3 startPos = m_floorManager.GetStartRoom()->GetWorldPosition();
        Vector3 exitPos = m_floorManager.GetExitRoom()->GetWorldPosition();
        DrawSphere(startPos, 0.5f, GREEN);
        DrawSphere(exitPos, 0.5f, RED);
        
        EndMode3D();
        
        // 2D интерфейс
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
        
        // Счётчик монет
        DrawCircle(25, 25, 15, YELLOW);
        DrawText(TextFormat("%d", m_totalCoins), 45, 15, 30, WHITE);
        
        // Сообщение об ошибке
        if (m_errorMessageTimer > 0) {
            m_errorMessageTimer -= GetFrameTime();
            int tw = MeasureText(m_errorMessage.c_str(), 25);
            DrawText(m_errorMessage.c_str(), GetScreenWidth()/2 - tw/2, GetScreenHeight()/2 - 50, 25, RED);
        }
        
        // Подсказка, если в комнате есть автомат
        if (m_currentRoom->HasSlotMachine()) {
            const char* msg = "Press LEFT CLICK on the machine to play SLOT!";
            int tw = MeasureText(msg, 20);
            DrawText(msg, GetScreenWidth()/2 - tw/2, GetScreenHeight() - 50, 20, YELLOW);
        }
        
        if (m_currentRoom == m_floorManager.GetExitRoom()) {
            DrawText("YOU ESCAPED! Go to next floor!", GetScreenWidth()/2-200, GetScreenHeight()/2, 30, GREEN);
        }
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

void Game::AddCoins(int amount) {
    m_totalCoins += amount;
    if (m_totalCoins < 0) m_totalCoins = 0;
    std::cout << "Coins updated: " << m_totalCoins << std::endl;
}