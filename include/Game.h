#ifndef GAME_H
#define GAME_H

#include "FloorManager.h"
#include "CameraController.h"
#include "Renderer3D.h"
#include "MinimapRenderer.h"
#include "Menu.h"
#include "SlotMachine.h"
#include <string>

class Game {
public:
    Game(int mazeWidth, int mazeHeight);
    ~Game();
    void Run();

private:
    enum GameState { MENU, PLAYING, SLOT_MACHINE };
    
    void HandleInput();
    void Update();
    void Draw();
    void MoveToRoom(Room* newRoom);
    Direction GetHoveredDoor();
    void StartGame();
    void AddCoins(int amount);
    bool IsHatchHovered(Ray ray);

    FloorManager m_floorManager;
    Room* m_currentRoom;
    CameraController m_camera;
    Renderer3D m_renderer3D;
    MinimapRenderer m_minimap;
    Menu m_menu;
    SlotMachine m_slotMachine;
    
    GameState m_state;
    
    Music m_music;
    bool m_musicLoaded;
    bool m_musicStarted;
    
    Sound m_coinSound;
    bool m_coinSoundLoaded;
    
    int m_totalCoins;
    
    // Для сообщения об ошибке
    float m_errorMessageTimer;
    std::string m_errorMessage;
    
    bool m_showMinimap;
    bool m_showDebug;
    bool m_devMode;
};

#endif