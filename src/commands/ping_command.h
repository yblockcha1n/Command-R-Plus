#pragma once
#include <dpp/dpp.h>

class PingCommand {
public:
    static void execute(const dpp::slashcommand_t& event);
};