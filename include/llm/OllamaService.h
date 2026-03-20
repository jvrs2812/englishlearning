#pragma once

#include "interfaces/ILLMService.h"
#include <string>

namespace english_buddy {

/// Ollama LLM service implementation.
/// Communicates with Ollama via its REST API (/api/chat).
class OllamaService : public ILLMService {
public:
    OllamaService(const std::string& baseUrl, const std::string& model);
    ~OllamaService() override = default;

    std::string chat(const std::vector<Message>& conversation) override;

private:
    std::string host_;
    int port_;
    std::string model_;

    void parseUrl(const std::string& baseUrl);
};

} // namespace english_buddy
