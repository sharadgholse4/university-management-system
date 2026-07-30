#pragma once
#include <string>
#include <regex>
#include <utility>
#include <stdexcept>
#include "crow.h"
#include "common/types.hpp"
#include "common/errors.hpp"

namespace ums {

enum class AttendanceStatus { Present, Absent, Late };

inline std::string attendanceStatusToString(AttendanceStatus status) {
    switch (status) {
        case AttendanceStatus::Present: return "present";
        case AttendanceStatus::Absent: return "absent";
        case AttendanceStatus::Late: return "late";
    }
    throw std::logic_error("Invalid attendance status");
}

inline Result<AttendanceStatus> stringToAttendanceStatus(const std::string& str) {
    if (str == "present") return AttendanceStatus::Present;
    if (str == "absent") return AttendanceStatus::Absent;
    if (str == "late") return AttendanceStatus::Late;
    return AppError{ErrorCode::ValidationError, "Invalid attendance status: " + str};
}

class Attendance {
public:
    static Result<Attendance> create(int studentId, int courseId, std::string date, 
                                     std::string status, int markedBy) {
        if (studentId <= 0) {
            return AppError{ErrorCode::ValidationError, "Student ID must be positive"};
        }
        if (courseId <= 0) {
            return AppError{ErrorCode::ValidationError, "Course ID must be positive"};
        }
        if (date.empty()) {
            return AppError{ErrorCode::ValidationError, "Date must not be empty"};
        }
        if (!isValidDate(date)) {
            return AppError{ErrorCode::ValidationError, "Date must be in YYYY-MM-DD format"};
        }
        auto statusResult = stringToAttendanceStatus(status);
        if (!isSuccess(statusResult)) {
            return AppError{ErrorCode::ValidationError, "Status must be 'present', 'absent', or 'late'"};
        }
        if (markedBy <= 0) {
            return AppError{ErrorCode::ValidationError, "Marked By user ID must be positive"};
        }
        return Attendance(0, studentId, courseId, std::move(date), std::move(status), markedBy, "", "");
    }

    static Attendance fromDatabase(int id, int studentId, int courseId, std::string date,
                                   std::string status, int markedBy, std::string createdAt, std::string updatedAt) {
        return Attendance(id, studentId, courseId, std::move(date), std::move(status), markedBy, std::move(createdAt), std::move(updatedAt));
    }

    int id() const { return id_; }
    int studentId() const { return studentId_; }
    int courseId() const { return courseId_; }
    const std::string& date() const { return date_; }
    const std::string& status() const { return status_; }
    int markedBy() const { return markedBy_; }
    const std::string& createdAt() const { return createdAt_; }
    const std::string& updatedAt() const { return updatedAt_; }

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["id"] = id_;
        json["studentId"] = studentId_;
        json["courseId"] = courseId_;
        json["date"] = date_;
        json["status"] = status_;
        json["markedBy"] = markedBy_;
        json["createdAt"] = createdAt_;
        json["updatedAt"] = updatedAt_;
        return json;
    }

private:
    Attendance(int id, int studentId, int courseId, std::string date, std::string status,
               int markedBy, std::string createdAt, std::string updatedAt)
        : id_(id), studentId_(studentId), courseId_(courseId), date_(std::move(date)),
          status_(std::move(status)), markedBy_(markedBy), createdAt_(std::move(createdAt)),
          updatedAt_(std::move(updatedAt)) {}

    static bool isValidDate(const std::string& date) {
        static const std::regex dateRegex(R"(^\d{4}-\d{2}-\d{2}$)");
        return std::regex_match(date, dateRegex);
    }

    int id_;
    int studentId_;
    int courseId_;
    std::string date_;
    std::string status_;
    int markedBy_;
    std::string createdAt_;
    std::string updatedAt_;
};

} // namespace ums
