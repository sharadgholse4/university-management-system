#pragma once
#include <string>
#include <regex>
#include <utility>
#include "crow.h"
#include "common/types.hpp"
#include "common/errors.hpp"

namespace ums {

// WHY: The Department class encapsulates department data and validation rules.
// Following the factory pattern ensures valid state on creation.
class Department {
public:
    static Result<Department> create(std::string name, std::string code, int headProfessorId) {
        if (name.empty()) {
            return AppError{ErrorCode::ValidationError, "Department name cannot be empty"};
        }
        if (code.empty() || code.length() < 2 || code.length() > 10) {
            return AppError{ErrorCode::ValidationError, "Code must be 2-10 characters"};
        }
        static const std::regex codeRegex(R"([A-Z0-9]+)");
        if (!std::regex_match(code, codeRegex)) {
            return AppError{ErrorCode::ValidationError, "Code must be uppercase alphanumeric"};
        }
        if (headProfessorId < 0) {
            return AppError{ErrorCode::ValidationError, "Head professor ID must be >= 0"};
        }
        return Department(0, std::move(name), std::move(code), headProfessorId, "", "");
    }

    static Department fromDatabase(int id, std::string name, std::string code, 
                                   int headProfessorId, std::string createdAt, std::string updatedAt) {
        return Department(id, std::move(name), std::move(code), headProfessorId, 
                          std::move(createdAt), std::move(updatedAt));
    }

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& code() const { return code_; }
    int headProfessorId() const { return headProfessorId_; }
    const std::string& createdAt() const { return createdAt_; }
    const std::string& updatedAt() const { return updatedAt_; }

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["id"] = id_;
        json["name"] = name_;
        json["code"] = code_;
        json["headProfessorId"] = headProfessorId_;
        json["createdAt"] = createdAt_;
        json["updatedAt"] = updatedAt_;
        return json;
    }

private:
    Department(int id, std::string name, std::string code, int headProfessorId, 
               std::string createdAt, std::string updatedAt)
        : id_(id), name_(std::move(name)), code_(std::move(code)), 
          headProfessorId_(headProfessorId), createdAt_(std::move(createdAt)), updatedAt_(std::move(updatedAt)) {}

    int id_;
    std::string name_;
    std::string code_;
    int headProfessorId_;
    std::string createdAt_;
    std::string updatedAt_;
};

} // namespace ums
