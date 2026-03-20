#pragma once

#include "models/Message.h"
#include <string>
#include <vector>

namespace english_buddy {

/// Interface for LLM services (Ollama, OpenAI, etc.)
/// Single Responsibility: Only handles LLM chat completions.
class ILLMService {
public:
    virtual ~ILLMService() = default;

    /// Send a conversation (list of messages) to the LLM and return the response text.
    virtual std::string chat(const std::vector<Message>& conversation) = 0;
};

} // namespace english_buddy
