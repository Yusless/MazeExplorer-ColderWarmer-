#include "Room.h"
#include "MazeGraph.h"
#include <iostream>
#include <stack>
#include <map>
#include <algorithm>
#include <random>

MazeGraph::MazeGraph(int width, int height) 
    : m_width(width), m_height(height), m_gen(m_rd()) {
    
    m_rooms.resize(height);
    for (int y = 0; y < height; ++y) {
        m_rooms[y].resize(width);
        for (int x = 0; x < width; ++x) {
            m_rooms[y][x] = Room(x, y);
            m_rooms[y][x].id = y * width + x;
        }
    }
}

void MazeGraph::Generate() {
    std::vector<std::vector<char>> visited(m_height, std::vector<char>(m_width, false));
    std::stack<Room*> stack;
    
    int startX = m_gen() % m_width;
    int startY = m_gen() % m_height;
    Room* current = &m_rooms[startY][startX];
    
    visited[startY][startX] = true;
    stack.push(current);
    
    while (!stack.empty()) {
        current = stack.top();
        
        std::vector<std::pair<Direction, Room*>> unvisitedNeighbors;
        
        if (current->gridY > 0 && !visited[current->gridY-1][current->gridX]) {
            unvisitedNeighbors.push_back({Direction::NORTH, 
                &m_rooms[current->gridY-1][current->gridX]});
        }
        if (current->gridY < m_height-1 && !visited[current->gridY+1][current->gridX]) {
            unvisitedNeighbors.push_back({Direction::SOUTH, 
                &m_rooms[current->gridY+1][current->gridX]});
        }
        if (current->gridX > 0 && !visited[current->gridY][current->gridX-1]) {
            unvisitedNeighbors.push_back({Direction::WEST, 
                &m_rooms[current->gridY][current->gridX-1]});
        }
        if (current->gridX < m_width-1 && !visited[current->gridY][current->gridX+1]) {
            unvisitedNeighbors.push_back({Direction::EAST, 
                &m_rooms[current->gridY][current->gridX+1]});
        }
        
        if (!unvisitedNeighbors.empty()) {
            std::shuffle(unvisitedNeighbors.begin(), unvisitedNeighbors.end(), m_gen);
            auto [dir, nextRoom] = unvisitedNeighbors[0];
            
            ConnectRooms(current, nextRoom, dir);
            
            visited[nextRoom->gridY][nextRoom->gridX] = true;
            stack.push(nextRoom);
        } else {
            stack.pop();
        }
    }
    
    m_startRoom = &m_rooms[0][0];
    m_exitRoom = &m_rooms[m_height-1][m_width-1];
    
    std::cout << "Maze generated: " << m_width << "x" << m_height << std::endl;
}

void MazeGraph::ConnectRooms(Room* from, Room* to, Direction dir) {
    Direction opposite;
    switch(dir) {
        case Direction::NORTH: opposite = Direction::SOUTH; break;
        case Direction::SOUTH: opposite = Direction::NORTH; break;
        case Direction::EAST:  opposite = Direction::WEST; break;
        case Direction::WEST:  opposite = Direction::EAST; break;
    }
    
    from->hasDoor[static_cast<int>(dir)] = true;
    from->neighbors[static_cast<int>(dir)] = to;
    
    to->hasDoor[static_cast<int>(opposite)] = true;
    to->neighbors[static_cast<int>(opposite)] = from;
}

Room* MazeGraph::GetRoom(int x, int y) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        return &m_rooms[y][x];
    }
    return nullptr;
}

void MazeGraph::SetStartRoom(int x, int y) {
    if (auto room = GetRoom(x, y)) {
        m_startRoom = room;
    }
}

void MazeGraph::SetExitRoom(int x, int y) {
    if (auto room = GetRoom(x, y)) {
        m_exitRoom = room;
    }
}

void MazeGraph::PrintGraph() const {
    std::cout << "\nMaze Graph:\n";
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            std::cout << "+";
            std::cout << (m_rooms[y][x].hasDoor[0] ? "   " : "---");
        }
        std::cout << "+\n";
        
        for (int x = 0; x < m_width; ++x) {
            std::cout << (m_rooms[y][x].hasDoor[3] ? " " : "|");
            if (&m_rooms[y][x] == m_startRoom) std::cout << " S ";
            else if (&m_rooms[y][x] == m_exitRoom) std::cout << " E ";
            else std::cout << "   ";
        }
        std::cout << "|\n";
    }
    for (int x = 0; x < m_width; ++x) std::cout << "+---";
    std::cout << "+\n";
}