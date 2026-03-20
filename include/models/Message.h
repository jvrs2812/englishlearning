#pragma once

#include <string>
#include <cstdint>

namespace english_buddy {

/// Data model representing a single message in a conversation.
struct Message {
    std::string role;       // "system", "user", "assistant"
    std::string content;
    int64_t     timestamp;  // Unix timestamp

    Message() : timestamp(0) {}

    Message(std::string r, std::string c, int64_t ts = 0)
        : role(std::move(r))
        , content(std::move(c))
        , timestamp(ts) {}
};

} // namespace english_buddy
