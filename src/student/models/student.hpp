#pragma once
#include <string>
#include <regex>
#include <utility>
#include "crow.h"
#include "common/types.hpp"
#include "common/errors.hpp"

namespace ums {

class Student {
public:
    // Factory method - validates student properties. Returns error if invalid.
    // WHY: By using a static factory method, we ensure that it's impossible to create
    // a Student object with invalid state in business logic.
    static Result<Student> create(int userId, std::string rollNumber, std::string firstName, 
                                  std::string lastName, int departmentId, int semester) {
        if (userId <= 0) {
            return AppError{ErrorCode::ValidationError, "User ID must be positive"};
        }
        if (rollNumber.empty() || rollNumber.length() < 3 || rollNumber.length() > 20) {
            return AppError{ErrorCode::ValidationError, "Roll number must be 3-20 characters"};
        }
        if (!isValidRollNumber(rollNumber)) {
            return AppError{ErrorCode::ValidationError, "Roll number must be alphanumeric with hyphens"};
        }
        if (firstName.empty() || firstName.length() > 100) {
            return AppError{ErrorCode::ValidationError, "First name must be 1-100 characters"};
        }
        if (lastName.empty() || lastName.length() > 100) {
            return AppError{ErrorCode::ValidationError, "Last name must be 1-100 characters"};
        }
        if (semester < 1 || semester > 8) {
            return AppError{ErrorCode::ValidationError, "Semester must be between 1 and 8"};
        }
        return Student(0, userId, std::move(rollNumber), std::move(firstName), 
                       std::move(lastName), departmentId, semester, "", "", "");
    }

    // For repository to reconstruct from DB rows - no validation needed
    // WHY: When reading from the database, data is already valid.
    static Student fromDatabase(int id, int userId, std::string rollNumber, std::string firstName, 
                                std::string lastName, int departmentId, int semester, 
                                std::string enrollmentDate, std::string createdAt, std::string updatedAt) {
        return Student(id, userId, std::move(rollNumber), std::move(firstName), std::move(lastName), 
                       departmentId, semester, std::move(enrollmentDate), std::move(createdAt), std::move(updatedAt));
    }

    // Getters
    int id() const { return id_; }
    int userId() const { return userId_; }
    const std::string& rollNumber() const { return rollNumber_; }
    const std::string& firstName() const { return firstName_; }
    const std::string& lastName() const { return lastName_; }
    int departmentId() const { return departmentId_; }
    int semester() const { return semester_; }
    const std::string& enrollmentDate() const { return enrollmentDate_; }
    const std::string& createdAt() const { return createdAt_; }
    const std::string& updatedAt() const { return updatedAt_; }

    // Convert to JSON for HTTP responses
    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["id"] = id_;
        json["userId"] = userId_;
        json["rollNumber"] = rollNumber_;
        json["firstName"] = firstName_;
        json["lastName"] = lastName_;
        json["departmentId"] = departmentId_;
        json["semester"] = semester_;
        json["enrollmentDate"] = enrollmentDate_;
        json["createdAt"] = createdAt_;
        json["updatedAt"] = updatedAt_;
        return json;
    }

private:
    Student(int id, int userId, std::string rollNumber, std::string firstName, std::string lastName, 
            int departmentId, int semester, std::string enrollmentDate, std::string createdAt, std::string updatedAt)
        : id_(id), userId_(userId), rollNumber_(std::move(rollNumber)), firstName_(std::move(firstName)), 
          lastName_(std::move(lastName)), departmentId_(departmentId), semester_(semester), 
          enrollmentDate_(std::move(enrollmentDate)), createdAt_(std::move(createdAt)), updatedAt_(std::move(updatedAt)) {}

    static bool isValidRollNumber(const std::string& rollNumber) {
        static const std::regex rollNumRegex(R"([a-zA-Z0-9\-]+)");
        return std::regex_match(rollNumber, rollNumRegex);
    }

    int id_;
    int userId_;
    std::string rollNumber_;
    std::string firstName_;
    std::string lastName_;
    int departmentId_;
    int semester_;
    std::string enrollmentDate_;
    std::string createdAt_;
    std::string updatedAt_;
};

} // namespace ums
