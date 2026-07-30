#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "enrollment/services/enrollment_service.hpp"
#include "test_helpers/mock_enrollment_repository.hpp"
#include "test_helpers/mock_student_repository.hpp"
#include "test_helpers/mock_course_repository.hpp"
#include "student/models/student.hpp"
#include "course/models/course.hpp"

using namespace ums;
using namespace ums::test;
using ::testing::_;
using ::testing::Return;

// WHY: Validates the business logic for Enrollments, mocking out all repositories.
// Ensures that students and courses exist before creating an enrollment, and
// checks that duplicate enrollments are prevented.
class EnrollmentServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockEnrollmentRepo_ = std::make_shared<MockEnrollmentRepository>();
        mockStudentRepo_ = std::make_shared<MockStudentRepository>();
        mockCourseRepo_ = std::make_shared<MockCourseRepository>();
        
        service_ = std::make_unique<EnrollmentService>(*mockEnrollmentRepo_, *mockStudentRepo_, *mockCourseRepo_);
    }

    std::shared_ptr<MockEnrollmentRepository> mockEnrollmentRepo_;
    std::shared_ptr<MockStudentRepository> mockStudentRepo_;
    std::shared_ptr<MockCourseRepository> mockCourseRepo_;
    std::unique_ptr<EnrollmentService> service_;
};

// Test EnrollStudent valid data succeeds
TEST_F(EnrollmentServiceTest, EnrollStudentValidDataSucceeds) {
    EnrollStudentRequest req{1, 101};
    Enrollment expectedEnrollment = Enrollment::fromDatabase(10, 1, 101, "2024-01-01");
    
    Student mockStudent = Student::fromDatabase(1, 2, "R1", "A", "B", 1, 1, "d", "d", "d");
    Course mockCourse = Course::fromDatabase(101, 1, "C1", "Course 1", 3, "d", "d");
    
    EXPECT_CALL(*mockStudentRepo_, findById(1))
        .WillOnce(Return(Result<Student>(mockStudent)));
    EXPECT_CALL(*mockCourseRepo_, findById(101))
        .WillOnce(Return(Result<Course>(mockCourse)));
    EXPECT_CALL(*mockEnrollmentRepo_, existsByStudentAndCourse(1, 101))
        .WillOnce(Return(Result<bool>(false)));
    EXPECT_CALL(*mockEnrollmentRepo_, create(1, 101))
        .WillOnce(Return(Result<Enrollment>(expectedEnrollment)));
        
    auto result = service_->enrollStudent(req);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).id(), 10);
}

// Test EnrollStudent duplicate returns conflict
TEST_F(EnrollmentServiceTest, EnrollStudentDuplicateReturnsConflict) {
    EnrollStudentRequest req{1, 101};
    Student mockStudent = Student::fromDatabase(1, 2, "R1", "A", "B", 1, 1, "d", "d", "d");
    Course mockCourse = Course::fromDatabase(101, 1, "C1", "Course 1", 3, "d", "d");
    
    EXPECT_CALL(*mockStudentRepo_, findById(1))
        .WillOnce(Return(Result<Student>(mockStudent)));
    EXPECT_CALL(*mockCourseRepo_, findById(101))
        .WillOnce(Return(Result<Course>(mockCourse)));
    EXPECT_CALL(*mockEnrollmentRepo_, existsByStudentAndCourse(1, 101))
        .WillOnce(Return(Result<bool>(true)));
        
    auto result = service_->enrollStudent(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test EnrollStudent invalid student returns not found
TEST_F(EnrollmentServiceTest, EnrollStudentInvalidStudentReturnsNotFound) {
    EnrollStudentRequest req{999, 101};
    
    EXPECT_CALL(*mockStudentRepo_, findById(999))
        .WillOnce(Return(Result<Student>(AppError{ErrorCode::NotFound, "Student not found"})));
        
    auto result = service_->enrollStudent(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::NotFound);
}

// Test EnrollStudent invalid course returns not found
TEST_F(EnrollmentServiceTest, EnrollStudentInvalidCourseReturnsNotFound) {
    EnrollStudentRequest req{1, 999};
    Student mockStudent = Student::fromDatabase(1, 2, "R1", "A", "B", 1, 1, "d", "d", "d");
    
    EXPECT_CALL(*mockStudentRepo_, findById(1))
        .WillOnce(Return(Result<Student>(mockStudent)));
    EXPECT_CALL(*mockCourseRepo_, findById(999))
        .WillOnce(Return(Result<Course>(AppError{ErrorCode::NotFound, "Course not found"})));
        
    auto result = service_->enrollStudent(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::NotFound);
}

// Test DropEnrollment valid data succeeds
TEST_F(EnrollmentServiceTest, DropEnrollmentValidDataSucceeds) {
    EXPECT_CALL(*mockEnrollmentRepo_, existsByStudentAndCourse(1, 101))
        .WillOnce(Return(Result<bool>(true)));
    EXPECT_CALL(*mockEnrollmentRepo_, deleteByStudentAndCourse(1, 101))
        .WillOnce(Return(Result<bool>(true)));
        
    auto result = service_->dropEnrollment(1, 101);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_TRUE(getValue(result));
}

// Test DropEnrollment non-existent returns not found
TEST_F(EnrollmentServiceTest, DropEnrollmentNonExistentReturnsNotFound) {
    EXPECT_CALL(*mockEnrollmentRepo_, existsByStudentAndCourse(1, 101))
        .WillOnce(Return(Result<bool>(false)));
        
    auto result = service_->dropEnrollment(1, 101);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::NotFound);
}

// Test GetEnrollmentsByStudent returns list
TEST_F(EnrollmentServiceTest, GetEnrollmentsByStudentReturns) {
    std::vector<Enrollment> enrollments = { Enrollment::fromDatabase(10, 1, 101, "2024-01-01") };
    
    EXPECT_CALL(*mockEnrollmentRepo_, findByStudentId(1))
        .WillOnce(Return(Result<std::vector<Enrollment>>(enrollments)));
        
    auto result = service_->getEnrollmentsByStudent(1);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}

// Test GetEnrollmentsByCourse returns list
TEST_F(EnrollmentServiceTest, GetEnrollmentsByCourseReturns) {
    std::vector<Enrollment> enrollments = { Enrollment::fromDatabase(10, 1, 101, "2024-01-01") };
    
    EXPECT_CALL(*mockEnrollmentRepo_, findByCourseId(101))
        .WillOnce(Return(Result<std::vector<Enrollment>>(enrollments)));
        
    auto result = service_->getEnrollmentsByCourse(101);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}
