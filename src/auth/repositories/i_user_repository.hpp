#pragma once
#include <vector>
#include <optional>
#include <string>
#include "common/types.hpp"
#include "auth/models/user.hpp"

namespace ums {

// WHY: Using a pure abstract interface for repositories allows for dependency injection.
// This means we can easily swap out the SQLite implementation for a mock implementation
// in unit tests, or switch to a different database (e.g. PostgreSQL) in the future
// without changing the service layer.
class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    
    // Find a user by their unique ID
    virtual Result<User> findById(int id) = 0;
    
    // Find a user by their unique email address
    virtual Result<User> findByEmail(const std::string& email) = 0;
    
    // Create a new user with the given details
    virtual Result<User> create(const std::string& email, const std::string& passwordHash, Role role) = 0;
    
    // Retrieve all users (useful for admin endpoints)
    virtual Result<std::vector<User>> findAll() = 0;
    
    // Check if a user with the given email already exists
    virtual Result<bool> existsByEmail(const std::string& email) = 0;
};

} // namespace ums
