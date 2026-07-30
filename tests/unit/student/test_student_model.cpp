#include <gtest/gtest.h>
#include <string>
#include "student/models/student.hpp"
#include "common/types.hpp"
#include "common/errors.hpp"
#include "crow.h"

using namespace ums;

// Test creating a valid student succeeds
TEST(StudentModelTest, CreateWithValidDataSucceeds) {
    auto result = Student::create(1, "CS2023001", "John", "Doe", 1, 3);
    EXPECT_TRUE(isSuccess(result));
    const Student& student = getValue(result);
    EXPECT_EQ(student.rollNumber(), "CS2023001");
    EXPECT_EQ(student.firstName(), "John");
    EXPECT_EQ(student.lastName(), "Doe");
    EXPECT_EQ(student.departmentId(), 1);
    EXPECT_EQ(student.semester(), 3);
}

// Test creating with empty roll number returns ValidationError
TEST(StudentModelTest, CreateWithEmptyRollNumberReturnsValidationError) {
    auto result = Student::create(1, "", "John", "Doe", 1, 3);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with invalid roll number format returns ValidationError
TEST(StudentModelTest, CreateWithInvalidRollNumberFormat) {
    auto result = Student::create(1, "CS!2023", "John", "Doe", 1, 3);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with too long roll number returns ValidationError
TEST(StudentModelTest, CreateWithTooLongRollNumber) {
    auto result = Student::create(1, "CS2023001000000000000000", "John", "Doe", 1, 3);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with empty first name returns ValidationError
TEST(StudentModelTest, CreateWithEmptyFirstNameReturnsValidationError) {
    auto result = Student::create(1, "CS2023001", "", "Doe", 1, 3);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with empty last name returns ValidationError
TEST(StudentModelTest, CreateWithEmptyLastNameReturnsValidationError) {
    auto result = Student::create(1, "CS2023001", "John", "", 1, 3);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with semester 0 returns ValidationError
TEST(StudentModelTest, CreateWithSemester0ReturnsValidationError) {
    auto result = Student::create(1, "CS2023001", "John", "Doe", 1, 0);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with semester 9 returns ValidationError
TEST(StudentModelTest, CreateWithSemester9ReturnsValidationError) {
    auto result = Student::create(1, "CS2023001", "John", "Doe", 1, 9);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with semester 1 succeeds
TEST(StudentModelTest, CreateWithValidSemester1Succeeds) {
    auto result = Student::create(1, "CS2023001", "John", "Doe", 1, 1);
    EXPECT_TRUE(isSuccess(result));
}

// Test creating with semester 8 succeeds
TEST(StudentModelTest, CreateWithValidSemester8Succeeds) {
    auto result = Student::create(1, "CS2023001", "John", "Doe", 1, 8);
    EXPECT_TRUE(isSuccess(result));
}

// Test fromDatabase constructs correctly
TEST(StudentModelTest, FromDatabaseConstructsCorrectly) {
    Student student = Student::fromDatabase(1, 2, "CS2023001", "John", "Doe", 3, 5, "2024-01-01", "2024-01-01", "2024-01-02");
    EXPECT_EQ(student.id(), 1);
    EXPECT_EQ(student.userId(), 2);
    EXPECT_EQ(student.rollNumber(), "CS2023001");
    EXPECT_EQ(student.firstName(), "John");
    EXPECT_EQ(student.lastName(), "Doe");
    EXPECT_EQ(student.departmentId(), 3);
    EXPECT_EQ(student.semester(), 5);
    EXPECT_EQ(student.enrollmentDate(), "2024-01-01");
    EXPECT_EQ(student.createdAt(), "2024-01-01");
    EXPECT_EQ(student.updatedAt(), "2024-01-02");
}

// Test toJson includes all fields
TEST(StudentModelTest, ToJsonIncludesAllFields) {
    Student student = Student::fromDatabase(1, 2, "CS2023001", "John", "Doe", 3, 5, "2024-01-01", "2024-01-01", "2024-01-02");
    auto json = student.toJson();
    std::string jsonString = json.dump();
    
    EXPECT_NE(jsonString.find("1"), std::string::npos);
    EXPECT_NE(jsonString.find("2"), std::string::npos);
    EXPECT_NE(jsonString.find("CS2023001"), std::string::npos);
    EXPECT_NE(jsonString.find("John"), std::string::npos);
    EXPECT_NE(jsonString.find("Doe"), std::string::npos);
    EXPECT_NE(jsonString.find("3"), std::string::npos);
    EXPECT_NE(jsonString.find("5"), std::string::npos);
    EXPECT_NE(jsonString.find("2024-01-01"), std::string::npos);
    EXPECT_NE(jsonString.find("2024-01-02"), std::string::npos);
}

// Test getters return correct values
TEST(StudentModelTest, GettersReturnCorrectValues) {
    Student student = Student::fromDatabase(10, 20, "R123", "A", "B", 5, 2, "e", "c", "u");
    EXPECT_EQ(student.id(), 10);
    EXPECT_EQ(student.userId(), 20);
    EXPECT_EQ(student.rollNumber(), "R123");
    EXPECT_EQ(student.firstName(), "A");
    EXPECT_EQ(student.lastName(), "B");
    EXPECT_EQ(student.departmentId(), 5);
    EXPECT_EQ(student.semester(), 2);
    EXPECT_EQ(student.enrollmentDate(), "e");
    EXPECT_EQ(student.createdAt(), "c");
    EXPECT_EQ(student.updatedAt(), "u");
}
