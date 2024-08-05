#pragma once
#include <dpp/dpp.h>
#include <map>
#include "../logic/commandr_logic.h"

class ChatCommand {
public:
    static void startChat(const dpp::slashcommand_t& event);
    static void handleMessage(const dpp::message_create_t& event);
    static void endChat(const dpp::slashcommand_t& event);
    static void togglePrivateMode(const dpp::slashcommand_t& event);
    static bool getPrivateMode(const dpp::snowflake& userId);

private:
    static std::map<dpp::snowflake, std::string> userConversationIds;
    static std::map<dpp::snowflake, std::vector<ChatMessage>> userChatHistory;
    static std::map<dpp::snowflake, bool> userPrivateMode;
    static const std::string systemPrompt;
};