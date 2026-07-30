#pragma once
// ====================================================================================
// types.hpp — Core type definitions for the University Management System
// ====================================================================================
// WHY THIS FILE EXISTS:
//   Every module in the system uses Result<T> for error handling and Role for RBAC.
//   Centralizing these types here avoids duplication and ensures consistency.
//
// WHY std::variant INSTEAD OF EXCEPTIONS:
//   Using Result<T> = std::variant<T, AppError> forces callers to handle errors
//   explicitly at every call site. Exceptions are reserved for truly exceptional
//   conditions (e.g., out of memory), not for expected business errors like
//   "user not found" or "invalid email".
// ====================================================================================

#include <variant>
#include <string>
#include <stdexcept>
#include "common/errors.hpp" // AppError must be a complete type for std::variant

namespace ums {

// -------------------------------------------------------------------------
// Role Enum
// -------------------------------------------------------------------------
// WHY ENUM CLASS: Provides type safety — you can't accidentally compare a
// Role to an int or pass a string where a Role is expected.

enum class Role { Student, Professor, Admin };

// WHY INLINE: These are defined in a header; inline prevents ODR violations
// when included from multiple translation units.

inline std::string roleToString(Role role) {
    switch (role) {
        case Role::Student: return "student";
        case Role::Professor: return "professor";
        case Role::Admin: return "admin";
    }
    throw std::logic_error("Invalid role"); // unreachable if enum is complete
}

inline Role stringToRole(const std::string& str) {
    if (str == "student") return Role::Student;
    if (str == "professor") return Role::Professor;
    if (str == "admin") return Role::Admin;
    throw std::invalid_argument("Invalid role: " + str);
}

// -------------------------------------------------------------------------
// Result<T> — Algebraic error handling
// -------------------------------------------------------------------------
// WHY: This is the backbone of our error propagation strategy.
// Every repository, service, and controller method returns Result<T>.
// The caller must check isSuccess() before accessing the value.
//
// COMPLEXITY: All helper functions are O(1) — they just check/access the variant.

template<typename T>
using Result = std::variant<T, AppError>;

template<typename T>
bool isSuccess(const Result<T>& result) {
    return std::holds_alternative<T>(result);
}

template<typename T>
const T& getValue(const Result<T>& result) {
    return std::get<T>(result);
}

template<typename T>
T& getValue(Result<T>& result) {
    return std::get<T>(result);
}

template<typename T>
const AppError& getError(const Result<T>& result) {
    return std::get<AppError>(result);
}

} // namespace ums
