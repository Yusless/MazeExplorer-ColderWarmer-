#include "Renderer3D.h"
#include "Wall.h"
#include "Floor.h"
#include "Door.h"
#include "Constants.h"
#include <raymath.h>

Renderer3D::Renderer3D() {}

void Renderer3D::DrawRoom(Room* room) {
    Vector3 pos = room->GetWorldPosition();
    float half = Constants::ROOM_SIZE / 2.0f;
    
    // Пол
    Floor floor({pos.x, 0.0f, pos.z}, Constants::ROOM_SIZE, Constants::ROOM_SIZE, GRAY);
    floor.Draw();
    
    // Потолок (прозрачный или тёмный)
    Floor ceiling({pos.x, Constants::WALL_HEIGHT, pos.z}, Constants::ROOM_SIZE, Constants::ROOM_SIZE, DARKGRAY);
    ceiling.Draw();
    
    Color wallColor = {80, 80, 100, 255};
    
    // Стены с учётом дверных проёмов
    // Север (+Z)
    if (!room->HasDoor(Direction::NORTH)) {
        Wall wall({pos.x, Constants::WALL_HEIGHT/2, pos.z + half}, 
                  {Constants::ROOM_SIZE, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        wall.Draw();
    } else {
        // Левая и правая части стены вокруг двери
        Wall leftWall({pos.x - Constants::ROOM_SIZE/4 - Constants::DOOR_WIDTH/4, Constants::WALL_HEIGHT/2, pos.z + half},
                      {Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        Wall rightWall({pos.x + Constants::ROOM_SIZE/4 + Constants::DOOR_WIDTH/4, Constants::WALL_HEIGHT/2, pos.z + half},
                       {Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        leftWall.Draw();
        rightWall.Draw();
        // Верхняя перемычка
        Wall top({pos.x, Constants::WALL_HEIGHT - 0.25f, pos.z + half},
                 {Constants::DOOR_WIDTH, 0.5f, Constants::WALL_THICKNESS}, wallColor);
        top.Draw();
    }
    
    // Юг (-Z)
    if (!room->HasDoor(Direction::SOUTH)) {
        Wall wall({pos.x, Constants::WALL_HEIGHT/2, pos.z - half},
                  {Constants::ROOM_SIZE, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        wall.Draw();
    } else {
        Wall leftWall({pos.x - Constants::ROOM_SIZE/4 - Constants::DOOR_WIDTH/4, Constants::WALL_HEIGHT/2, pos.z - half},
                      {Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        Wall rightWall({pos.x + Constants::ROOM_SIZE/4 + Constants::DOOR_WIDTH/4, Constants::WALL_HEIGHT/2, pos.z - half},
                       {Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        leftWall.Draw();
        rightWall.Draw();
        Wall top({pos.x, Constants::WALL_HEIGHT - 0.25f, pos.z - half},
                 {Constants::DOOR_WIDTH, 0.5f, Constants::WALL_THICKNESS}, wallColor);
        top.Draw();
    }
    
    // Восток (+X)
    if (!room->HasDoor(Direction::EAST)) {
        Wall wall({pos.x + half, Constants::WALL_HEIGHT/2, pos.z},
                  {Constants::WALL_THICKNESS, Constants::WALL_HEIGHT, Constants::ROOM_SIZE}, wallColor);
        wall.Draw();
    } else {
        Wall leftWall({pos.x + half, Constants::WALL_HEIGHT/2, pos.z - Constants::ROOM_SIZE/4 - Constants::DOOR_WIDTH/4},
                      {Constants::WALL_THICKNESS, Constants::WALL_HEIGHT, Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2}, wallColor);
        Wall rightWall({pos.x + half, Constants::WALL_HEIGHT/2, pos.z + Constants::ROOM_SIZE/4 + Constants::DOOR_WIDTH/4},
                       {Constants::WALL_THICKNESS, Constants::WALL_HEIGHT, Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2}, wallColor);
        leftWall.Draw();
        rightWall.Draw();
        Wall top({pos.x + half, Constants::WALL_HEIGHT - 0.25f, pos.z},
                 {Constants::WALL_THICKNESS, 0.5f, Constants::DOOR_WIDTH}, wallColor);
        top.Draw();
    }
    
    // Запад (-X)
    if (!room->HasDoor(Direction::WEST)) {
        Wall wall({pos.x - half, Constants::WALL_HEIGHT/2, pos.z},
                  {Constants::WALL_THICKNESS, Constants::WALL_HEIGHT, Constants::ROOM_SIZE}, wallColor);
        wall.Draw();
    } else {
        Wall leftWall({pos.x - half, Constants::WALL_HEIGHT/2, pos.z - Constants::ROOM_SIZE/4 - Constants::DOOR_WIDTH/4},
                      {Constants::WALL_THICKNESS, Constants::WALL_HEIGHT, Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2}, wallColor);
        Wall rightWall({pos.x - half, Constants::WALL_HEIGHT/2, pos.z + Constants::ROOM_SIZE/4 + Constants::DOOR_WIDTH/4},
                       {Constants::WALL_THICKNESS, Constants::WALL_HEIGHT, Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2}, wallColor);
        leftWall.Draw();
        rightWall.Draw();
        Wall top({pos.x - half, Constants::WALL_HEIGHT - 0.25f, pos.z},
                 {Constants::WALL_THICKNESS, 0.5f, Constants::DOOR_WIDTH}, wallColor);
        top.Draw();
    }
    
    // Старт и выход
    if (room == room->neighbors[0]/* не точное сравнение, нужно передавать start/exit отдельно */) {
        // Отрисовка маркеров вынесена в Game или в отдельный слой
        DrawSphere({pos.x, 0.5f, pos.z}, 0.5f, GREEN);
    }
}

void Renderer3D::DrawDoors(Room* room, const Camera3D& camera, Direction& outHoveredDoor) {
    Vector3 pos = room->GetWorldPosition();
    float half = Constants::ROOM_SIZE / 2.0f;
    outHoveredDoor = Direction::NONE;
    
    Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, camera);
    
    auto checkDoor = [&](Direction dir, Vector3 doorPos) {
        if (!room->HasDoor(dir)) return;
        Door door(doorPos, dir);
        BoundingBox box = door.GetBoundingBox();
        RayCollision collision = GetRayCollisionBox(ray, box);
        door.Draw();
        if (collision.hit) {
            outHoveredDoor = dir;
            door.DrawHighlight();
        }
    };
    
    checkDoor(Direction::NORTH, {pos.x, Constants::DOOR_HEIGHT/2, pos.z + half});
    checkDoor(Direction::SOUTH, {pos.x, Constants::DOOR_HEIGHT/2, pos.z - half});
    checkDoor(Direction::EAST,  {pos.x + half, Constants::DOOR_HEIGHT/2, pos.z});
    checkDoor(Direction::WEST,  {pos.x - half, Constants::DOOR_HEIGHT/2, pos.z});
}