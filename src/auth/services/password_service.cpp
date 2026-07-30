#include "auth/services/password_service.hpp"
// WHY: We use hilch/Bcrypt.cpp which provides a clean C++ wrapper around
// the OpenBSD bcrypt implementation. The header is at include/bcrypt.h
// in the fetched dependency, and uses namespace bcrypt::.
#include "bcrypt.h"
#include <iostream>

namespace ums {

PasswordService::PasswordService(int cost) : cost_(cost) {}

// -------------------------------------------------------------------------
// hash() — BCrypt password hashing
// -------------------------------------------------------------------------
// WHY BCrypt: Unlike SHA-256 or MD5, BCrypt:
//   1. Incorporates a random salt (prevents rainbow table attacks)
//   2. Has an adjustable cost factor (can be increased as hardware improves)
//   3. Uses constant-time comparison (prevents timing attacks)
//
// TIME COMPLEXITY: O(2^cost) — intentionally slow. Cost=12 takes ~250ms.
// SPACE COMPLEXITY: O(1) — fixed-size Blowfish state (4KB key schedule)
//
// SECURITY: The cost factor is configurable via environment variable.
// In tests we use cost=4 for speed; production should use cost=12+.
std::string PasswordService::hash(const std::string& password) const {
    try {
        return bcrypt::generateHash(password, cost_);
    } catch (const std::exception& e) {
        std::cerr << "Password hashing failed: " << e.what() << std::endl;
        throw; // Re-throw — this is a critical failure
    }
}

// -------------------------------------------------------------------------
// verify() — BCrypt password verification
// -------------------------------------------------------------------------
// WHY: We never decrypt the hash (BCrypt is a one-way function).
// Instead, we hash the input with the same salt (extracted from the stored
// hash) and compare the results. The library handles salt extraction internally.
//
// SECURITY: BCrypt comparison is constant-time, preventing timing attacks
// where an attacker measures response time to guess password characters.
bool PasswordService::verify(const std::string& password, const std::string& hash) const {
    try {
        return bcrypt::validatePassword(password, hash);
    } catch (const std::exception& e) {
        // If the hash format is completely invalid, treat as verification failure
        // rather than crashing the application
        std::cerr << "Password validation error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace ums
