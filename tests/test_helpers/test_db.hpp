#pragma once
#include <memory>
#include <SQLiteCpp/SQLiteCpp.h>
#include "database/database_manager.hpp"

namespace ums::test {

// Creates a fresh in-memory database for each test
class TestDatabase {
public:
    TestDatabase() : dbManager_(":memory:") {
        // Run the users table creation SQL directly
        dbManager_.executeSql(
            "CREATE TABLE IF NOT EXISTS users ("
            "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  email TEXT NOT NULL UNIQUE,"
            "  password_hash TEXT NOT NULL,"
            "  role TEXT NOT NULL CHECK(role IN ('student', 'professor', 'admin')),"
            "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
            "  updated_at TEXT NOT NULL DEFAULT (datetime('now'))"
            ");"
        );
    }

    SQLite::Database& db() { return dbManager_.db(); }
    DatabaseManager& manager() { return dbManager_; }

private:
    DatabaseManager dbManager_;
};

} // namespace ums::test
