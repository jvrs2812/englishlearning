#pragma once

#include "interfaces/IConversationStore.h"
#include <sqlite3.h>
#include <string>
#include <mutex>

namespace english_buddy {

/// SQLite-based conversation persistence.
/// Stores messages in a local SQLite database file.
class SQLiteConversationStore : public IConversationStore {
public:
    explicit SQLiteConversationStore(const std::string& dbPath);
    ~SQLiteConversationStore() override;

    void saveMessage(int64_t chatId, const Message& msg) override;
    std::vector<Message> getHistory(int64_t chatId, int limit = 50) override;
    void clearHistory(int64_t chatId) override;

private:
    sqlite3* db_ = nullptr;
    std::mutex mutex_;

    void initDatabase();
    void executeSQL(const std::string& sql);
};

} // namespace english_buddy
