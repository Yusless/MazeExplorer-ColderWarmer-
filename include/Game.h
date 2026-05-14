#ifndef GAME_H
#define GAME_H

#include "FloorManager.h"
#include "CameraController.h"
#include "Renderer3D.h"
#include "MinimapRenderer.h"
#include "Menu.h"

class Game {
public:
    Game(int mazeWidth, int mazeHeight);
    ~Game();
    void Run();

private:
    enum GameState { MENU, PLAYING };

    void HandleInput();
    void Update();
    void Draw();
    void MoveToRoom(Room* newRoom);
    Direction GetHoveredDoor();
    void StartGame();
    void AddCoin() { ++m_totalCoins; }
    void SpendCoins(int amount) { if (m_totalCoins >= amount) m_totalCoins -= amount; }

    FloorManager m_floorManager;
    Room* m_currentRoom;
    CameraController m_camera;
    Renderer3D m_renderer3D;
    MinimapRenderer m_minimap;
    Menu m_menu;

    GameState m_state;
    Music m_music;
    bool m_musicLoaded;
    bool m_musicStarted;

    bool m_showMinimap;
    bool m_showDebug;
    bool m_devMode;

    int m_totalCoins;   // счётчик монет
};

#endif