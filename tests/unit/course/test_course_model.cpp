#include <gtest/gtest.h>
#include <string>
#include "course/models/department.hpp"
#include "course/models/course.hpp"
#include "common/types.hpp"
#include "common/errors.hpp"
#include "crow.h"

using namespace ums;

// Test Department creation with valid data succeeds
TEST(CourseModelTest, DepartmentCreateWithValidDataSucceeds) {
    auto result = Department::create("Computer Science", "CS", 101);
    EXPECT_TRUE(isSuccess(result));
    const Department& dept = getValue(result);
    EXPECT_EQ(dept.name(), "Computer Science");
    EXPECT_EQ(dept.code(), "CS");
    EXPECT_EQ(dept.headProfessorId(), 101);
}

// Test Department creation with empty name returns ValidationError
TEST(CourseModelTest, DepartmentCreateWithEmptyNameReturnsError) {
    auto result = Department::create("", "CS", 101);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test Department creation with empty code returns ValidationError
TEST(CourseModelTest, DepartmentCreateWithEmptyCodeReturnsError) {
    auto result = Department::create("Computer Science", "", 101);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test Department toJson includes all fields
TEST(CourseModelTest, DepartmentToJsonIncludesAllFields) {
    Department dept = Department::fromDatabase(1, "Computer Science", "CS", 101, "2024-01-01", "2024-01-02");
    auto json = dept.toJson();
    std::string jsonString = json.dump();
    
    EXPECT_NE(jsonString.find("1"), std::string::npos);
    EXPECT_NE(jsonString.find("Computer Science"), std::string::npos);
    EXPECT_NE(jsonString.find("CS"), std::string::npos);
    EXPECT_NE(jsonString.find("101"), std::string::npos);
    EXPECT_NE(jsonString.find("2024-01-01"), std::string::npos);
    EXPECT_NE(jsonString.find("2024-01-02"), std::string::npos);
}

// Test Course creation with valid data succeeds
TEST(CourseModelTest, CourseCreateWithValidDataSucceeds) {
    auto result = Course::create("CS101", "Intro to CS", 1, 101, 3, 1);
    EXPECT_TRUE(isSuccess(result));
    const Course& course = getValue(result);
    EXPECT_EQ(course.code(), "CS101");
    EXPECT_EQ(course.name(), "Intro to CS");
    EXPECT_EQ(course.departmentId(), 1);
    EXPECT_EQ(course.professorId(), 101);
    EXPECT_EQ(course.credits(), 3);
    EXPECT_EQ(course.semester(), 1);
}

// Test Course creation with invalid code returns ValidationError
TEST(CourseModelTest, CourseCreateWithInvalidCodeReturnsError) {
    auto result = Course::create("", "Intro to CS", 1, 101, 3, 1);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test Course creation with empty name returns ValidationError
TEST(CourseModelTest, CourseCreateWithEmptyNameReturnsError) {
    auto result = Course::create("CS101", "", 1, 101, 3, 1);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test Course creation with invalid departmentId (<=0) returns ValidationError
TEST(CourseModelTest, CourseCreateWithInvalidDepartmentIdReturnsError) {
    auto result = Course::create("CS101", "Intro to CS", 0, 101, 3, 1);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test Course creation with invalid professorId (<=0) returns ValidationError
TEST(CourseModelTest, CourseCreateWithInvalidProfessorIdReturnsError) {
    auto result = Course::create("CS101", "Intro to CS", 1, 0, 3, 1);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test Course creation with invalid credits (0 or 7) returns ValidationError
TEST(CourseModelTest, CourseCreateWithInvalidCreditsReturnsError) {
    auto result1 = Course::create("CS101", "Intro to CS", 1, 101, 0, 1);
    EXPECT_FALSE(isSuccess(result1));
    EXPECT_EQ(getError(result1).code, ErrorCode::ValidationError);
    
    auto result2 = Course::create("CS101", "Intro to CS", 1, 101, 7, 1);
    EXPECT_FALSE(isSuccess(result2));
    EXPECT_EQ(getError(result2).code, ErrorCode::ValidationError);
}

// Test Course creation with invalid semester (0 or 9) returns ValidationError
TEST(CourseModelTest, CourseCreateWithInvalidSemesterReturnsError) {
    auto result1 = Course::create("CS101", "Intro to CS", 1, 101, 3, 0);
    EXPECT_FALSE(isSuccess(result1));
    EXPECT_EQ(getError(result1).code, ErrorCode::ValidationError);
    
    auto result2 = Course::create("CS101", "Intro to CS", 1, 101, 3, 9);
    EXPECT_FALSE(isSuccess(result2));
    EXPECT_EQ(getError(result2).code, ErrorCode::ValidationError);
}

// Test Course toJson includes all fields
TEST(CourseModelTest, CourseToJsonIncludesAllFields) {
    Course course = Course::fromDatabase(1, "CS101", "Intro to CS", 1, 101, 3, 1, "2024-01-01", "2024-01-02");
    auto json = course.toJson();
    std::string jsonString = json.dump();
    
    EXPECT_NE(jsonString.find("1"), std::string::npos);
    EXPECT_NE(jsonString.find("CS101"), std::string::npos);
    EXPECT_NE(jsonString.find("Intro to CS"), std::string::npos);
    EXPECT_NE(jsonString.find("101"), std::string::npos);
    EXPECT_NE(jsonString.find("3"), std::string::npos);
    EXPECT_NE(jsonString.find("2024-01-01"), std::string::npos);
    EXPECT_NE(jsonString.find("2024-01-02"), std::string::npos);
}

// Test Department fromDatabase constructs correctly
TEST(CourseModelTest, DepartmentFromDatabaseConstructsCorrectly) {
    Department dept = Department::fromDatabase(1, "Computer Science", "CS", 101, "2024-01-01", "2024-01-02");
    EXPECT_EQ(dept.id(), 1);
    EXPECT_EQ(dept.name(), "Computer Science");
    EXPECT_EQ(dept.code(), "CS");
    EXPECT_EQ(dept.headProfessorId(), 101);
    EXPECT_EQ(dept.createdAt(), "2024-01-01");
    EXPECT_EQ(dept.updatedAt(), "2024-01-02");
}

// Test Course fromDatabase constructs correctly
TEST(CourseModelTest, CourseFromDatabaseConstructsCorrectly) {
    Course course = Course::fromDatabase(1, "CS101", "Intro to CS", 1, 101, 3, 1, "2024-01-01", "2024-01-02");
    EXPECT_EQ(course.id(), 1);
    EXPECT_EQ(course.code(), "CS101");
    EXPECT_EQ(course.name(), "Intro to CS");
    EXPECT_EQ(course.departmentId(), 1);
    EXPECT_EQ(course.professorId(), 101);
    EXPECT_EQ(course.credits(), 3);
    EXPECT_EQ(course.semester(), 1);
    EXPECT_EQ(course.createdAt(), "2024-01-01");
    EXPECT_EQ(course.updatedAt(), "2024-01-02");
}
