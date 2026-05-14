#include "InputHandler.h"

InputHandler::InputHandler() : mouseDelta{0,0} {}

void InputHandler::Update() {
    mouseDelta = GetMouseDelta();
    // Обновление состояний клавиш можно не хранить, используем raylib напрямую в методах
}

bool InputHandler::IsKeyPressed(int key) const {
    return IsKeyDown(key);
}

bool InputHandler::IsMouseButtonPressed(int button) const {
    return IsMouseButtonDown(button);
}

Vector2 InputHandler::GetMouseDelta() const {
    return mouseDelta;
}

bool InputHandler::IsKeyJustPressed(int key) const {
    return IsKeyPressed(key);
}

bool InputHandler::IsMouseButtonJustPressed(int button) const {
    return IsMouseButtonPressed(button);
}

void InputHandler::RegisterKeyCallback(int key, std::function<void()> callback) {
    keyCallbacks[key] = callback;
}

void InputHandler::RegisterMouseCallback(int button, std::function<void()> callback) {
    mouseCallbacks[button] = callback;
}

void InputHandler::ProcessCallbacks() {
    for (auto& pair : keyCallbacks) {
        if (IsKeyJustPressed(pair.first)) {
            pair.second();
        }
    }
    for (auto& pair : mouseCallbacks) {
        if (IsMouseButtonJustPressed(pair.first)) {
            pair.second();
        }
    }
}