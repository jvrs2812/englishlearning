#include "scheduler/DailyScheduler.h"
#include <ctime>
#include <iostream>

namespace english_buddy {

DailyScheduler::DailyScheduler(std::shared_ptr<IConfig> config)
    : config_(std::move(config))
    , lastTriggerTime_(std::chrono::steady_clock::now() - std::chrono::hours(24))
{
    std::cout << "[Scheduler] Window: " << config_->getScheduleStart()
              << " - " << config_->getScheduleEnd()
              << " every " << config_->getScheduleIntervalMinutes() << " min"
              << std::endl;
}

DailyScheduler::~DailyScheduler() {
    stop();
}

void DailyScheduler::start() {
    if (running_.exchange(true)) return;

    std::cout << "[Scheduler] Starting..." << std::endl;
    schedulerThread_ = std::thread([this]() { schedulerLoop(); });
}

void DailyScheduler::stop() {
    if (!running_.exchange(false)) return;

    std::cout << "[Scheduler] Stopping..." << std::endl;
    if (schedulerThread_.joinable()) {
        schedulerThread_.join();
    }
}

void DailyScheduler::onTrigger(TriggerCallback callback) {
    triggerCallback_ = std::move(callback);
}

void DailyScheduler::schedulerLoop() {
    while (running_) {
        // Check every 30 seconds
        for (int i = 0; i < 30 && running_; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (!running_) break;

        // Check if we're within the time window
        if (!isWithinWindow()) continue;

        // Check if enough time has passed since last trigger
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(
            now - lastTriggerTime_
        ).count();

        int intervalMinutes = config_->getScheduleIntervalMinutes();

        if (elapsed >= intervalMinutes) {
            std::lock_guard<std::mutex> lock(mutex_);
            lastTriggerTime_ = now;

            std::cout << "[Scheduler] Triggering! (every " << intervalMinutes
                      << " min, " << elapsed << " min since last)" << std::endl;

            if (triggerCallback_) {
                auto chatIds = config_->getAllowedChatIds();
                if (!chatIds.empty()) {
                    triggerCallback_(chatIds);
                }
            }
        }
    }
}

bool DailyScheduler::isWithinWindow() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local{};
    localtime_r(&t, &local);

    int currentMinutes = local.tm_hour * 60 + local.tm_min;
    int startMinutes = parseTime(config_->getScheduleStart());
    int endMinutes = parseTime(config_->getScheduleEnd());

    return currentMinutes >= startMinutes && currentMinutes <= endMinutes;
}

int DailyScheduler::parseTime(const std::string& timeStr) const {
    int hours = 0, minutes = 0;
    auto colonPos = timeStr.find(':');
    if (colonPos != std::string::npos) {
        hours = std::stoi(timeStr.substr(0, colonPos));
        minutes = std::stoi(timeStr.substr(colonPos + 1));
    }
    return hours * 60 + minutes;
}

} // namespace english_buddy
