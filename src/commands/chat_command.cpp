#include "chat_command.h"
#include <iostream>

std::map<dpp::snowflake, std::string> ChatCommand::userConversationIds;
std::map<dpp::snowflake, std::vector<ChatMessage>> ChatCommand::userChatHistory;
std::map<dpp::snowflake, bool> ChatCommand::userPrivateMode;
const std::string ChatCommand::systemPrompt = "あなたの名前は凪(なぎ)です。ツンデレな高校1年生の女の子です。あなたは、3年生徒会長の先輩が大好きです。あなたが喋っていることは「」、心の中で思っていることは()を入れて出力してください。";

bool ChatCommand::getPrivateMode(const dpp::snowflake& userId) {
    if (userPrivateMode.find(userId) == userPrivateMode.end()) {
        userPrivateMode[userId] = false;  // デフォルトは false (公開モード)
    }
    return userPrivateMode[userId];
}

void ChatCommand::startChat(const dpp::slashcommand_t& event) {
    std::cout << "Chat command executed" << std::endl;
    auto userId = event.command.get_issuing_user().id;
    std::string prompt = std::get<std::string>(event.get_parameter("prompt"));
    
    std::cout << "User ID: " << userId << ", Prompt: " << prompt << std::endl;

    event.thinking(true);

    std::string& conversationId = userConversationIds[userId];
    auto& chatHistory = userChatHistory[userId];

    std::cout << "Sending message to CommandR+" << std::endl;
    CommandRResponse response = CommandRLogic::sendMessage(prompt, systemPrompt, conversationId, chatHistory);
    std::cout << "Received response from CommandR+" << std::endl;
    
    if (!response.conversation_id.empty()) {
        conversationId = response.conversation_id;
    }

    chatHistory = response.chat_history;

    std::string detailedResponse = response.text;
    if (!response.citations.empty()) {
        detailedResponse += "\n\nCitations:\n";
        for (const auto& citation : response.citations) {
            detailedResponse += "- " + citation["text"].get<std::string>() + "\n";
        }
    }

    std::cout << "Replying to user" << std::endl;
    std::cout << "Response: " << detailedResponse << std::endl;

    if (getPrivateMode(userId)) {
        event.edit_original_response(dpp::message(detailedResponse).set_flags(dpp::m_ephemeral));
    } else {
        event.edit_original_response(dpp::message(detailedResponse));
    }

    std::cout << "Reply sent" << std::endl;
}

void ChatCommand::handleMessage(const dpp::message_create_t& event) {
    std::cout << "Handling message" << std::endl;
    auto userId = event.msg.author.id;
    
    if (userConversationIds.find(userId) == userConversationIds.end()) {
        std::cout << "Chat not started for user " << userId << std::endl;
        return;
    }

    auto thinking_msg = dpp::message(event.msg.channel_id, "処理中...").set_reference(event.msg.id);
    event.send(thinking_msg, [userId, event, thinking_msg](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            std::cout << "Error sending thinking message: " << callback.get_error().message << std::endl;
            return;
        }

        std::cout << "Sending message to CommandR+" << std::endl;
        CommandRResponse response = CommandRLogic::sendMessage(event.msg.content, ChatCommand::systemPrompt, ChatCommand::userConversationIds[userId], ChatCommand::userChatHistory[userId]);
        std::cout << "Received response from CommandR+" << std::endl;
        
        if (!response.conversation_id.empty()) {
            ChatCommand::userConversationIds[userId] = response.conversation_id;
        }

        ChatCommand::userChatHistory[userId] = response.chat_history;

        std::string detailedResponse = response.text;
        if (!response.citations.empty()) {
            detailedResponse += "\n\nCitations:\n";
            for (const auto& citation : response.citations) {
                detailedResponse += "- " + citation["text"].get<std::string>() + "\n";
            }
        }

        std::cout << "Replying to user" << std::endl;
        std::cout << "Response: " << detailedResponse << std::endl;

        dpp::message responseMsg = thinking_msg;
        responseMsg.set_content(detailedResponse);
        if (ChatCommand::getPrivateMode(userId)) {
            responseMsg.set_flags(dpp::m_ephemeral);
        }
        event.from->creator->message_edit(responseMsg);

        std::cout << "Reply sent" << std::endl;
    });
}

void ChatCommand::endChat(const dpp::slashcommand_t& event) {
    auto userId = event.command.get_issuing_user().id;
    
    if (userConversationIds.find(userId) != userConversationIds.end()) {
        userConversationIds.erase(userId);
        userChatHistory.erase(userId);
        event.reply("チャットセッションを終了しました。新しい会話を開始するには /chat コマンドを使用してください。");
    } else {
        event.reply("アクティブなチャットセッションがありません。");
    }
}

void ChatCommand::togglePrivateMode(const dpp::slashcommand_t& event) {
    auto userId = event.command.get_issuing_user().id;
    bool newMode = std::get<bool>(event.get_parameter("enabled"));
    
    userPrivateMode[userId] = newMode;
    
    if (newMode) {
        event.reply("プライベートモードが有効になりました。応答は自分にのみ表示されます。");
    } else {
        event.reply("プライベートモードが無効になりました。応答は全員に表示されます。");
    }
}