#include "results/services/result_service.hpp"

namespace ums {

ResultService::ResultService(IResultRepository& resultRepo) : resultRepo_(resultRepo) {}

std::optional<AppError> ResultService::validateAddRequest(const AddResultRequest& request) {
    if (request.studentId <= 0) return AppError{ErrorCode::ValidationError, "Student ID must be positive"};
    if (request.courseId <= 0) return AppError{ErrorCode::ValidationError, "Course ID must be positive"};
    if (request.marksObtained < 0) return AppError{ErrorCode::ValidationError, "Marks obtained cannot be negative"};
    if (request.maxMarks <= 0) return AppError{ErrorCode::ValidationError, "Max marks must be greater than zero"};
    if (request.marksObtained > request.maxMarks) return AppError{ErrorCode::ValidationError, "Marks obtained cannot exceed max marks"};
    if (request.semester < 1 || request.semester > 8) return AppError{ErrorCode::ValidationError, "Semester must be between 1 and 8"};
    
    auto typeResult = stringToExamType(request.examType);
    if (!isSuccess(typeResult)) return getError(typeResult);
    
    return std::nullopt;
}

std::optional<AppError> ResultService::validateUpdateRequest(const UpdateResultRequest& request) {
    if (request.marksObtained < 0) return AppError{ErrorCode::ValidationError, "Marks obtained cannot be negative"};
    if (request.maxMarks <= 0) return AppError{ErrorCode::ValidationError, "Max marks must be greater than zero"};
    if (request.marksObtained > request.maxMarks) return AppError{ErrorCode::ValidationError, "Marks obtained cannot exceed max marks"};
    
    return std::nullopt;
}

Result<ExamResult> ResultService::addResult(const AddResultRequest& request) {
    if (auto err = validateAddRequest(request)) return *err;
    
    // Check if result already exists
    auto existsResult = resultRepo_.existsByStudentCourseExamType(request.studentId, request.courseId, request.examType);
    if (!isSuccess(existsResult)) return getError(existsResult);
    if (getValue(existsResult)) {
        return AppError{ErrorCode::Conflict, "Result for this student, course, and exam type already exists"};
    }
    
    std::string grade = calculateGrade(request.marksObtained, request.maxMarks);
    
    return resultRepo_.create(request.studentId, request.courseId, request.examType, 
                              request.marksObtained, request.maxMarks, grade, request.semester);
}

Result<ExamResult> ResultService::updateResult(int id, const UpdateResultRequest& request) {
    if (auto err = validateUpdateRequest(request)) return *err;
    
    // Check if exists first
    auto result = resultRepo_.findById(id);
    if (!isSuccess(result)) return getError(result);
    
    std::string grade = calculateGrade(request.marksObtained, request.maxMarks);
    
    return resultRepo_.update(id, request.marksObtained, request.maxMarks, grade);
}

Result<ExamResult> ResultService::getResultById(int id) {
    return resultRepo_.findById(id);
}

Result<std::vector<ExamResult>> ResultService::getResultsByStudent(int studentId) {
    return resultRepo_.findByStudentId(studentId);
}

Result<std::vector<ExamResult>> ResultService::getResultsByStudentAndSemester(int studentId, int semester) {
    return resultRepo_.findByStudentAndSemester(studentId, semester);
}

Result<std::vector<ExamResult>> ResultService::getResultsByCourse(int courseId) {
    return resultRepo_.findByCourseId(courseId);
}

GpaSummary ResultService::computeGpa(int studentId, int semester, const std::vector<ExamResult>& results) {
    GpaSummary summary;
    summary.studentId = studentId;
    summary.semester = semester;
    summary.totalExams = results.size();
    summary.totalMarksObtained = 0.0;
    summary.totalMaxMarks = 0.0;
    
    if (results.empty()) {
        summary.gpa = 0.0;
        summary.standing = "N/A";
        return summary;
    }
    
    double totalWeightedGradePoints = 0.0;
    
    for (const auto& result : results) {
        summary.totalMarksObtained += result.marksObtained();
        summary.totalMaxMarks += result.maxMarks();
        
        // Weighting GPA by maxMarks of the exam to give proper weight to exams with more marks
        totalWeightedGradePoints += (result.gradePoints() * result.maxMarks());
    }
    
    summary.gpa = totalWeightedGradePoints / summary.totalMaxMarks;
    
    if (summary.gpa >= 3.7) summary.standing = "First Class with Distinction";
    else if (summary.gpa >= 3.0) summary.standing = "First Class";
    else if (summary.gpa >= 2.5) summary.standing = "Second Class";
    else if (summary.gpa >= 2.0) summary.standing = "Pass";
    else summary.standing = "Fail";
    
    return summary;
}

Result<GpaSummary> ResultService::calculateSgpa(int studentId, int semester) {
    auto results = resultRepo_.findByStudentAndSemester(studentId, semester);
    if (!isSuccess(results)) return getError(results);
    
    return computeGpa(studentId, semester, getValue(results));
}

Result<GpaSummary> ResultService::calculateCgpa(int studentId) {
    auto results = resultRepo_.findByStudentId(studentId);
    if (!isSuccess(results)) return getError(results);
    
    return computeGpa(studentId, 0, getValue(results));
}

} // namespace ums
