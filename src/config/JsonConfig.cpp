#include "config/JsonConfig.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

namespace english_buddy {

JsonConfig::JsonConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("[Config] Cannot open config file: " + configPath);
    }

    json config;
    try {
        file >> config;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("[Config] Invalid JSON in config file: " +
                                 std::string(e.what()));
    }

    telegramToken_ = config.value("telegram_token", "");
    ollamaUrl_     = config.value("ollama_url", "http://ollama:11434");
    ollamaModel_   = config.value("ollama_model", "llama3");
    scheduleStart_ = config.value("schedule_start", "08:00");
    scheduleEnd_   = config.value("schedule_end", "22:00");
    scheduleIntervalMinutes_ = config.value("schedule_interval_minutes", 30);
    dbPath_        = config.value("db_path", "/app/data/conversations.db");

    if (config.contains("allowed_chat_ids") && config["allowed_chat_ids"].is_array()) {
        for (const auto& id : config["allowed_chat_ids"]) {
            allowedChatIds_.push_back(id.get<int64_t>());
        }
    }

    if (telegramToken_.empty() || telegramToken_ == "YOUR_BOT_TOKEN_HERE") {
        throw std::runtime_error("[Config] telegram_token is not configured! "
                                 "Please set it in " + configPath);
    }

    std::cout << "[Config] Loaded configuration from: " << configPath << std::endl;
    std::cout << "[Config] Ollama: " << ollamaUrl_ << " model=" << ollamaModel_ << std::endl;
    std::cout << "[Config] Schedule: " << scheduleStart_ << " - " << scheduleEnd_
              << " every " << scheduleIntervalMinutes_ << " min" << std::endl;
    std::cout << "[Config] Allowed chats: " << allowedChatIds_.size() << std::endl;
}

std::string JsonConfig::getTelegramToken() const   { return telegramToken_; }
std::string JsonConfig::getOllamaUrl() const        { return ollamaUrl_; }
std::string JsonConfig::getOllamaModel() const      { return ollamaModel_; }
std::string JsonConfig::getScheduleStart() const    { return scheduleStart_; }
std::string JsonConfig::getScheduleEnd() const      { return scheduleEnd_; }
int JsonConfig::getScheduleIntervalMinutes() const { return scheduleIntervalMinutes_; }
std::string JsonConfig::getDbPath() const           { return dbPath_; }
std::vector<int64_t> JsonConfig::getAllowedChatIds() const { return allowedChatIds_; }

} // namespace english_buddy
