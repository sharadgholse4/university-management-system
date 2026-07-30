#include "auth/repositories/sqlite_user_repository.hpp"
#include <iostream>

namespace ums {

SQLiteUserRepository::SQLiteUserRepository(SQLite::Database& db) : db_(db) {}

// WHY: Ensures the necessary table exists before we try to query it.
// In a production system, this might be handled by a dedicated migration tool,
// but having it here makes it self-contained for simpler setups.
void SQLiteUserRepository::initialize() {
    try {
        db_.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "email TEXT UNIQUE NOT NULL, "
            "password_hash TEXT NOT NULL, "
            "role TEXT NOT NULL, "
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
            "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ")"
        );
    } catch (const SQLite::Exception& e) {
        std::cerr << "Failed to initialize users table: " << e.what() << std::endl;
        throw; // Re-throw as this is a fatal initialization error
    }
}

// Helper to extract a User object from a statement row.
// WHY: Centralizes parsing logic. We use User::fromDatabase since we trust the DB data.
User SQLiteUserRepository::extractUser(SQLite::Statement& stmt) {
    int id = stmt.getColumn(0);
    std::string email = stmt.getColumn(1).getString();
    std::string passwordHash = stmt.getColumn(2).getString();
    std::string roleStr = stmt.getColumn(3).getString();
    std::string createdAt = stmt.getColumn(4).getString();
    std::string updatedAt = stmt.getColumn(5).getString();
    
    Role role = stringToRole(roleStr);
    return User::fromDatabase(id, email, passwordHash, role, createdAt, updatedAt);
}

Result<User> SQLiteUserRepository::findById(int id) {
    try {
        // WHY: Always use parameterized queries (bind) to prevent SQL injection attacks.
        SQLite::Statement stmt(db_, "SELECT id, email, password_hash, role, created_at, updated_at FROM users WHERE id = ?");
        stmt.bind(1, id);
        
        if (stmt.executeStep()) {
            return extractUser(stmt);
        } else {
            return AppError{ErrorCode::NotFound, "User not found"};
        }
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<User> SQLiteUserRepository::findByEmail(const std::string& email) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, email, password_hash, role, created_at, updated_at FROM users WHERE email = ?");
        stmt.bind(1, email);
        
        if (stmt.executeStep()) {
            return extractUser(stmt);
        } else {
            return AppError{ErrorCode::NotFound, "User not found"};
        }
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<User> SQLiteUserRepository::create(const std::string& email, const std::string& passwordHash, Role role) {
    try {
        // WHY: Using a transaction could be beneficial if there were multiple statements,
        // but for a single insert it's atomic anyway.
        SQLite::Statement stmt(db_, "INSERT INTO users (email, password_hash, role) VALUES (?, ?, ?)");
        stmt.bind(1, email);
        stmt.bind(2, passwordHash);
        stmt.bind(3, roleToString(role));
        
        stmt.exec();
        
        // Retrieve the generated ID
        long long lastId = db_.getLastInsertRowid();
        
        // Return the newly created user (by finding it to get all defaults like timestamps)
        return findById(static_cast<int>(lastId));
    } catch (const SQLite::Exception& e) {
        // Handle uniqueness constraint violations
        std::string errorMsg = e.what();
        if (errorMsg.find("UNIQUE constraint failed") != std::string::npos) {
            return AppError{ErrorCode::Conflict, "User with this email already exists"};
        }
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<User>> SQLiteUserRepository::findAll() {
    try {
        SQLite::Statement stmt(db_, "SELECT id, email, password_hash, role, created_at, updated_at FROM users");
        std::vector<User> users;
        
        while (stmt.executeStep()) {
            users.push_back(extractUser(stmt));
        }
        
        return users;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteUserRepository::existsByEmail(const std::string& email) {
    try {
        SQLite::Statement stmt(db_, "SELECT COUNT(*) FROM users WHERE email = ?");
        stmt.bind(1, email);
        
        if (stmt.executeStep()) {
            int count = stmt.getColumn(0);
            return count > 0;
        }
        return false;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

} // namespace ums
