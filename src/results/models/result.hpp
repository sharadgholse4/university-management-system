#pragma once
#include <string>
#include <utility>
#include "crow.h"
#include "common/types.hpp"
#include "common/errors.hpp"

namespace ums {

enum class ExamType { Midterm, Final, Assignment, Quiz };

inline std::string examTypeToString(ExamType type) {
    switch (type) {
        case ExamType::Midterm: return "midterm";
        case ExamType::Final: return "final";
        case ExamType::Assignment: return "assignment";
        case ExamType::Quiz: return "quiz";
    }
    return "";
}

inline Result<ExamType> stringToExamType(const std::string& str) {
    std::string lower = str;
    for (char& c : lower) c = std::tolower(static_cast<unsigned char>(c));
    if (lower == "midterm") return ExamType::Midterm;
    if (lower == "final") return ExamType::Final;
    if (lower == "assignment") return ExamType::Assignment;
    if (lower == "quiz") return ExamType::Quiz;
    return AppError{ErrorCode::ValidationError, "Invalid exam type: " + str};
}

inline std::string calculateGrade(double marksObtained, double maxMarks) {
    double percentage = (marksObtained / maxMarks) * 100.0;
    if (percentage >= 90) return "A+";
    if (percentage >= 80) return "A";
    if (percentage >= 75) return "B+";
    if (percentage >= 70) return "B";
    if (percentage >= 65) return "C+";
    if (percentage >= 60) return "C";
    if (percentage >= 50) return "D";
    return "F";
}

inline double gradeToPoints(const std::string& grade) {
    if (grade == "A+") return 4.0;
    if (grade == "A") return 4.0;
    if (grade == "B+") return 3.5;
    if (grade == "B") return 3.0;
    if (grade == "C+") return 2.5;
    if (grade == "C") return 2.0;
    if (grade == "D") return 1.0;
    if (grade == "F") return 0.0;
    return 0.0;
}

class ExamResult {
public:
    static Result<ExamResult> create(int studentId, int courseId, const std::string& examTypeStr, 
                                     double marksObtained, double maxMarks, int semester) {
        if (studentId <= 0) return AppError{ErrorCode::ValidationError, "Student ID must be positive"};
        if (courseId <= 0) return AppError{ErrorCode::ValidationError, "Course ID must be positive"};
        
        auto typeResult = stringToExamType(examTypeStr);
        if (!isSuccess(typeResult)) return getError(typeResult);
        
        if (marksObtained < 0) return AppError{ErrorCode::ValidationError, "Marks obtained cannot be negative"};
        if (maxMarks <= 0) return AppError{ErrorCode::ValidationError, "Max marks must be greater than zero"};
        if (marksObtained > maxMarks) return AppError{ErrorCode::ValidationError, "Marks obtained cannot exceed max marks"};
        if (semester < 1 || semester > 8) return AppError{ErrorCode::ValidationError, "Semester must be between 1 and 8"};
        
        std::string grade = calculateGrade(marksObtained, maxMarks);
        
        return ExamResult(0, studentId, courseId, examTypeStr, marksObtained, maxMarks, std::move(grade), semester, "", "");
    }

    static ExamResult fromDatabase(int id, int studentId, int courseId, std::string examType, 
                                   double marksObtained, double maxMarks, std::string grade, 
                                   int semester, std::string createdAt, std::string updatedAt) {
        return ExamResult(id, studentId, courseId, std::move(examType), marksObtained, maxMarks, 
                          std::move(grade), semester, std::move(createdAt), std::move(updatedAt));
    }

    int id() const { return id_; }
    int studentId() const { return studentId_; }
    int courseId() const { return courseId_; }
    const std::string& examType() const { return examType_; }
    double marksObtained() const { return marksObtained_; }
    double maxMarks() const { return maxMarks_; }
    const std::string& grade() const { return grade_; }
    int semester() const { return semester_; }
    const std::string& createdAt() const { return createdAt_; }
    const std::string& updatedAt() const { return updatedAt_; }

    double percentage() const { return (marksObtained_ / maxMarks_) * 100.0; }
    double gradePoints() const { return gradeToPoints(grade_); }

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["id"] = id_;
        json["studentId"] = studentId_;
        json["courseId"] = courseId_;
        json["examType"] = examType_;
        json["marksObtained"] = marksObtained_;
        json["maxMarks"] = maxMarks_;
        json["grade"] = grade_;
        json["semester"] = semester_;
        json["percentage"] = percentage();
        json["gradePoints"] = gradePoints();
        json["createdAt"] = createdAt_;
        json["updatedAt"] = updatedAt_;
        return json;
    }

private:
    ExamResult(int id, int studentId, int courseId, std::string examType, double marksObtained, 
               double maxMarks, std::string grade, int semester, std::string createdAt, std::string updatedAt)
        : id_(id), studentId_(studentId), courseId_(courseId), examType_(std::move(examType)), 
          marksObtained_(marksObtained), maxMarks_(maxMarks), grade_(std::move(grade)), semester_(semester), 
          createdAt_(std::move(createdAt)), updatedAt_(std::move(updatedAt)) {}

    int id_;
    int studentId_;
    int courseId_;
    std::string examType_;
    double marksObtained_;
    double maxMarks_;
    std::string grade_;
    int semester_;
    std::string createdAt_;
    std::string updatedAt_;
};

} // namespace ums
