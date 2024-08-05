#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct ChatMessage {
    std::string role;
    std::string message;
    json tool_calls;
    json tool_results;
};

struct CommandRResponse {
    std::string text;
    std::string generation_id;
    json citations;
    json documents;
    bool is_search_required;
    json search_queries;
    json search_results;
    std::string finish_reason;
    json tool_calls;
    std::vector<ChatMessage> chat_history;
    json meta;
    std::string conversation_id;
};

class CommandRLogic {
public:
    static CommandRResponse sendMessage(const std::string& message, const std::string& systemPrompt, const std::string& conversationId, const std::vector<ChatMessage>& chatHistory);

private:
    static std::string makeRequest(const std::string& payload);
    static CommandRResponse parseResponse(const std::string& response);
};