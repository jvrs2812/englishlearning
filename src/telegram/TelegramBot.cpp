#include "telegram/TelegramBot.h"
#include <iostream>

namespace english_buddy {

TelegramBot::TelegramBot(const std::string& token)
    : bot_(std::make_unique<TgBot::Bot>(token))
{
}

TelegramBot::~TelegramBot() {
    stop();
}

void TelegramBot::sendMessage(int64_t chatId, const std::string& text) {
    try {
        // Telegram has a 4096 character limit per message
        const size_t MAX_LEN = 4000;
        if (text.length() <= MAX_LEN) {
            bot_->getApi().sendMessage(chatId, text);
        } else {
            // Split long messages
            size_t pos = 0;
            while (pos < text.length()) {
                size_t end = std::min(pos + MAX_LEN, text.length());
                if (end < text.length()) {
                    size_t lastNewline = text.rfind('\n', end);
                    if (lastNewline != std::string::npos && lastNewline > pos) {
                        end = lastNewline + 1;
                    }
                }
                bot_->getApi().sendMessage(chatId, text.substr(pos, end - pos));
                pos = end;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[TelegramBot] Error sending message: " << e.what() << std::endl;
    }
}

void TelegramBot::onMessageReceived(MessageCallback callback) {
    messageCallback_ = std::move(callback);

    bot_->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        if (message->text.empty()) return;
        if (!messageCallback_) return;

        // Extract data before moving to thread (message ptr may not survive)
        int64_t chatId = message->chat->id;
        std::string username = message->from ?
            (message->from->firstName.empty() ? message->from->username : message->from->firstName)
            : "Unknown";
        std::string text = message->text;

        // Process message in a separate thread so polling isn't blocked
        std::lock_guard<std::mutex> lock(workersMutex_);
        cleanupFinishedWorkers();

        workers_.emplace_back([this, chatId, username = std::move(username), text = std::move(text)]() {
            try {
                messageCallback_(chatId, username, text);
            } catch (const std::exception& e) {
                std::cerr << "[TelegramBot] Worker error: " << e.what() << std::endl;
            }
        });
    });
}

void TelegramBot::cleanupFinishedWorkers() {
    // Remove finished threads (already locked by caller)
    workers_.erase(
        std::remove_if(workers_.begin(), workers_.end(),
            [](std::thread& t) {
                if (t.joinable()) {
                    // Try to join with zero timeout — if not done, keep it
                    // We detach finished threads instead
                    return false;
                }
                return true;
            }
        ),
        workers_.end()
    );
}

void TelegramBot::start() {
    if (running_.exchange(true)) return;

    std::cout << "[TelegramBot] Starting long-polling (async mode)..." << std::endl;

    pollingThread_ = std::thread([this]() {
        try {
            TgBot::TgLongPoll longPoll(*bot_);
            while (running_) {
                longPoll.start();
            }
        } catch (const std::exception& e) {
            std::cerr << "[TelegramBot] Polling error: " << e.what() << std::endl;
            running_ = false;
        }
    });
}

void TelegramBot::stop() {
    if (!running_.exchange(false)) return;

    std::cout << "[TelegramBot] Stopping..." << std::endl;

    if (pollingThread_.joinable()) {
        pollingThread_.join();
    }

    // Wait for all worker threads to finish
    std::lock_guard<std::mutex> lock(workersMutex_);
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers_.clear();

    std::cout << "[TelegramBot] Stopped." << std::endl;
}

} // namespace english_buddy
