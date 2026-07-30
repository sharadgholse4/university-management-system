#pragma once
#include <vector>
#include <string>
#include "common/types.hpp"
#include "results/models/result.hpp"

namespace ums {

class IResultRepository {
public:
    virtual ~IResultRepository() = default;
    virtual void initialize() = 0;
    virtual Result<ExamResult> findById(int id) = 0;
    virtual Result<ExamResult> create(int studentId, int courseId, const std::string& examType,
                                     double marksObtained, double maxMarks, const std::string& grade,
                                     int semester) = 0;
    virtual Result<ExamResult> update(int id, double marksObtained, double maxMarks,
                                     const std::string& grade) = 0;
    virtual Result<std::vector<ExamResult>> findByStudentId(int studentId) = 0;
    virtual Result<std::vector<ExamResult>> findByStudentAndSemester(int studentId, int semester) = 0;
    virtual Result<std::vector<ExamResult>> findByCourseId(int courseId) = 0;
    virtual Result<bool> existsByStudentCourseExamType(int studentId, int courseId, const std::string& examType) = 0;
};

} // namespace ums
