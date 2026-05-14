#include "FloorManager.h"
#include <iostream>

FloorManager::FloorManager(int width, int height)
    : m_width(width), m_height(height), m_currentFloor(0) {
}

void FloorManager::GenerateFirstFloor() {
    m_currentFloor = 1;
    m_currentMaze = std::make_unique<MazeGraph>(m_width, m_height);
    m_currentMaze->Generate();
    std::cout << "=== Entering floor " << m_currentFloor << " ===" << std::endl;
}

void FloorManager::NextFloor() {
    ++m_currentFloor;
    m_currentMaze = std::make_unique<MazeGraph>(m_width, m_height);
    m_currentMaze->Generate();
    std::cout << "=== Moved to floor " << m_currentFloor << " ===" << std::endl;
}