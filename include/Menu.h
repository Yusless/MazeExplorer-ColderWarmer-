#ifndef MENU_H
#define MENU_H

#include <raylib.h>

class Menu {
public:
    Menu();
    void Update();
    void Draw();
    bool ShouldStart() const;

private:
    bool m_startRequested;
};

#endif