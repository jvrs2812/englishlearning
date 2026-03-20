#include "config/JsonConfig.h"
#include "telegram/TelegramBot.h"
#include "llm/OllamaService.h"
#include "storage/SQLiteConversationStore.h"
#include "scheduler/DailyScheduler.h"
#include "core/ChatController.h"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <atomic>
#include <thread>

static std::atomic<bool> g_running{true};

void signalHandler(int signum) {
    std::cout << "\n[Main] Received signal " << signum << ", shutting down..." << std::endl;
    g_running = false;
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  English Buddy Bot v1.0" << std::endl;
    std::cout << "  Your friendly English practice pal!" << std::endl;
    std::cout << "========================================" << std::endl;

    // Determine config path
    std::string configPath = "/app/data/config.json";

    // Check for environment variable override
    const char* envConfig = std::getenv("CONFIG_PATH");
    if (envConfig) {
        configPath = envConfig;
    }

    // Check for command-line argument override
    if (argc > 1) {
        configPath = argv[1];
    }

    try {
        // =============================================
        // Dependency Injection — Manual Composition Root
        // =============================================

        // 1. Configuration (no dependencies)
        auto config = std::make_shared<english_buddy::JsonConfig>(configPath);

        // 2. Conversation Store (depends on config for db path)
        auto store = std::make_shared<english_buddy::SQLiteConversationStore>(
            config->getDbPath()
        );

        // 3. Messenger (depends on config for token)
        auto messenger = std::make_shared<english_buddy::TelegramBot>(
            config->getTelegramToken()
        );

        // 4. LLM Service (depends on config for URL and model)
        auto llmService = std::make_shared<english_buddy::OllamaService>(
            config->getOllamaUrl(),
            config->getOllamaModel()
        );

        // 5. Scheduler (depends on config for time window)
        auto scheduler = std::make_shared<english_buddy::DailyScheduler>(config);

        // 6. Controller (depends on all abstractions)
        english_buddy::ChatController controller(
            messenger, llmService, store, scheduler, config
        );

        // Setup signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);

        // Start the bot
        controller.start();

        // Keep main thread alive until signal
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Graceful shutdown
        controller.stop();

        std::cout << "[Main] Goodbye! 👋" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Main] Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
