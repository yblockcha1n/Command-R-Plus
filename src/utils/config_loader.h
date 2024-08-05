#pragma once
#include <string>
#include <yaml-cpp/yaml.h>

class ConfigLoader {
public:
    static ConfigLoader& getInstance();
    std::string getBotToken() const;
    std::string getCommandRApiKey() const;

private:
    ConfigLoader();
    YAML::Node config;
};