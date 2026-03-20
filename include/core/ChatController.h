#pragma once

#include "interfaces/IMessenger.h"
#include "interfaces/ILLMService.h"
#include "interfaces/IConversationStore.h"
#include "interfaces/IScheduler.h"
#include "interfaces/IConfig.h"
#include "core/PromptBuilder.h"
#include <memory>

namespace english_buddy {

/// Main orchestrator — wires all components together.
/// Depends only on abstractions (interfaces), never on concrete implementations.
class ChatController {
public:
    ChatController(
        std::shared_ptr<IMessenger> messenger,
        std::shared_ptr<ILLMService> llmService,
        std::shared_ptr<IConversationStore> store,
        std::shared_ptr<IScheduler> scheduler,
        std::shared_ptr<IConfig> config
    );

    ~ChatController() = default;

    /// Initialize callbacks and start all services.
    void start();

    /// Stop all services gracefully.
    void stop();

private:
    std::shared_ptr<IMessenger> messenger_;
    std::shared_ptr<ILLMService> llmService_;
    std::shared_ptr<IConversationStore> store_;
    std::shared_ptr<IScheduler> scheduler_;
    std::shared_ptr<IConfig> config_;
    PromptBuilder promptBuilder_;

    /// Handle an incoming user message.
    void handleMessage(int64_t chatId, const std::string& username, const std::string& text);

    /// Handle a scheduler trigger — send proactive greeting.
    void handleSchedulerTrigger(const std::vector<int64_t>& chatIds);

    /// Handle the /start command.
    void handleStartCommand(int64_t chatId, const std::string& username);

    /// Handle the /clear command.
    void handleClearCommand(int64_t chatId);
};

} // namespace english_buddy
