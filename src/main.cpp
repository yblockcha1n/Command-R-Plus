#include "bot.h"
#include <iostream>

int main() {
    try {
        std::cout << "Starting bot..." << std::endl;
        Bot bot;
        std::cout << "Bot instance created." << std::endl;
        std::cout << "Running bot..." << std::endl;
        bot.run();
        std::cout << "Bot finished running." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
    return 0;
}