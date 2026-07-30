#pragma once
#include <string>
#include <utility>
#include "crow.h"
#include "common/types.hpp"
#include "common/errors.hpp"

namespace ums {

// WHY: The Course class encapsulates course data and validation rules.
// Factory pattern ensures we don't instantiate invalid courses in business logic.
class Course {
public:
    static Result<Course> create(std::string code, std::string name, int departmentId, 
                                 int professorId, int credits, int semester) {
        if (code.empty()) {
            return AppError{ErrorCode::ValidationError, "Course code cannot be empty"};
        }
        if (name.empty()) {
            return AppError{ErrorCode::ValidationError, "Course name cannot be empty"};
        }
        if (departmentId <= 0) {
            return AppError{ErrorCode::ValidationError, "Department ID must be positive"};
        }
        if (professorId <= 0) {
            return AppError{ErrorCode::ValidationError, "Professor ID must be positive"};
        }
        if (credits < 1 || credits > 6) {
            return AppError{ErrorCode::ValidationError, "Credits must be between 1 and 6"};
        }
        if (semester < 1 || semester > 8) {
            return AppError{ErrorCode::ValidationError, "Semester must be between 1 and 8"};
        }
        return Course(0, std::move(code), std::move(name), departmentId, professorId, credits, semester, "", "");
    }

    static Course fromDatabase(int id, std::string code, std::string name, int departmentId, 
                               int professorId, int credits, int semester, 
                               std::string createdAt, std::string updatedAt) {
        return Course(id, std::move(code), std::move(name), departmentId, professorId, credits, semester, 
                      std::move(createdAt), std::move(updatedAt));
    }

    int id() const { return id_; }
    const std::string& code() const { return code_; }
    const std::string& name() const { return name_; }
    int departmentId() const { return departmentId_; }
    int professorId() const { return professorId_; }
    int credits() const { return credits_; }
    int semester() const { return semester_; }
    const std::string& createdAt() const { return createdAt_; }
    const std::string& updatedAt() const { return updatedAt_; }

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["id"] = id_;
        json["code"] = code_;
        json["name"] = name_;
        json["departmentId"] = departmentId_;
        json["professorId"] = professorId_;
        json["credits"] = credits_;
        json["semester"] = semester_;
        json["createdAt"] = createdAt_;
        json["updatedAt"] = updatedAt_;
        return json;
    }

private:
    Course(int id, std::string code, std::string name, int departmentId, int professorId, 
           int credits, int semester, std::string createdAt, std::string updatedAt)
        : id_(id), code_(std::move(code)), name_(std::move(name)), departmentId_(departmentId), 
          professorId_(professorId), credits_(credits), semester_(semester), 
          createdAt_(std::move(createdAt)), updatedAt_(std::move(updatedAt)) {}

    int id_;
    std::string code_;
    std::string name_;
    int departmentId_;
    int professorId_;
    int credits_;
    int semester_;
    std::string createdAt_;
    std::string updatedAt_;
};

} // namespace ums
