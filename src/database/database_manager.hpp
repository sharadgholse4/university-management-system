#pragma once
#include <string>
#include <memory>
#include <SQLiteCpp/SQLiteCpp.h>

namespace ums {

// WHY: The DatabaseManager class acts as a single point of configuration for our SQLite connection.
// It abstracts away the low-level SQLite flags and encapsulates migration logic.
class DatabaseManager {
public:
    // path can be ":memory:" for testing
    explicit DatabaseManager(const std::string& dbPath);
    ~DatabaseManager() = default;
    
    // Non-copyable, movable to ensure unique ownership of the DB connection
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) = default;
    DatabaseManager& operator=(DatabaseManager&&) = default;

    SQLite::Database& db();
    const SQLite::Database& db() const;

    // Run all .sql files from a directory in alphabetical order
    void runMigrations(const std::string& migrationsDir);

    // Run a single SQL string (for testing)
    void executeSql(const std::string& sql);

private:
    std::unique_ptr<SQLite::Database> db_;
};

} // namespace ums
