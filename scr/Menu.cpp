#include "Menu.h"

Menu::Menu() : m_startRequested(false) {}

void Menu::Update() {
    if (IsKeyPressed(KEY_SPACE)) {
        m_startRequested = true;
    }
}

bool Menu::ShouldStart() const {
    return m_startRequested;
}

void Menu::Draw() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    const char* title = "WELCOME TO THE DUNGEON";
    const char* prompt = "PRESS SPACE TO START";
    int fontSize = 50;
    int titleWidth = MeasureText(title, fontSize);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, GetScreenHeight()/2 - 60, fontSize, RAYWHITE);
    DrawText(prompt, GetScreenWidth()/2 - MeasureText(prompt, 25)/2, GetScreenHeight()/2 + 30, 25, GREEN);
}