#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace english_buddy {

/// Callback type for scheduler triggers: receives the list of chat IDs to notify.
using TriggerCallback = std::function<void(const std::vector<int64_t>& chatIds)>;

/// Interface for scheduled tasks (daily greeting, reminders, etc.)
/// Single Responsibility: Only handles time-based scheduling.
class IScheduler {
public:
    virtual ~IScheduler() = default;

    /// Start the scheduler loop.
    virtual void start() = 0;

    /// Stop the scheduler loop gracefully.
    virtual void stop() = 0;

    /// Register a callback to be invoked when a scheduled trigger fires.
    virtual void onTrigger(TriggerCallback callback) = 0;
};

} // namespace english_buddy
