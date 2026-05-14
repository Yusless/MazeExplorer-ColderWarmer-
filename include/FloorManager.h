#ifndef FLOOR_MANAGER_H
#define FLOOR_MANAGER_H

#include <memory>
#include "MazeGraph.h"

class FloorManager {
public:
    FloorManager(int width, int height);
    ~FloorManager() = default;

    void GenerateFirstFloor();          // начальный этаж (первый)
    void NextFloor();                   // переход на следующий этаж (удаляет старый лабиринт, создаёт новый)

    // Доступ к текущему лабиринту
    MazeGraph* GetCurrentMaze() const { return m_currentMaze.get(); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetCurrentFloor() const { return m_currentFloor; }

    // Удобные обёртки
    Room* GetStartRoom() const { return m_currentMaze ? m_currentMaze->GetStartRoom() : nullptr; }
    Room* GetExitRoom() const { return m_currentMaze ? m_currentMaze->GetExitRoom() : nullptr; }
    Room* GetRoom(int x, int y) const { return m_currentMaze ? m_currentMaze->GetRoom(x, y) : nullptr; }
    void PrintGraph() const { if (m_currentMaze) m_currentMaze->PrintGraph(); }

private:
    std::unique_ptr<MazeGraph> m_currentMaze;
    int m_width;
    int m_height;
    int m_currentFloor;
};

#endif