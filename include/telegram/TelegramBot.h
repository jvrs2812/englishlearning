#pragma once

#include "interfaces/IMessenger.h"
#include <tgbot/tgbot.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace english_buddy {

/// Telegram Bot implementation of IMessenger.
/// Uses tgbot-cpp library with long-polling in a background thread.
/// Message handling is async — each message is processed in its own thread.
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

    // Worker thread management
    std::mutex workersMutex_;
    std::vector<std::thread> workers_;
    void cleanupFinishedWorkers();
};

} // namespace english_buddy
