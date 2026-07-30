#include <gtest/gtest.h>
#include <string>
#include "enrollment/models/enrollment.hpp"
#include "common/types.hpp"
#include "common/errors.hpp"
#include "crow.h"

using namespace ums;

// WHY: We need to ensure that the core domain model for Enrollments behaves correctly,
// particularly its factory method that enforces validation rules.

// Test creating with valid data succeeds
TEST(EnrollmentModelTest, CreateWithValidDataSucceeds) {
    auto result = Enrollment::create(1, 101);
    EXPECT_TRUE(isSuccess(result));
    const Enrollment& enrollment = getValue(result);
    EXPECT_EQ(enrollment.studentId(), 1);
    EXPECT_EQ(enrollment.courseId(), 101);
}

// Test creating with invalid student id returns ValidationError
TEST(EnrollmentModelTest, CreateWithInvalidStudentIdReturnsError) {
    auto result = Enrollment::create(0, 101);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);

    auto result2 = Enrollment::create(-1, 101);
    EXPECT_FALSE(isSuccess(result2));
    EXPECT_EQ(getError(result2).code, ErrorCode::ValidationError);
}

// Test creating with invalid course id returns ValidationError
TEST(EnrollmentModelTest, CreateWithInvalidCourseIdReturnsError) {
    auto result = Enrollment::create(1, 0);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);

    auto result2 = Enrollment::create(1, -5);
    EXPECT_FALSE(isSuccess(result2));
    EXPECT_EQ(getError(result2).code, ErrorCode::ValidationError);
}

// Test fromDatabase constructs correctly
TEST(EnrollmentModelTest, FromDatabaseConstructsCorrectly) {
    Enrollment enrollment = Enrollment::fromDatabase(10, 1, 101, "2024-01-01T10:00:00Z");
    EXPECT_EQ(enrollment.id(), 10);
    EXPECT_EQ(enrollment.studentId(), 1);
    EXPECT_EQ(enrollment.courseId(), 101);
    EXPECT_EQ(enrollment.enrolledAt(), "2024-01-01T10:00:00Z");
}

// Test toJson includes all fields correctly formatted
TEST(EnrollmentModelTest, ToJsonIncludesAllFields) {
    Enrollment enrollment = Enrollment::fromDatabase(10, 1, 101, "2024-01-01T10:00:00Z");
    auto json = enrollment.toJson();
    std::string jsonString = json.dump();
    
    EXPECT_NE(jsonString.find("\"id\":10"), std::string::npos);
    EXPECT_NE(jsonString.find("\"studentId\":1"), std::string::npos);
    EXPECT_NE(jsonString.find("\"courseId\":101"), std::string::npos);
    EXPECT_NE(jsonString.find("\"enrolledAt\":\"2024-01-01T10:00:00Z\""), std::string::npos);
}

// Additional test to ensure getters return correct types
TEST(EnrollmentModelTest, GettersReturnCorrectValues) {
    Enrollment enrollment = Enrollment::fromDatabase(5, 2, 202, "2024-05-05");
    EXPECT_EQ(enrollment.id(), 5);
    EXPECT_EQ(enrollment.studentId(), 2);
    EXPECT_EQ(enrollment.courseId(), 202);
    EXPECT_EQ(enrollment.enrolledAt(), "2024-05-05");
}

// Test invalid both IDs
TEST(EnrollmentModelTest, CreateWithInvalidBothIdsReturnsError) {
    auto result = Enrollment::create(0, 0);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating extreme valid IDs
TEST(EnrollmentModelTest, CreateWithExtremeValidIdsSucceeds) {
    auto result = Enrollment::create(999999, 888888);
    EXPECT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).studentId(), 999999);
}
