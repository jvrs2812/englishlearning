#pragma once

#include "interfaces/IMessenger.h"
#include <tgbot/tgbot.h>
#include <atomic>
#include <memory>
#include <string>
#include <thread>

namespace english_buddy {

/// Telegram Bot implementation of IMessenger.
/// Uses tgbot-cpp library with long-polling in a background thread.
class TelegramBot : public IMessenger {
public:
    explicit TelegramBot(const std::string& token);
    ~TelegramBot() override;

    void sendMessage(int64_t chatId, const std::string& text) override;
    void onMessageReceived(MessageCallback callback) override;
    void start() override;
    void stop() override;

private:
    std::unique_ptr<TgBot::Bot> bot_;
    MessageCallback messageCallback_;
    std::thread pollingThread_;
    std::atomic<bool> running_{false};
};

} // namespace english_buddy
