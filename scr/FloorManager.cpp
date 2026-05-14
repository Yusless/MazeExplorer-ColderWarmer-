#include "FloorManager.h"
#include <iostream>
#include <random>
#include <algorithm>

FloorManager::FloorManager(int width, int height)
    : m_width(width), m_height(height), m_currentFloor(0) {
}

void FloorManager::GenerateFirstFloor() {
    m_currentFloor = 1;
    m_currentMaze = std::make_unique<MazeGraph>(m_width, m_height);
    m_currentMaze->Generate();
    GenerateCoins();
    std::cout << "=== Entering floor " << m_currentFloor << " ===" << std::endl;
}

void FloorManager::NextFloor() {
    ++m_currentFloor;
    m_currentMaze = std::make_unique<MazeGraph>(m_width, m_height);
    m_currentMaze->Generate();
    GenerateCoins();
    std::cout << "=== Moved to floor " << m_currentFloor << " ===" << std::endl;
}

void FloorManager::GenerateCoins() {
    m_coins.clear();
    int numCoins = (m_width * m_height) / 3;
    if (numCoins < 3) numCoins = 3;
    std::uniform_int_distribution<int> distX(0, m_width - 1);
    std::uniform_int_distribution<int> distZ(0, m_height - 1);
    std::mt19937 gen(std::random_device{}());
    
    for (int i = 0; i < numCoins; ++i) {
        int x = distX(gen);
        int z = distZ(gen);
        Room* room = m_currentMaze->GetRoom(x, z);
        if (room && room != m_currentMaze->GetStartRoom() && room != m_currentMaze->GetExitRoom()) {
            float offsetX = (float)(gen() % 6 - 3) * 0.8f;
            float offsetZ = (float)(gen() % 6 - 3) * 0.8f;
            Vector3 pos = room->GetWorldPosition();
            pos.x += offsetX;
            pos.z += offsetZ;
            float half = 4.0f;
            pos.x = std::clamp(pos.x, room->GetWorldPosition().x - half + 0.8f, room->GetWorldPosition().x + half - 0.8f);
            pos.z = std::clamp(pos.z, room->GetWorldPosition().z - half + 0.8f, room->GetWorldPosition().z + half - 0.8f);
            pos.y = 0.2f;
            m_coins.emplace_back(pos);
        }
    }
}