#include "Game.h"
#include "Door.h"
#include "Constants.h"
#include "Menu.h"
#include <iostream>
#include <raymath.h>

Game::Game(int minSize, int maxSize)
    : m_floorManager(minSize, maxSize),
      m_state(MENU),
      m_musicLoaded(false),
      m_coinSoundLoaded(false),
      m_winSoundLoaded(false), m_loseSoundLoaded(false),
      m_doorSoundLoaded(false),
      m_totalCoins(0),
      m_errorMessageTimer(0.0f),
      m_showMinimap(true), m_showDebug(false), m_devMode(true)
{
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, "Maze Explorer");
    InitAudioDevice();
    SetTargetFPS(60);

    LoadSounds();

    m_slotMachine.SetAddCoinsCallback([this](int amount) { AddCoins(amount); });
    m_slotMachine.SetWinSound(m_winSound);
    m_slotMachine.SetLoseSound(m_loseSound);
}

Game::~Game() {
    if (m_musicLoaded) UnloadSound(m_music);
    if (m_coinSoundLoaded) UnloadSound(m_coinSound);
    if (m_winSoundLoaded) UnloadSound(m_winSound);
    if (m_loseSoundLoaded) UnloadSound(m_loseSound);
    if (m_doorSoundLoaded) UnloadSound(m_doorSound);
    CloseAudioDevice();
    CloseWindow();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        if (m_state == MENU) {
            m_menu.Update();
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
        else {
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
    PlaySound(m_music);

}

void Game::HandleInput() {
    Vector2 mouseDelta = GetMouseDelta();
    m_camera.HandleMouseInput(mouseDelta);
    
    if (IsKeyPressed(KEY_M)) {
        m_showMinimap = !m_showMinimap;
    }
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, m_camera.GetCamera());
        
        if (m_currentRoom->HasSlotMachine()) {
            if (IsHittingMachine(ray)) {
                if (m_totalCoins <= 0) {
                    m_errorMessage = "NOT ENOUGH COINS! GET A JOB!";
                    m_errorMessageTimer = 5.0f;
                    return;
                    }
                m_slotMachine.Start(m_totalCoins);
                m_state = SLOT_MACHINE;
                EnableCursor();
                return;
                }
            }
        if (m_currentRoom == m_floorManager.GetExitRoom()) {
            if (IsHatchHovered(ray)) {
                PlaySound(m_music);
                m_floorManager.NextFloor();
                m_currentRoom = m_floorManager.GetStartRoom();
                m_currentRoom->SetExplored(true);
                m_camera.SetAngle(0.0f);
                m_camera.SetPitch(0.0f);
                m_camera.Update(m_currentRoom);
                return;
                }
            }
        Direction door = GetHoveredDoor();
        if (door != Direction::NONE) {
            Room* next = m_currentRoom->GetNeighbor(door);
            if (next) {
                PlaySound(m_doorSound);
                MoveToRoom(next);
                }
            } 
        else {
            auto& coins = m_floorManager.GetCoins();
            for (auto& coin : coins) {
                if (!coin.IsCollected()) {
                    RayCollision coll = GetRayCollisionBox(ray, coin.GetBoundingBox());
                    if (coll.hit) {
                        coin.Collect();
                        AddCoins(1);
                        PlaySound(m_coinSound);
                        break;
                    }
                }
            }
        }
    }
}

void Game::Update() {
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

        for (const auto& coin : m_floorManager.GetCoins()) {
            coin.Draw();
        }
        
        EndMode3D();
        int cx = GetScreenWidth()/2, cy = GetScreenHeight()/2;
        DrawCircle(cx, cy, 5, Fade(WHITE, 0.5f));
        DrawCircle(cx, cy, 2, WHITE);
        
        if (m_showMinimap) {
            Vector2 f = m_camera.GetPlanarForwardXZ();
            m_minimap.Draw(m_showDebug, m_currentRoom, f.x, f.y, m_floorManager.GetCurrentMaze());
        }

        DrawCircle(25, 25, 15, YELLOW);
        DrawText(TextFormat("%d", m_totalCoins), 45, 15, 30, WHITE);

        if (m_errorMessageTimer > 0) {
            m_errorMessageTimer -= GetFrameTime();
            int tw = MeasureText(m_errorMessage.c_str(), 25);
            DrawText(m_errorMessage.c_str(), GetScreenWidth()/2 - tw/2, GetScreenHeight()/2 - 50, 25, RED);
            }

        if (m_currentRoom->HasSlotMachine()) {
            const char* msg = "Press LEFT CLICK on the machine to play SLOT!";
            int tw = MeasureText(msg, 20);
            DrawText(msg, GetScreenWidth()/2 - tw/2, GetScreenHeight() - 50, 20, YELLOW);
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

bool Game::IsHatchHovered(Ray ray) {
    float half = Constants::ROOM_SIZE / 2.0f;
    float offset = 1.2f;
    
    Vector3 roomPos = m_currentRoom->GetWorldPosition();
    Vector3 hatchCenter = {
        roomPos.x + half - offset,
        0.05f,
        roomPos.z + half - offset
    };

    BoundingBox hatchBox = {
        {hatchCenter.x - 0.75f, hatchCenter.y - 0.1f, hatchCenter.z - 0.75f},
        {hatchCenter.x + 0.75f, hatchCenter.y + 0.2f, hatchCenter.z + 0.75f}
    };
    
    RayCollision coll = GetRayCollisionBox(ray, hatchBox);
    return coll.hit;
}

bool Game::IsHittingMachine(Ray ray){
    const float machineWidth = 2.4f;
    const float machineHeight = 1.9f;
    const float machineDepth = 1.6f;
    const float halfRoom = Constants::ROOM_SIZE / 2.0f;
            
    Vector3 roomPos = m_currentRoom->GetWorldPosition();
    Vector3 machineCenter = {
        roomPos.x,
        machineHeight / 2.0f,
        roomPos.z - halfRoom + machineDepth / 2.0f
        };
    BoundingBox slotBox = {
        {machineCenter.x - machineWidth/2, machineCenter.y - machineHeight/2, machineCenter.z - machineDepth/2},
        {machineCenter.x + machineWidth/2, machineCenter.y + machineHeight/2, machineCenter.z + machineDepth/2}
        };
    RayCollision coll = GetRayCollisionBox(ray, slotBox);
    return coll.hit;
}

void Game::LoadSounds(){
        m_music = LoadSound("../resources/menu_music.ogg");
    if (m_music.stream.buffer != nullptr) {
        m_musicLoaded = true;
    } else {
        m_musicLoaded = false;
    }

    m_coinSound = LoadSound("../resources/coin.ogg");
    if (m_coinSound.stream.buffer != nullptr) {
        m_coinSoundLoaded = true;
    } else {
        m_coinSoundLoaded = false;
    }

    m_winSound = LoadSound("../resources/win.ogg");
    if (m_winSound.stream.buffer != nullptr) {
        m_winSoundLoaded = true;
    } else {
        m_winSoundLoaded = false;
    }
    
    m_loseSound = LoadSound("../resources/lose.ogg");
    if (m_loseSound.stream.buffer != nullptr) {
        m_loseSoundLoaded = true;
    } else {
        m_loseSoundLoaded = false;
    }

    m_doorSound = LoadSound("../resources/door.ogg");
    if (m_doorSound.stream.buffer != nullptr) {
        m_doorSoundLoaded = true;
    } else {
        m_doorSoundLoaded = false;
    }
}