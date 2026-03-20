#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace english_buddy {

/// Interface for application configuration.
/// Single Responsibility: Only handles reading configuration values.
class IConfig {
public:
    virtual ~IConfig() = default;

    virtual std::string getTelegramToken() const = 0;
    virtual std::string getOllamaUrl() const = 0;
    virtual std::string getOllamaModel() const = 0;
    virtual std::string getScheduleStart() const = 0;
    virtual std::string getScheduleEnd() const = 0;
    virtual int getScheduleIntervalMinutes() const = 0;
    virtual std::string getDbPath() const = 0;
    virtual std::vector<int64_t> getAllowedChatIds() const = 0;
};

} // namespace english_buddy
