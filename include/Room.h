#ifndef ROOM_H
#define ROOM_H

#include "Constants.h"
#include <raylib.h>

enum class Direction { 
    NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3, NONE = -1 
};

class Room {
public:
    Room();
    Room(int x, int y);
    
    void SetDoor(Direction dir, bool has);
    bool HasDoor(Direction dir) const;
    Room* GetNeighbor(Direction dir) const;
    void SetNeighbor(Direction dir, Room* neighbor);
    
    Vector3 GetWorldPosition() const { return worldPosition; }
    int GetGridX() const { return gridX; }
    int GetGridY() const { return gridY; }
    int GetId() const { return id; }
    bool IsExplored() const { return explored; }
    void SetExplored(bool e) { explored = e; }
    bool IsExit() const { return Exit; }
    
    bool HasSlotMachine() const { return hasSlotMachine; }
    void SetSlotMachine(bool value) { hasSlotMachine = value; }
    Direction GetSlotMachineWall() const { return slotMachineWall; }
    void SetSlotMachineWall(Direction wall) { slotMachineWall = wall; }
    
    // Публичные поля для обратной совместимости
    int id;
    int gridX, gridY;
    Vector3 worldPosition;
    bool explored;
    Room* neighbors[4];
    bool hasDoor[4];
    bool Exit;
    
private:
    bool hasSlotMachine;
    Direction slotMachineWall;
};

#endif