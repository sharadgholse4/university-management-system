#pragma once
#include <vector>
#include <string>
#include <gmock/gmock.h>
#include "course/repositories/i_department_repository.hpp"
#include "course/repositories/i_course_repository.hpp"
#include "common/types.hpp"

namespace ums::test {

class MockDepartmentRepository : public IDepartmentRepository {
public:
    MOCK_METHOD(void, initialize, (), (override));
    MOCK_METHOD(Result<Department>, findById, (int), (override));
    MOCK_METHOD(Result<Department>, findByCode, (const std::string&), (override));
    MOCK_METHOD(Result<Department>, create, (const std::string&, const std::string&, int), (override));
    MOCK_METHOD(Result<Department>, update, (int, const std::string&, const std::string&, int), (override));
    MOCK_METHOD(Result<std::vector<Department>>, findAll, (), (override));
    MOCK_METHOD(Result<bool>, existsByCode, (const std::string&), (override));
};

class MockCourseRepository : public ICourseRepository {
public:
    MOCK_METHOD(void, initialize, (), (override));
    MOCK_METHOD(Result<Course>, findById, (int), (override));
    MOCK_METHOD(Result<Course>, findByCode, (const std::string&), (override));
    MOCK_METHOD(Result<Course>, create, (const std::string&, const std::string&, int, int, int, int), (override));
    MOCK_METHOD(Result<Course>, update, (int, const std::string&, int, int, int), (override));
    MOCK_METHOD(Result<std::vector<Course>>, findAll, (), (override));
    MOCK_METHOD(Result<std::vector<Course>>, findByDepartmentId, (int), (override));
    MOCK_METHOD(Result<std::vector<Course>>, findByProfessorId, (int), (override));
    MOCK_METHOD(Result<std::vector<Course>>, findBySemester, (int), (override));
    MOCK_METHOD(Result<bool>, existsByCode, (const std::string&), (override));
};

} // namespace ums::test
