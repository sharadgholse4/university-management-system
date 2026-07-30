#pragma once
#include "auth/repositories/i_user_repository.hpp"
#include <SQLiteCpp/SQLiteCpp.h>

namespace ums {

// WHY: This is the concrete implementation of IUserRepository using SQLite.
// It encapsulates all database-specific logic (SQL queries, parameter binding)
// so that the rest of the application remains database-agnostic.
class SQLiteUserRepository : public IUserRepository {
public:
    // WHY: We take a reference to an existing database connection rather than creating
    // one, allowing connection sharing and connection pooling across multiple repositories.
    explicit SQLiteUserRepository(SQLite::Database& db);
    
    // Creates users table if it does not exist
    void initialize();
    
    Result<User> findById(int id) override;
    Result<User> findByEmail(const std::string& email) override;
    Result<User> create(const std::string& email, const std::string& passwordHash, Role role) override;
    Result<std::vector<User>> findAll() override;
    Result<bool> existsByEmail(const std::string& email) override;

private:
    SQLite::Database& db_; // Reference to shared database connection
    
    // Helper to map a SQLite result row to a User domain object
    // WHY: Centralizes the row-to-object mapping logic to avoid duplication.
    User extractUser(SQLite::Statement& stmt);
};

} // namespace ums
