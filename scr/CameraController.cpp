#include "CameraController.h"
#include "Constants.h"
#include <raymath.h>
#include <cmath>
#include <iostream>

CameraController::CameraController() : angle(0.0f), pitch(0.0f) {
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 90.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void CameraController::Update(Room* currentRoom) {
    Vector3 roomCenter = currentRoom->GetWorldPosition();
    camera.position = {roomCenter.x, Constants::CAMERA_HEIGHT, roomCenter.z};
    Vector3 forward = GetForward();
    camera.target = Vector3Add(camera.position, forward);
}

void CameraController::HandleMouseInput(Vector2 mouseDelta) {
    
    angle -= mouseDelta.x * Constants::MOUSE_SENSITIVITY;
    pitch -= mouseDelta.y * Constants::MOUSE_SENSITIVITY;
    
    const float TWO_PI = 2.0f * M_PI;
    while (angle < 0) angle += TWO_PI;
    while (angle >= TWO_PI) angle -= TWO_PI;
    
    if (pitch > Constants::CAMERA_PITCH_MAX) pitch = Constants::CAMERA_PITCH_MAX;
    if (pitch < Constants::CAMERA_PITCH_MIN) pitch = Constants::CAMERA_PITCH_MIN;
}

void CameraController::SetPosition(Vector3 position) {
    camera.position = position;
}

void CameraController::SetAngle(float newAngle) {
    angle = newAngle;
}

void CameraController::SetPitch(float newPitch) {
    pitch = newPitch;
}

Vector3 CameraController::GetForward() const {
    return {
        sinf(angle) * cosf(pitch),
        sinf(pitch),
        cosf(angle) * cosf(pitch)
    };
}

Vector2 CameraController::GetPlanarForwardXZ() const {
    float cosp = cosf(pitch);
    float fx = sinf(angle) * cosp;
    float fz = cosf(angle) * cosp;
    float len = sqrtf(fx * fx + fz * fz);
    if (len > 1e-6f) return {fx / len, fz / len};
    return {sinf(angle), cosf(angle)};
}