#include "bot.h"
#include "commands/ping_command.h"
#include "commands/chat_command.h"
#include "utils/config_loader.h"

Bot::Bot() : bot(ConfigLoader::getInstance().getBotToken(), dpp::i_default_intents | dpp::i_message_content) {
    bot.on_log(dpp::utility::cout_logger());
}

void Bot::run() {
    setupCommands();
    setupEventHandlers();
    bot.start(dpp::st_wait);
}

void Bot::setupCommands() {
    bot.on_ready([this](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("ping", "Ping the bot", bot.me.id));
            
            dpp::slashcommand chatCommand("chat", "Start or continue a chat session with CommandR+", bot.me.id);
            chatCommand.add_option(dpp::command_option(dpp::co_string, "prompt", "Initial prompt for the chat", true));
            bot.global_command_create(chatCommand);

            bot.global_command_create(dpp::slashcommand("endchat", "End the current chat session", bot.me.id));

            dpp::slashcommand privateModeCommand("privatemode", "Toggle private mode for chat responses", bot.me.id);
            privateModeCommand.add_option(dpp::command_option(dpp::co_boolean, "enabled", "Enable or disable private mode", true));
            bot.global_command_create(privateModeCommand);
        }
    });
}

void Bot::setupEventHandlers() {
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            PingCommand::execute(event);
        } else if (event.command.get_command_name() == "chat") {
            ChatCommand::startChat(event);
        } else if (event.command.get_command_name() == "endchat") {
            ChatCommand::endChat(event);
        } else if (event.command.get_command_name() == "privatemode") {
            ChatCommand::togglePrivateMode(event);
        }
    });

    bot.on_message_create([](const dpp::message_create_t& event) {
        if (!event.msg.author.is_bot()) {
            ChatCommand::handleMessage(event);
        }
    });
}