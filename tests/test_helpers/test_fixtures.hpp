#pragma once
#include "auth/models/user.hpp"

namespace ums::test {

// Create a valid test user (from DB format)
inline User makeTestUser(int id = 1, const std::string& email = "test@example.com",
                          Role role = Role::Student) {
    return User::fromDatabase(id, email, "$2b$12$hashedpassword", role, "2024-01-01 00:00:00", "2024-01-01 00:00:00");
}

inline User makeTestAdmin(int id = 99) {
    return User::fromDatabase(id, "admin@university.edu", "$2b$12$hashedpassword", Role::Admin, "2024-01-01 00:00:00", "2024-01-01 00:00:00");
}

inline User makeTestProfessor(int id = 50) {
    return User::fromDatabase(id, "prof@university.edu", "$2b$12$hashedpassword", Role::Professor, "2024-01-01 00:00:00", "2024-01-01 00:00:00");
}

} // namespace ums::test
