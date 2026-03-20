#include "llm/OllamaService.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

namespace english_buddy {

OllamaService::OllamaService(const std::string& baseUrl, const std::string& model)
    : model_(model)
{
    parseUrl(baseUrl);
    std::cout << "[OllamaService] Configured: " << host_ << ":" << port_
              << " model=" << model_ << std::endl;
}

void OllamaService::parseUrl(const std::string& baseUrl) {
    // Parse "http://host:port" format
    std::string url = baseUrl;

    // Remove trailing slash
    if (!url.empty() && url.back() == '/') {
        url.pop_back();
    }

    // Remove protocol prefix
    std::string hostPort = url;
    auto protoEnd = url.find("://");
    if (protoEnd != std::string::npos) {
        hostPort = url.substr(protoEnd + 3);
    }

    // Split host and port
    auto colonPos = hostPort.rfind(':');
    if (colonPos != std::string::npos) {
        host_ = hostPort.substr(0, colonPos);
        port_ = std::stoi(hostPort.substr(colonPos + 1));
    } else {
        host_ = hostPort;
        port_ = 11434; // Default Ollama port
    }
}

std::string OllamaService::chat(const std::vector<Message>& conversation) {
    httplib::Client cli(host_, port_);
    cli.set_read_timeout(120, 0);  // LLMs can take a while
    cli.set_write_timeout(30, 0);

    // Build the request body
    json requestBody;
    requestBody["model"] = model_;
    requestBody["stream"] = false;

    // Optimize for speed on CPU
    json options;
    options["num_predict"] = 300;    // Max tokens to generate (keeps response concise)
    options["temperature"] = 0.7;
    options["num_ctx"] = 2048;       // Smaller context window = faster on CPU
    requestBody["options"] = options;

    json messages = json::array();
    for (const auto& msg : conversation) {
        json m;
        m["role"] = msg.role;
        m["content"] = msg.content;
        messages.push_back(m);
    }
    requestBody["messages"] = messages;

    std::string body = requestBody.dump();

    std::cout << "[OllamaService] Sending chat request with "
              << conversation.size() << " messages..." << std::endl;

    auto result = cli.Post("/api/chat", body, "application/json");

    if (!result) {
        auto err = result.error();
        throw std::runtime_error("[OllamaService] HTTP request failed: " +
                                 httplib::to_string(err));
    }

    if (result->status != 200) {
        throw std::runtime_error("[OllamaService] HTTP " +
                                 std::to_string(result->status) + ": " + result->body);
    }

    try {
        json response = json::parse(result->body);
        std::string content = response["message"]["content"].get<std::string>();
        std::cout << "[OllamaService] Got response (" << content.size() << " chars)" << std::endl;
        return content;
    } catch (const json::exception& e) {
        throw std::runtime_error("[OllamaService] Failed to parse response: " +
                                 std::string(e.what()));
    }
}

} // namespace english_buddy
