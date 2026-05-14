#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <raylib.h>
#include "Room.h"

class CameraController {
public:
    CameraController();
    void Update(Room* currentRoom);
    void HandleMouseInput(Vector2 mouseDelta);
    void SetPosition(Vector3 position);
    void SetAngle(float angle);
    void SetPitch(float pitch);
    
    Camera3D GetCamera() const { return camera; }
    float GetAngle() const { return angle; }
    float GetPitch() const { return pitch; }
    Vector3 GetForward() const;
    /// Единичный вектор взгляда на плоскости XZ (учитывает pitch); для миникарты без привязки к знаку мыши.
    Vector2 GetPlanarForwardXZ() const;
    
private:
    Camera3D camera;
    float angle;
    float pitch;
};

#endif