#pragma once
#include <gmock/gmock.h>
#include "attendance/repositories/i_attendance_repository.hpp"

namespace ums { namespace test {

class MockAttendanceRepository : public IAttendanceRepository {
public:
    MOCK_METHOD(void, initialize, (), (override));
    MOCK_METHOD(Result<Attendance>, findById, (int id), (override));
    MOCK_METHOD(Result<Attendance>, create, (int studentId, int courseId, const std::string& date,
                const std::string& status, int markedBy), (override));
    MOCK_METHOD(Result<Attendance>, update, (int id, const std::string& status), (override));
    MOCK_METHOD((Result<std::vector<Attendance>>), findByStudentId, (int studentId), (override));
    MOCK_METHOD((Result<std::vector<Attendance>>), findByCourseId, (int courseId), (override));
    MOCK_METHOD((Result<std::vector<Attendance>>), findByStudentAndCourse, (int studentId, int courseId), (override));
    MOCK_METHOD((Result<std::vector<Attendance>>), findByCourseAndDate, (int courseId, const std::string& date), (override));
    MOCK_METHOD(Result<bool>, existsByStudentCourseDate, (int studentId, int courseId, const std::string& date), (override));
};

}} // namespace ums::test
