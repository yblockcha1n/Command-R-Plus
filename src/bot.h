#pragma once
#include <dpp/dpp.h>

class Bot {
public:
    Bot();
    void run();

private:
    dpp::cluster bot;
    void setupCommands();
    void setupEventHandlers();
};