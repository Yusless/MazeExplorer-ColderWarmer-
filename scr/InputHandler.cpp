#include "InputHandler.h"
#include <iostream>

InputHandler::InputHandler() : mouseDelta{0, 0} {}

void InputHandler::Update() {
    // Сохраняем предыдущие состояния
    previousKeys = currentKeys;
    previousMouse = currentMouse;
    
    // Получаем дельту мыши от raylib
    mouseDelta = GetMouseDelta();
    
    // Отладка: если мышь двигается, выводим
    if (mouseDelta.x != 0 || mouseDelta.y != 0) {
        std::cout << "InputHandler: raw mouse delta = " << mouseDelta.x << ", " << mouseDelta.y << std::endl;
    }
    
    // Обновляем состояния клавиш
    static const int keysToCheck[] = {KEY_M, KEY_F1, KEY_P};
    for (int key : keysToCheck) {
        currentKeys[key] = IsKeyDown(key);
    }
    
    // Обновляем состояния мыши
    currentMouse[MOUSE_LEFT_BUTTON] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
}

bool InputHandler::IsKeyPressed(int key) const {
    auto it = currentKeys.find(key);
    return it != currentKeys.end() && it->second;
}

bool InputHandler::IsKeyJustPressed(int key) const {
    auto curr = currentKeys.find(key);
    auto prev = previousKeys.find(key);
    return (curr != currentKeys.end() && curr->second) &&
           (prev == previousKeys.end() || !prev->second);
}

bool InputHandler::IsMouseButtonPressed(int button) const {
    auto it = currentMouse.find(button);
    return it != currentMouse.end() && it->second;
}

bool InputHandler::IsMouseButtonJustPressed(int button) const {
    auto curr = currentMouse.find(button);
    auto prev = previousMouse.find(button);
    return (curr != currentMouse.end() && curr->second) &&
           (prev == previousMouse.end() || !prev->second);
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