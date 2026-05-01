// main.cpp
#include "GameRenderer.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    try {
    
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