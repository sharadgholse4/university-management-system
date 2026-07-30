#include "database/database_manager.hpp"
#include <filesystem>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ums {

DatabaseManager::DatabaseManager(const std::string& dbPath) {
    // WHY: We open the database in READWRITE and CREATE modes so that it creates the file if missing.
    db_ = std::make_unique<SQLite::Database>(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    
    // WHY: WAL (Write-Ahead Logging) mode significantly improves concurrency in SQLite, 
    // allowing simultaneous readers and a single writer, which is critical for web applications.
    db_->exec("PRAGMA journal_mode=WAL;");
    
    // WHY: SQLite disables foreign key constraints by default for backward compatibility.
    // We explicitly enable them to ensure referential integrity at the DB level.
    db_->exec("PRAGMA foreign_keys=ON;");
}

SQLite::Database& DatabaseManager::db() {
    return *db_;
}

const SQLite::Database& DatabaseManager::db() const {
    return *db_;
}

void DatabaseManager::runMigrations(const std::string& migrationsDir) {
    // WHY: std::filesystem allows cross-platform directory iteration.
    std::vector<std::string> migrationFiles;
    for (const auto& entry : std::filesystem::directory_iterator(migrationsDir)) {
        if (entry.path().extension() == ".sql") {
            migrationFiles.push_back(entry.path().string());
        }
    }

    // Sort alphabetically so that migrations like 001_..., 002_... run in the correct order.
    std::sort(migrationFiles.begin(), migrationFiles.end());

    for (const auto& file : migrationFiles) {
        std::ifstream ifs(file);
        if (!ifs.is_open()) {
            throw std::runtime_error("Failed to open migration file: " + file);
        }
        
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        
        // WHY: We execute each migration as a transaction so that if it fails partway through,
        // it rolls back and doesn't leave the DB in an inconsistent state.
        SQLite::Transaction transaction(*db_);
        db_->exec(buffer.str());
        transaction.commit();
    }
}

void DatabaseManager::executeSql(const std::string& sql) {
    // WHY: Simple wrapper to allow direct execution of raw SQL, primarily useful for setting up test data.
    db_->exec(sql);
}

} // namespace ums
