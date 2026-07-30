#pragma once
#include <string>
#include <regex>
#include "crow.h"
#include "common/types.hpp"
#include "common/errors.hpp"

namespace ums {

class User {
public:
    // Factory method - validates email format. Returns error if invalid.
    // This makes invalid User objects unrepresentable.
    // WHY: By using a static factory method instead of a public constructor,
    // we ensure that it's impossible to create a User object with invalid state
    // (e.g. malformed email) in business logic.
    static Result<User> create(std::string email, std::string passwordHash, Role role) {
        if (email.empty()) {
            return AppError{ErrorCode::ValidationError, "Email is required"};
        }
        if (!isValidEmail(email)) {
            return AppError{ErrorCode::ValidationError, "Invalid email format"};
        }
        if (passwordHash.empty()) {
            return AppError{ErrorCode::ValidationError, "Password hash is required"};
        }
        return User(0, std::move(email), std::move(passwordHash), role, "", "");
    }

    // For repository to reconstruct from DB rows - no validation needed
    // because the DB enforces constraints, and data was validated on insert.
    // WHY: When reading from the database, we assume the data is already valid.
    // Re-validating would be redundant and slow down DB reads. This method is
    // only meant to be used by the repository layer.
    static User fromDatabase(int id, std::string email, std::string passwordHash,
                              Role role, std::string createdAt, std::string updatedAt) {
        return User(id, std::move(email), std::move(passwordHash), role,
                    std::move(createdAt), std::move(updatedAt));
    }

    // Getters
    // WHY: Getters return primitive types by value and objects/strings by const reference.
    // Returning by const reference avoids unnecessary copies, improving performance.
    int id() const { return id_; }
    const std::string& email() const { return email_; }
    const std::string& passwordHash() const { return passwordHash_; }
    Role role() const { return role_; }
    const std::string& createdAt() const { return createdAt_; }
    const std::string& updatedAt() const { return updatedAt_; }

    // SECURITY: Never includes passwordHash in JSON output
    // WHY: The password hash is sensitive information and should never leave the
    // backend (e.g. sent in an API response). We explicitly omit it here.
    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["id"] = id_;
        json["email"] = email_;
        json["role"] = roleToString(role_);
        json["createdAt"] = createdAt_;
        json["updatedAt"] = updatedAt_;
        return json;
    }

private:
    User(int id, std::string email, std::string passwordHash,
         Role role, std::string createdAt, std::string updatedAt)
        : id_(id), email_(std::move(email)), passwordHash_(std::move(passwordHash)),
          role_(role), createdAt_(std::move(createdAt)), updatedAt_(std::move(updatedAt)) {}

    static bool isValidEmail(const std::string& email) {
        // Simple but effective email regex
        static const std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        return std::regex_match(email, emailRegex);
    }

    int id_;
    std::string email_;
    std::string passwordHash_;
    Role role_;
    std::string createdAt_;
    std::string updatedAt_;
};

} // namespace ums
