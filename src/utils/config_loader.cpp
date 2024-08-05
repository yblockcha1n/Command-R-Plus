#include "config_loader.h"
#include <iostream>
#include <filesystem>

ConfigLoader& ConfigLoader::getInstance() {
    static ConfigLoader instance;
    return instance;
}

ConfigLoader::ConfigLoader() {
    std::vector<std::filesystem::path> possiblePaths = {
        "config.yaml",
        "../config.yaml",
        "../../config.yaml"
    };

    for (const auto& path : possiblePaths) {
        std::cout << "Trying to load config from: " << std::filesystem::absolute(path) << std::endl;
        if (std::filesystem::exists(path)) {
            try {
                config = YAML::LoadFile(path.string());
                std::cout << "Config loaded successfully from: " << std::filesystem::absolute(path) << std::endl;
                return;
            } catch (const YAML::Exception& e) {
                std::cerr << "Error loading config file from " << path << ": " << e.what() << std::endl;
            }
        } else {
            std::cout << "Config file not found at: " << std::filesystem::absolute(path) << std::endl;
        }
    }

    std::cerr << "Error: Could not find or load config.yaml" << std::endl;
    exit(1);
}

std::string ConfigLoader::getBotToken() const {
    return config["bot_token"].as<std::string>();
}

std::string ConfigLoader::getCommandRApiKey() const {
    return config["commandr_api_key"].as<std::string>();
}