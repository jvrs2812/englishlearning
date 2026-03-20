#pragma once

#include "interfaces/IConfig.h"
#include <string>

namespace english_buddy {

/// JSON file-based configuration reader.
/// Reads config from a JSON file and provides typed accessors.
class JsonConfig : public IConfig {
public:
    explicit JsonConfig(const std::string& configPath);
    ~JsonConfig() override = default;

    std::string getTelegramToken() const override;
    std::string getOllamaUrl() const override;
    std::string getOllamaModel() const override;
    std::string getScheduleStart() const override;
    std::string getScheduleEnd() const override;
    int getScheduleIntervalMinutes() const override;
    std::string getDbPath() const override;
    std::vector<int64_t> getAllowedChatIds() const override;

private:
    std::string telegramToken_;
    std::string ollamaUrl_;
    std::string ollamaModel_;
    std::string scheduleStart_;
    std::string scheduleEnd_;
    int scheduleIntervalMinutes_;
    std::string dbPath_;
    std::vector<int64_t> allowedChatIds_;
};

} // namespace english_buddy
