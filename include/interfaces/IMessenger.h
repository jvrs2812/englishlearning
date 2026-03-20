#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace english_buddy {

/// Callback type for received messages: (chatId, username, text)
using MessageCallback = std::function<void(int64_t chatId, const std::string& username, const std::string& text)>;

/// Interface for messaging platforms (Telegram, Discord, etc.)
/// Single Responsibility: Only handles sending/receiving messages.
class IMessenger {
public:
    virtual ~IMessenger() = default;

    /// Send a text message to a specific chat.
    virtual void sendMessage(int64_t chatId, const std::string& text) = 0;

    /// Register a callback to be invoked when a message is received.
    virtual void onMessageReceived(MessageCallback callback) = 0;

    /// Start listening for messages (blocking or in a background thread).
    virtual void start() = 0;

    /// Stop the messenger gracefully.
    virtual void stop() = 0;
};

} // namespace english_buddy
