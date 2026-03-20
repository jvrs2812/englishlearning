#include "storage/SQLiteConversationStore.h"
#include <iostream>
#include <stdexcept>
#include <chrono>

namespace english_buddy {

SQLiteConversationStore::SQLiteConversationStore(const std::string& dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        throw std::runtime_error("[SQLiteStore] Cannot open database: " + err);
    }

    std::cout << "[SQLiteStore] Database opened: " << dbPath << std::endl;
    initDatabase();
}

SQLiteConversationStore::~SQLiteConversationStore() {
    if (db_) {
        sqlite3_close(db_);
        std::cout << "[SQLiteStore] Database closed." << std::endl;
    }
}

void SQLiteConversationStore::initDatabase() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS messages (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            chat_id     INTEGER NOT NULL,
            role        TEXT    NOT NULL,
            content     TEXT    NOT NULL,
            timestamp   INTEGER NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_messages_chat_id ON messages(chat_id);
        CREATE INDEX IF NOT EXISTS idx_messages_timestamp ON messages(chat_id, timestamp);
    )";
    executeSQL(sql);
    std::cout << "[SQLiteStore] Database initialized." << std::endl;
}

void SQLiteConversationStore::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string err(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error("[SQLiteStore] SQL error: " + err);
    }
}

void SQLiteConversationStore::saveMessage(int64_t chatId, const Message& msg) {
    std::lock_guard<std::mutex> lock(mutex_);

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO messages (chat_id, role, content, timestamp) VALUES (?, ?, ?, ?)";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("[SQLiteStore] Prepare failed: " +
                                 std::string(sqlite3_errmsg(db_)));
    }

    int64_t ts = msg.timestamp;
    if (ts == 0) {
        ts = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    sqlite3_bind_int64(stmt, 1, chatId);
    sqlite3_bind_text(stmt, 2, msg.role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, msg.content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, ts);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw std::runtime_error("[SQLiteStore] Insert failed: " +
                                 std::string(sqlite3_errmsg(db_)));
    }
}

std::vector<Message> SQLiteConversationStore::getHistory(int64_t chatId, int limit) {
    std::lock_guard<std::mutex> lock(mutex_);

    sqlite3_stmt* stmt = nullptr;
    // Get the last N messages ordered by timestamp ascending
    const char* sql = R"(
        SELECT role, content, timestamp FROM (
            SELECT role, content, timestamp
            FROM messages
            WHERE chat_id = ?
            ORDER BY timestamp DESC, id DESC
            LIMIT ?
        ) sub ORDER BY timestamp ASC, rowid ASC
    )";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("[SQLiteStore] Prepare failed: " +
                                 std::string(sqlite3_errmsg(db_)));
    }

    sqlite3_bind_int64(stmt, 1, chatId);
    sqlite3_bind_int(stmt, 2, limit);

    std::vector<Message> messages;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Message msg;
        msg.role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        msg.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        msg.timestamp = sqlite3_column_int64(stmt, 2);
        messages.push_back(std::move(msg));
    }

    sqlite3_finalize(stmt);
    return messages;
}

void SQLiteConversationStore::clearHistory(int64_t chatId) {
    std::lock_guard<std::mutex> lock(mutex_);

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM messages WHERE chat_id = ?";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("[SQLiteStore] Prepare failed: " +
                                 std::string(sqlite3_errmsg(db_)));
    }

    sqlite3_bind_int64(stmt, 1, chatId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    std::cout << "[SQLiteStore] Cleared history for chat " << chatId << std::endl;
}

} // namespace english_buddy
