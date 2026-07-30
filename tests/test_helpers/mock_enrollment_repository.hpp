#pragma once
#include <gmock/gmock.h>
#include "enrollment/repositories/i_enrollment_repository.hpp"
#include "common/types.hpp"
#include <vector>

namespace ums::test {

// WHY: A GoogleMock class for IEnrollmentRepository allows us to unit test the 
// EnrollmentService in isolation, without hitting a real database.
class MockEnrollmentRepository : public IEnrollmentRepository {
public:
    MOCK_METHOD(void, initialize, (), (override));
    MOCK_METHOD(Result<Enrollment>, findById, (int id), (override));
    MOCK_METHOD(Result<Enrollment>, create, (int studentId, int courseId), (override));
    MOCK_METHOD(Result<bool>, deleteById, (int id), (override));
    MOCK_METHOD(Result<bool>, deleteByStudentAndCourse, (int studentId, int courseId), (override));
    MOCK_METHOD(Result<std::vector<Enrollment>>, findByStudentId, (int studentId), (override));
    MOCK_METHOD(Result<std::vector<Enrollment>>, findByCourseId, (int courseId), (override));
    MOCK_METHOD(Result<bool>, existsByStudentAndCourse, (int studentId, int courseId), (override));
};

} // namespace ums::test
