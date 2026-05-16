#include "FloorManager.h"
#include <iostream>
#include <random>
#include <ctime>
#include <algorithm>
#include <vector>
#include <string>

FloorManager::FloorManager(int minSize, int maxSize) :
    m_currentFloor(0),
    m_minSize(minSize),
    m_maxSize(maxSize),
    m_width(minSize),
    m_height(minSize) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void FloorManager::GenerateFirstFloor() {
    m_currentFloor = 1;
    m_currentMaze = std::make_unique<MazeGraph>(5,5);
    m_currentMaze->Generate();
    m_width = m_currentMaze->GetWidth();
    m_height = m_currentMaze->GetHeight();
    GenerateCoins();
    PlaceSlotMachineRoom();
}

void FloorManager::NextFloor() {
    ++m_currentFloor;
    m_width = m_minSize + std::rand() % (m_maxSize - m_minSize + 1);
    m_height = m_minSize + std::rand() % (m_maxSize - m_minSize + 1);
    m_currentMaze = std::make_unique<MazeGraph>(m_width, m_height);
    m_currentMaze->Generate();
    m_width = m_currentMaze->GetWidth();
    m_height = m_currentMaze->GetHeight();
    GenerateCoins();
    PlaceSlotMachineRoom();
}

void FloorManager::GenerateCoins() {
    if (!m_currentMaze) return;
    m_coins.clear();
    int w = m_currentMaze->GetWidth();
    int h = m_currentMaze->GetHeight();
    int coinCount = m_currentFloor * (w * h) / 4;
    coinCount = std::max(5, std::min(coinCount, 75));
    std::uniform_int_distribution<int> distX(0, w - 1);
    std::uniform_int_distribution<int> distZ(0, h - 1);
    std::uniform_real_distribution<float> distOffset(-3.0f, 3.0f);
    std::mt19937 gen(std::random_device{}());
    for (int i = 0; i < coinCount; ++i) {
        int x = distX(gen);
        int z = distZ(gen);
        Room* room = m_currentMaze->GetRoom(x, z);
        if (room && !room->HasSlotMachine()) {
            Vector3 pos = room->GetWorldPosition();
            pos.x += distOffset(gen);
            pos.z += distOffset(gen);
            pos.y = 0.25f;
            m_coins.emplace_back(pos);
        }
    }
}

void FloorManager::PlaceSlotMachineRoom() {
    if (!m_currentMaze) return;
    int w = m_currentMaze->GetWidth();
    int h = m_currentMaze->GetHeight();

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Room* r = m_currentMaze->GetRoom(x, y);
            if (r) {
                r->SetSlotMachine(false);
                r->SetSlotMachineWall(Direction::NONE);
            }
        }
    }

    std::vector<Room*> candidates;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Room* room = m_currentMaze->GetRoom(x, y);
            if (!room) continue;

            bool isStartOrExit = (room == GetStartRoom() || room == GetExitRoom());
            bool hasEastDoor = room->HasDoor(Direction::NORTH);
            Room* neighbor = room->GetNeighbor(Direction::NORTH);
            bool neighborHasWestDoor = neighbor ? neighbor->HasDoor(Direction::SOUTH) : false;
            bool suitable = (!isStartOrExit && !hasEastDoor && !neighborHasWestDoor);
            if (suitable) candidates.push_back(room);
        }
    }

    if (candidates.empty()) {
        return;
    }

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    Room* chosen = candidates[dist(rng)];
    chosen->SetSlotMachine(true);
    chosen->SetSlotMachineWall(Direction::EAST);
}