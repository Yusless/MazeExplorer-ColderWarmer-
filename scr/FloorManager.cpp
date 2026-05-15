#include "FloorManager.h"
#include <iostream>
#include <random>
#include <algorithm>

FloorManager::FloorManager(int width, int height)
    : m_width(width), m_height(height), m_currentFloor(0) {}

void FloorManager::GenerateFirstFloor() {
    m_currentFloor = 1;
    m_currentMaze = std::make_unique<MazeGraph>(m_width, m_height);
    m_currentMaze->Generate();
    GenerateCoins();
    PlaceSlotMachineRoom();
    std::cout << "=== Entering floor " << m_currentFloor << " ===" << std::endl;
}

void FloorManager::NextFloor() {
    ++m_currentFloor;
    m_currentMaze = std::make_unique<MazeGraph>(m_width, m_height);
    m_currentMaze->Generate();
    GenerateCoins();
    PlaceSlotMachineRoom();
    std::cout << "=== Moved to floor " << m_currentFloor << " ===" << std::endl;
}

void FloorManager::GenerateCoins() {
    m_coins.clear();
    int numCoins = (m_width * m_height) / 2;  // побольше монеток
    if (numCoins < 6) numCoins = 6;

    std::cout << "Generating " << numCoins << " coins on floor " << m_currentFloor << std::endl;

    std::uniform_int_distribution<int> distX(0, m_width - 1);
    std::uniform_int_distribution<int> distZ(0, m_height - 1);
    std::uniform_real_distribution<float> distOffset(-3.0f, 3.0f); // смещение в комнате
    std::mt19937 gen(std::random_device{}());

    int generated = 0;
    for (int i = 0; i < numCoins; ++i) {
        int x = distX(gen);
        int z = distZ(gen);
        Room* room = m_currentMaze->GetRoom(x, z);
        // Не кладём монетки в комнату с автоматом
        if (room && !room->HasSlotMachine()) {
            float offsetX = distOffset(gen);
            float offsetZ = distOffset(gen);
            Vector3 pos = room->GetWorldPosition();
            pos.x += offsetX;
            pos.z += offsetZ;
            pos.y = 0.25f;
            m_coins.emplace_back(pos);
            generated++;
        }
    }
    std::cout << "Actually placed coins: " << generated << std::endl;
}

void FloorManager::PlaceSlotMachineRoom() {
    // Сбросить все флаги
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Room* r = m_currentMaze->GetRoom(x, y);
            if (r) r->SetSlotMachine(false);
        }
    }

    std::vector<Room*> candidates;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Room* r = m_currentMaze->GetRoom(x, y);
            if (r && r != GetStartRoom() && r != GetExitRoom()) {
                candidates.push_back(r);
            }
        }
    }
    if (candidates.empty()) {
        std::cerr << "Warning: No suitable room for slot machine!" << std::endl;
        return;
    }
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, candidates.size() - 1);
    Room* slotRoom = candidates[dist(rng)];
    slotRoom->SetSlotMachine(true);
    std::cout << "Slot machine placed in room (" 
              << slotRoom->GetGridX() << "," << slotRoom->GetGridY() 
              << ") on floor " << m_currentFloor << std::endl;
}