#ifndef FLOOR_MANAGER_H
#define FLOOR_MANAGER_H

#include <memory>
#include <vector>
#include "MazeGraph.h"
#include "Coin.h"
#include "Room.h"

class FloorManager {
public:
    FloorManager(int minSize = 5, int maxSize = 15);
    ~FloorManager() = default;

    void GenerateFirstFloor();
    void NextFloor();

    MazeGraph* GetCurrentMaze() const { return m_currentMaze.get(); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetCurrentFloor() const { return m_currentFloor; }

    Room* GetStartRoom() const { return m_currentMaze ? m_currentMaze->GetStartRoom() : nullptr; }
    Room* GetExitRoom() const { return m_currentMaze ? m_currentMaze->GetExitRoom() : nullptr; }
    Room* GetRoom(int x, int y) const { return m_currentMaze ? m_currentMaze->GetRoom(x, y) : nullptr; }
    void PrintGraph() const { if (m_currentMaze) m_currentMaze->PrintGraph(); }

    const std::vector<Coin>& GetCoins() const { return m_coins; }
    std::vector<Coin>& GetCoins() { return m_coins; }
    void ClearCoins() { m_coins.clear(); }

private:
    void GenerateCoins();
    void PlaceSlotMachineRoom();   // <-- ДОБАВИТЬ ЭТУ СТРОКУ

    std::unique_ptr<MazeGraph> m_currentMaze;
    int m_width;
    int m_height;
    int m_currentFloor;
    int m_minSize;
    int m_maxSize;
    std::vector<Coin> m_coins;
};

#endif