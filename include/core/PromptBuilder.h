#pragma once

#include "models/Message.h"
#include <string>
#include <vector>

namespace english_buddy {

/// Builds the system prompt and conversation context for the LLM.
/// Encapsulates all prompt engineering logic.
class PromptBuilder {
public:
    PromptBuilder() = default;

    /// Get the system prompt that defines Jake's personality and behavior.
    Message getSystemPrompt() const;

    /// Build a greeting message for proactive daily conversation starters.
    Message getGreetingPrompt() const;

    /// Assemble the full conversation to send to the LLM:
    /// [system prompt] + [conversation history]
    std::vector<Message> buildConversation(const std::vector<Message>& history) const;

    /// Assemble a conversation for a proactive greeting:
    /// [system prompt] + [history] + [greeting instruction]
    std::vector<Message> buildGreetingConversation(const std::vector<Message>& history) const;
};

} // namespace english_buddy
