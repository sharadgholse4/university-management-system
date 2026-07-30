#pragma once
#include <vector>
#include <string>
#include <optional>
#include "crow.h"
#include "common/types.hpp"
#include "results/models/result.hpp"
#include "results/repositories/i_result_repository.hpp"

namespace ums {

struct AddResultRequest {
    int studentId;
    int courseId;
    std::string examType; // "midterm", "final", "assignment", "quiz"
    double marksObtained;
    double maxMarks;
    int semester;
};

struct UpdateResultRequest {
    double marksObtained;
    double maxMarks;
};

struct GpaSummary {
    int studentId;
    int semester; // 0 for cumulative CGPA, >0 for semester SGPA
    double gpa;   // Weighted grade point average (0.0 to 4.0)
    int totalExams;
    double totalMarksObtained;
    double totalMaxMarks;
    std::string standing; // "First Class with Distinction" (>=3.7), "First Class" (>=3.0), "Second Class" (>=2.5), "Pass" (>=2.0), "Fail" (<2.0)

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["studentId"] = studentId;
        json["semester"] = semester;
        json["gpa"] = gpa;
        json["totalExams"] = totalExams;
        json["totalMarksObtained"] = totalMarksObtained;
        json["totalMaxMarks"] = totalMaxMarks;
        json["standing"] = standing;
        return json;
    }
};

class ResultService {
public:
    ResultService(IResultRepository& resultRepo);
    Result<ExamResult> addResult(const AddResultRequest& request);
    Result<ExamResult> updateResult(int id, const UpdateResultRequest& request);
    Result<ExamResult> getResultById(int id);
    Result<std::vector<ExamResult>> getResultsByStudent(int studentId);
    Result<std::vector<ExamResult>> getResultsByStudentAndSemester(int studentId, int semester);
    Result<std::vector<ExamResult>> getResultsByCourse(int courseId);
    Result<GpaSummary> calculateSgpa(int studentId, int semester);
    Result<GpaSummary> calculateCgpa(int studentId);

private:
    IResultRepository& resultRepo_;
    std::optional<AppError> validateAddRequest(const AddResultRequest& request);
    std::optional<AppError> validateUpdateRequest(const UpdateResultRequest& request);
    GpaSummary computeGpa(int studentId, int semester, const std::vector<ExamResult>& results);
};

} // namespace ums
