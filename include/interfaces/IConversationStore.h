#pragma once

#include "models/Message.h"
#include <cstdint>
#include <vector>

namespace english_buddy {

/// Interface for conversation persistence.
/// Single Responsibility: Only handles storing and retrieving messages.
class IConversationStore {
public:
    virtual ~IConversationStore() = default;

    /// Persist a single message for a given chat.
    virtual void saveMessage(int64_t chatId, const Message& msg) = 0;

    /// Retrieve the last `limit` messages for a given chat, ordered by timestamp.
    virtual std::vector<Message> getHistory(int64_t chatId, int limit = 50) = 0;

    /// Clear all history for a given chat.
    virtual void clearHistory(int64_t chatId) = 0;
};

} // namespace english_buddy
