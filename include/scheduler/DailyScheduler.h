#pragma once

#include "interfaces/IScheduler.h"
#include "interfaces/IConfig.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace english_buddy {

/// Interval-based scheduler that triggers every N minutes within a time window.
/// For example: every 30 minutes from 08:00 to 22:00.
class DailyScheduler : public IScheduler {
public:
    explicit DailyScheduler(std::shared_ptr<IConfig> config);
    ~DailyScheduler() override;

    void start() override;
    void stop() override;
    void onTrigger(TriggerCallback callback) override;

private:
    std::shared_ptr<IConfig> config_;
    TriggerCallback triggerCallback_;
    std::thread schedulerThread_;
    std::atomic<bool> running_{false};
    std::mutex mutex_;
    std::chrono::steady_clock::time_point lastTriggerTime_;

    void schedulerLoop();
    bool isWithinWindow() const;
    int parseTime(const std::string& timeStr) const;
};

} // namespace english_buddy
