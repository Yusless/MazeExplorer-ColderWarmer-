#include "raylib.h"

int main() {
    InitWindow(800, 600, "Тест русского текста");
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(u8"Привет, мир! Русский текст работает!", 100, 200, 30, WHITE);
        DrawText(u8"МИНИКАРТА", 100, 300, 30, GREEN);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}