#include "Game.h"
#include <iostream>

int main() {
    try {
        Game game(2, 10);
        game.Run();
    } catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        std::cin.get();
        return 1;
    }
    return 0;
}