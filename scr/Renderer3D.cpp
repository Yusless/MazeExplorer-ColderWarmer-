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

    // Потолок
    Floor ceiling({pos.x, Constants::WALL_HEIGHT, pos.z}, - Constants::ROOM_SIZE, Constants::ROOM_SIZE, GRAY);
    ceiling.Draw();

    Color wallColor = {80, 80, 100, 255};

    // ========= СТЕНЫ =========
    // Север
    if (!room->HasDoor(Direction::NORTH)) {
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

    // Юг
    if (!room->HasDoor(Direction::SOUTH)) {
        Wall wall({pos.x, Constants::WALL_HEIGHT/2, pos.z + half},
                  {Constants::ROOM_SIZE, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        wall.Draw();
    } else {
        Wall leftWall({pos.x - Constants::ROOM_SIZE/4 - Constants::DOOR_WIDTH/4, Constants::WALL_HEIGHT/2, pos.z + half},
                      {Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        Wall rightWall({pos.x + Constants::ROOM_SIZE/4 + Constants::DOOR_WIDTH/4, Constants::WALL_HEIGHT/2, pos.z + half},
                       {Constants::ROOM_SIZE/2 - Constants::DOOR_WIDTH/2, Constants::WALL_HEIGHT, Constants::WALL_THICKNESS}, wallColor);
        leftWall.Draw();
        rightWall.Draw();
        Wall top({pos.x, Constants::WALL_HEIGHT - 0.25f, pos.z + half},
                 {Constants::DOOR_WIDTH, 0.5f, Constants::WALL_THICKNESS}, wallColor);
        top.Draw();
    }

    // Восток
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

    // Запад
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

    // ========= АВТОМАТ У СЕВЕРНОЙ СТЕНЫ =========
    if (room->HasSlotMachine()) {
        float machineWidth = 2.4f;
        float machineHeight = 1.9f;
        float machineDepth = 1.6f;
        // Прислоняем к северной стене (Z = pos.z - half + machineDepth/2)
        Vector3 machinePos = {pos.x, machineHeight/2.0f, pos.z - half + machineDepth/2.0f};
        DrawCube(machinePos, machineWidth, machineHeight, machineDepth, BROWN);
        DrawCubeWires(machinePos, machineWidth, machineHeight, machineDepth, DARKBROWN);
        // Панель спереди (с южной стороны автомата)
        Vector3 panelPos = {pos.x, machineHeight * 0.65f, machinePos.z + machineDepth/2 + 0.05f};
        DrawCube(panelPos, 1.6f, 1.0f, 0.1f, DARKGRAY);
        // Три окошка
        float startX = pos.x - 0.7f;
        float winY = machineHeight * 0.65f;
        float winZ = panelPos.z;
        for (int i = 0; i < 3; ++i) {
            DrawCube({startX + i * 0.8f, winY, winZ}, 0.4f, 0.6f, 0.05f, LIGHTGRAY);
            DrawCubeWires({startX + i * 0.8f, winY, winZ}, 0.4f, 0.6f, 0.05f, GOLD);
        }
        // Кнопка
        DrawSphere({pos.x + 1.0f, machineHeight * 0.4f, machinePos.z + machineDepth/2 + 0.1f}, 0.15f, RED);
    }
}

void Renderer3D::DrawDoors(Room* room, const Camera3D& camera, Direction& outHoveredDoor) {
    Vector3 pos = room->GetWorldPosition();
    float half = Constants::ROOM_SIZE / 2.0f;
    outHoveredDoor = Direction::NONE;

    Ray ray = GetMouseRay({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, camera);
    Direction hovered = PickClosestDoorAlongRay(room, ray);

    auto drawDoor = [&](Direction dir, Vector3 doorPos) {
        if (!room->HasDoor(dir)) return;
        Door door(doorPos, dir);
        door.Draw();
        if (dir == hovered) {
            outHoveredDoor = hovered;
            door.DrawHighlight();
        }
    };

    drawDoor(Direction::NORTH, {pos.x, Constants::DOOR_HEIGHT/2, pos.z - half});
    drawDoor(Direction::SOUTH, {pos.x, Constants::DOOR_HEIGHT/2, pos.z + half});
    drawDoor(Direction::EAST,  {pos.x + half, Constants::DOOR_HEIGHT/2, pos.z});
    drawDoor(Direction::WEST,  {pos.x - half, Constants::DOOR_HEIGHT/2, pos.z});
}