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
                // Try to split at a newline or space
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
        if (messageCallback_) {
            std::string username = message->from ?
                (message->from->firstName.empty() ? message->from->username : message->from->firstName)
                : "Unknown";
            messageCallback_(message->chat->id, username, message->text);
        }
    });
}

void TelegramBot::start() {
    if (running_.exchange(true)) return; // Already running

    std::cout << "[TelegramBot] Starting long-polling..." << std::endl;

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
}

} // namespace english_buddy
