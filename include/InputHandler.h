#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <raylib.h>
#include <functional>
#include <unordered_map>

class InputHandler {
public:
    InputHandler();
    void Update();  // Вызывайте каждый кадр

    bool IsKeyPressed(int key) const;
    bool IsKeyJustPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonJustPressed(int button) const;
    
    Vector2 GetMouseDelta() const { return mouseDelta; }  // inline для простоты

    void RegisterKeyCallback(int key, std::function<void()> callback);
    void RegisterMouseCallback(int button, std::function<void()> callback);
    void ProcessCallbacks();

private:
    std::unordered_map<int, bool> currentKeys;
    std::unordered_map<int, bool> previousKeys;
    std::unordered_map<int, bool> currentMouse;
    std::unordered_map<int, bool> previousMouse;
    Vector2 mouseDelta;

    std::unordered_map<int, std::function<void()>> keyCallbacks;
    std::unordered_map<int, std::function<void()>> mouseCallbacks;
};

#endif