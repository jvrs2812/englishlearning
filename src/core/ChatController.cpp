#include "core/ChatController.h"
#include <chrono>
#include <iostream>

namespace english_buddy {

ChatController::ChatController(
    std::shared_ptr<IMessenger> messenger,
    std::shared_ptr<ILLMService> llmService,
    std::shared_ptr<IConversationStore> store,
    std::shared_ptr<IScheduler> scheduler,
    std::shared_ptr<IConfig> config
)
    : messenger_(std::move(messenger))
    , llmService_(std::move(llmService))
    , store_(std::move(store))
    , scheduler_(std::move(scheduler))
    , config_(std::move(config))
{
}

void ChatController::start() {
    std::cout << "[ChatController] Initializing..." << std::endl;

    // Register message handler
    messenger_->onMessageReceived(
        [this](int64_t chatId, const std::string& username, const std::string& text) {
            handleMessage(chatId, username, text);
        }
    );

    // Register scheduler trigger
    scheduler_->onTrigger(
        [this](const std::vector<int64_t>& chatIds) {
            handleSchedulerTrigger(chatIds);
        }
    );

    // Start services
    scheduler_->start();
    messenger_->start();

    std::cout << "[ChatController] Bot is running!" << std::endl;
}

void ChatController::stop() {
    std::cout << "[ChatController] Shutting down..." << std::endl;
    messenger_->stop();
    scheduler_->stop();
}

void ChatController::handleMessage(int64_t chatId, const std::string& username, const std::string& text) {
    std::cout << "[ChatController] Message from " << username
              << " (chat " << chatId << "): " << text << std::endl;

    // Handle commands
    if (text == "/start") {
        handleStartCommand(chatId, username);
        return;
    }

    if (text == "/clear") {
        handleClearCommand(chatId);
        return;
    }

    try {
        // 1. Save the user's message
        Message userMsg("user", text);
        store_->saveMessage(chatId, userMsg);

        // 2. Get conversation history
        auto history = store_->getHistory(chatId, 15);

        // 3. Build the full conversation with system prompt
        auto conversation = promptBuilder_.buildConversation(history);

        // 4. Send to LLM
        std::string response = llmService_->chat(conversation);

        // 5. Save assistant response
        Message assistantMsg("assistant", response);
        store_->saveMessage(chatId, assistantMsg);

        // 6. Send response back to user
        messenger_->sendMessage(chatId, response);

    } catch (const std::exception& e) {
        std::cerr << "[ChatController] Error: " << e.what() << std::endl;
        messenger_->sendMessage(chatId,
            "⚠️ Oops! Something went wrong on my end. Give me a sec and try again! 🙏");
    }
}

void ChatController::handleSchedulerTrigger(const std::vector<int64_t>& chatIds) {
    std::cout << "[ChatController] Scheduler triggered for "
              << chatIds.size() << " chats" << std::endl;

    for (int64_t chatId : chatIds) {
        try {
            // Get recent history for context
            auto history = store_->getHistory(chatId, 20);

            // Build greeting conversation
            auto conversation = promptBuilder_.buildGreetingConversation(history);

            // Generate greeting via LLM
            std::string greeting = llmService_->chat(conversation);

            // Save the greeting as an assistant message
            Message greetingMsg("assistant", greeting);
            store_->saveMessage(chatId, greetingMsg);

            // Send to user
            messenger_->sendMessage(chatId, greeting);

            std::cout << "[ChatController] Sent daily greeting to chat " << chatId << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[ChatController] Failed to send greeting to chat "
                      << chatId << ": " << e.what() << std::endl;
        }
    }
}

void ChatController::handleStartCommand(int64_t chatId, const std::string& username) {
    std::string welcome =
        "Hey there, " + username + "! 👋\n\n"
        "I'm Jake, your English practice buddy! 🇺🇸\n\n"
        "Just text me anything in English and I'll:\n"
        "✅ Tell you what you got right\n"
        "❌ Help fix any mistakes\n"
        "💡 Suggest more natural ways to say things\n\n"
        "Then we'll just keep chatting like friends! 😄\n\n"
        "Don't worry about making mistakes — that's how we learn!\n\n"
        "So... how's your day going? 🌟";

    messenger_->sendMessage(chatId, welcome);

    // Save as assistant message for context
    Message welcomeMsg("assistant", welcome);
    store_->saveMessage(chatId, welcomeMsg);
}

void ChatController::handleClearCommand(int64_t chatId) {
    store_->clearHistory(chatId);
    messenger_->sendMessage(chatId,
        "🧹 Conversation cleared! Let's start fresh.\n\nSo, what's up? 😊");
}

} // namespace english_buddy
