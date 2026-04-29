// main.cpp
#include "game_renderer.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    
    int width = 5;
    int height = 5;
    
    if (argc >= 3) {
        width = std::atoi(argv[1]);
        height = std::atoi(argv[2]);
        if (width < 2) width = 2;
        if (height < 2) height = 2;
    }
    
    std::cout << "Creating maze of size: " << width << "x" << height << "\n";
    
    GameRenderer game(width, height);
    game.Run();
    
    return 0;
}