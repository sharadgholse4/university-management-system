#pragma once
#include <string>
#include "crow.h"
#include "common/types.hpp"
#include "common/errors.hpp"

namespace ums {

class Enrollment {
public:
    // WHY: Factory method pattern ensures that all Enrollment objects created
    // are valid according to business rules.
    static Result<Enrollment> create(int studentId, int courseId) {
        if (studentId <= 0) {
            return AppError{ErrorCode::ValidationError, "Invalid student ID"};
        }
        if (courseId <= 0) {
            return AppError{ErrorCode::ValidationError, "Invalid course ID"};
        }
        return Enrollment(0, studentId, courseId, "");
    }

    // WHY: Allows the repository to reconstruct an object from DB data without
    // going through business validation logic (which was already run on creation).
    static Enrollment fromDatabase(int id, int studentId, int courseId, const std::string& enrolledAt) {
        return Enrollment(id, studentId, courseId, enrolledAt);
    }

    int id() const { return id_; }
    int studentId() const { return studentId_; }
    int courseId() const { return courseId_; }
    const std::string& enrolledAt() const { return enrolledAt_; }

    // WHY: Standardize serialization for REST API responses. CamelCase keys.
    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["id"] = id_;
        json["studentId"] = studentId_;
        json["courseId"] = courseId_;
        json["enrolledAt"] = enrolledAt_;
        return json;
    }

private:
    Enrollment(int id, int studentId, int courseId, const std::string& enrolledAt)
        : id_(id), studentId_(studentId), courseId_(courseId), enrolledAt_(enrolledAt) {}

    int id_;
    int studentId_;
    int courseId_;
    std::string enrolledAt_;
};

} // namespace ums
