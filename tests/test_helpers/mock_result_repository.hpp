#pragma once
#include <gmock/gmock.h>
#include "results/repositories/i_result_repository.hpp"

namespace ums {
namespace test {

class MockResultRepository : public IResultRepository {
public:
    MOCK_METHOD(void, initialize, (), (override));
    MOCK_METHOD(Result<ExamResult>, findById, (int id), (override));
    MOCK_METHOD(Result<ExamResult>, create, (int studentId, int courseId, const std::string& examType,
                double marksObtained, double maxMarks, const std::string& grade, int semester), (override));
    MOCK_METHOD(Result<ExamResult>, update, (int id, double marksObtained, double maxMarks, const std::string& grade), (override));
    MOCK_METHOD((Result<std::vector<ExamResult>>), findByStudentId, (int studentId), (override));
    MOCK_METHOD((Result<std::vector<ExamResult>>), findByStudentAndSemester, (int studentId, int semester), (override));
    MOCK_METHOD((Result<std::vector<ExamResult>>), findByCourseId, (int courseId), (override));
    MOCK_METHOD(Result<bool>, existsByStudentCourseExamType, (int studentId, int courseId, const std::string& examType), (override));
};

} // namespace test
} // namespace ums
