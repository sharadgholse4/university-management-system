#pragma once
#include <gmock/gmock.h>
#include "student/repositories/i_student_repository.hpp"

namespace ums { namespace test {

class MockStudentRepository : public IStudentRepository {
public:
    MOCK_METHOD(void, initialize, (), (override));
    MOCK_METHOD(Result<Student>, findById, (int id), (override));
    MOCK_METHOD(Result<Student>, findByUserId, (int userId), (override));
    MOCK_METHOD(Result<Student>, findByRollNumber, (const std::string& rollNumber), (override));
    MOCK_METHOD(Result<Student>, create, (int userId, const std::string& rollNumber,
                const std::string& firstName, const std::string& lastName,
                int departmentId, int semester), (override));
    MOCK_METHOD(Result<Student>, update, (int id, const std::string& firstName, const std::string& lastName,
                int departmentId, int semester), (override));
    MOCK_METHOD((Result<std::vector<Student>>), findAll, (), (override));
    MOCK_METHOD((Result<std::vector<Student>>), findByDepartment, (int departmentId), (override));
    MOCK_METHOD(Result<bool>, existsByUserId, (int userId), (override));
    MOCK_METHOD(Result<bool>, existsByRollNumber, (const std::string& rollNumber), (override));
};

}} // namespace ums::test
