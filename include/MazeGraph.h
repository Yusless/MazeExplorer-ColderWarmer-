// MazeGraph.h
#ifndef MAZE_GRAPH_H
#define MAZE_GRAPH_H

#include "Room.h"
#include <vector>
#include <random>
#include <stack>
#include <memory>
#include <map>

class MazeGraph {
public:
    MazeGraph(int width, int height);
    
    void Generate();
    void PlaceKeys();
    void SetStartRoom(int x, int y);
    void SetExitRoom(int x, int y);
    
    Room* GetRoom(int x, int y);
    Room* GetStartRoom() { return m_startRoom; }
    Room* GetExitRoom() { return m_exitRoom; }
    
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
    void PrintGraph() const;
    
private:
    int m_width, m_height;
    std::vector<std::vector<std::unique_ptr<Room>>> m_rooms;
    Room* m_startRoom = nullptr;
    Room* m_exitRoom = nullptr;
    
    std::random_device m_rd;
    std::mt19937 m_gen;
    
    void ConnectRooms(Room* from, Room* to, Direction dir);
    void LockRandomDoors();
    std::vector<Room*> FindPathWithoutKey(Room* start, Room* end, KeyType keyToIgnore);
};

#endif