#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <raylib.h>
#include <functional>
#include <unordered_map>

class InputHandler {
public:
    InputHandler();
    void Update();  // обновляет состояния клавиш и мыши
    bool IsKeyPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;
    Vector2 GetMouseDelta() const;
    bool IsKeyJustPressed(int key) const;
    bool IsMouseButtonJustPressed(int button) const;
    
    void RegisterKeyCallback(int key, std::function<void()> callback);
    void RegisterMouseCallback(int button, std::function<void()> callback);
    void ProcessCallbacks();
    
private:
    std::unordered_map<int, bool> keyState;
    std::unordered_map<int, bool> mouseState;
    Vector2 mouseDelta;
    std::unordered_map<int, std::function<void()>> keyCallbacks;
    std::unordered_map<int, std::function<void()>> mouseCallbacks;
};

#endif