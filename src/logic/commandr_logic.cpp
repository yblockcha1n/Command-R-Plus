#include "commandr_logic.h"
#include "../utils/config_loader.h"
#include <curl/curl.h>
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    } catch(std::bad_alloc& e) {
        return 0;
    }
}

CommandRResponse CommandRLogic::sendMessage(const std::string& message, const std::string& systemPrompt, const std::string& conversationId, const std::vector<ChatMessage>& chatHistory) {
    std::cout << "Preparing to send message to CommandR+" << std::endl;
    json payload;

    payload["message"] = message;
    payload["connectors"] = json::array({json::object({{"id", "web-search"}})});

    if (!systemPrompt.empty()) {
        payload["preamble"] = systemPrompt;
    }

    if (!conversationId.empty()) {
        payload["conversation_id"] = conversationId;
    }

    json history_array = json::array();
    for (const auto& msg : chatHistory) {
        json chat_msg;
        chat_msg["role"] = msg.role;
        chat_msg["message"] = msg.message;
        if (!msg.tool_calls.empty()) chat_msg["tool_calls"] = msg.tool_calls;
        if (!msg.tool_results.empty()) chat_msg["tool_results"] = msg.tool_results;
        history_array.push_back(chat_msg);
    }
    payload["chat_history"] = history_array;

    std::cout << "Sending request to CommandR+" << std::endl;
    std::string response = makeRequest(payload.dump());
    std::cout << "Received response from CommandR+" << std::endl;

    return parseResponse(response);
}

std::string CommandRLogic::makeRequest(const std::string& payload) {
    CURL* curl = curl_easy_init();
    std::string readBuffer;

    if (curl) {
        const std::string url = "https://api.cohere.com/v1/chat";
        const std::string apiKey = ConfigLoader::getInstance().getCommandRApiKey();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    std::cout << "API Response: " << readBuffer << std::endl;
    return readBuffer;
}

CommandRResponse CommandRLogic::parseResponse(const std::string& response) {
    CommandRResponse result;
    try {
        json j = json::parse(response);

        result.text = j["text"];
        result.generation_id = j["generation_id"];
        
        if (j.contains("citations") && !j["citations"].is_null()) {
            result.citations = j["citations"];
        }
        if (j.contains("documents") && !j["documents"].is_null()) {
            result.documents = j["documents"];
        }
        if (j.contains("is_search_required") && !j["is_search_required"].is_null()) {
            result.is_search_required = j["is_search_required"];
        }
        if (j.contains("search_queries") && !j["search_queries"].is_null()) {
            result.search_queries = j["search_queries"];
        }
        if (j.contains("search_results") && !j["search_results"].is_null()) {
            result.search_results = j["search_results"];
        }
        if (j.contains("finish_reason") && !j["finish_reason"].is_null()) {
            result.finish_reason = j["finish_reason"];
        }
        if (j.contains("tool_calls") && !j["tool_calls"].is_null()) {
            result.tool_calls = j["tool_calls"];
        }
        if (j.contains("meta") && !j["meta"].is_null()) {
            result.meta = j["meta"];
        }
        if (j.contains("conversation_id") && !j["conversation_id"].is_null()) {
            result.conversation_id = j["conversation_id"];
        }

        if (j.contains("chat_history") && !j["chat_history"].is_null()) {
            for (const auto& msg : j["chat_history"]) {
                ChatMessage chatMsg;
                chatMsg.role = msg["role"];
                chatMsg.message = msg["message"];
                if (msg.contains("tool_calls") && !msg["tool_calls"].is_null()) {
                    chatMsg.tool_calls = msg["tool_calls"];
                }
                if (msg.contains("tool_results") && !msg["tool_results"].is_null()) {
                    chatMsg.tool_results = msg["tool_results"];
                }
                result.chat_history.push_back(chatMsg);
            }
        }
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        result.text = "Error parsing API response";
    } catch (json::exception& e) {
        std::cerr << "JSON error: " << e.what() << std::endl;
        result.text = "Error processing API response";
    }

    return result;
}