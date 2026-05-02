// main.cpp
#include "GameRenderer.h"
#include <iostream>
#include <cstdlib>

int main() {
    try {
    
    int width = 5;
    int height = 5;

    GameRenderer game(width, height);
    game.Run();
} catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        std::cin.get();
        return 1;
        
    } catch (...) {
        std::cerr << "Unknown exception:" << std::endl;
        std::cin.get();
        return 1;
    }
    
    return 0;
}