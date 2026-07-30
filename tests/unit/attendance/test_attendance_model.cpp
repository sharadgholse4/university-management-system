#include <gtest/gtest.h>
#include <string>
#include "attendance/models/attendance.hpp"
#include "common/errors.hpp"

namespace ums { namespace test {

class AttendanceModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup if needed
    }
};

// --- Create method tests ---

TEST_F(AttendanceModelTest, CreateWithValidDataSucceeds) {
    auto result = Attendance::create(1, 2, "2024-01-15", "present", 3);
    ASSERT_TRUE(isSuccess(result));
    
    auto attendance = getValue(result);
    EXPECT_EQ(attendance.studentId(), 1);
    EXPECT_EQ(attendance.courseId(), 2);
    EXPECT_EQ(attendance.date(), "2024-01-15");
    EXPECT_EQ(attendance.status(), "present");
    EXPECT_EQ(attendance.markedBy(), 3);
}

TEST_F(AttendanceModelTest, CreateWithInvalidStudentIdReturnsError) {
    auto result = Attendance::create(0, 2, "2024-01-15", "present", 3);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

TEST_F(AttendanceModelTest, CreateWithInvalidCourseIdReturnsError) {
    auto result = Attendance::create(1, -1, "2024-01-15", "present", 3);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

TEST_F(AttendanceModelTest, CreateWithEmptyDateReturnsError) {
    auto result = Attendance::create(1, 2, "", "present", 3);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

TEST_F(AttendanceModelTest, CreateWithInvalidDateFormatReturnsError) {
    auto result = Attendance::create(1, 2, "2024/01/15", "present", 3);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
    
    auto result2 = Attendance::create(1, 2, "bad", "present", 3);
    ASSERT_FALSE(isSuccess(result2));
    EXPECT_EQ(getError(result2).code, ErrorCode::ValidationError);
}

TEST_F(AttendanceModelTest, CreateWithValidDateFormatSucceeds) {
    auto result = Attendance::create(1, 2, "2024-01-15", "present", 3);
    ASSERT_TRUE(isSuccess(result));
}

TEST_F(AttendanceModelTest, CreateWithInvalidStatusReturnsError) {
    auto result = Attendance::create(1, 2, "2024-01-15", "unknown", 3);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

TEST_F(AttendanceModelTest, CreateWithPresentStatusSucceeds) {
    auto result = Attendance::create(1, 2, "2024-01-15", "present", 3);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).status(), "present");
}

TEST_F(AttendanceModelTest, CreateWithAbsentStatusSucceeds) {
    auto result = Attendance::create(1, 2, "2024-01-15", "absent", 3);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).status(), "absent");
}

TEST_F(AttendanceModelTest, CreateWithLateStatusSucceeds) {
    auto result = Attendance::create(1, 2, "2024-01-15", "late", 3);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).status(), "late");
}

TEST_F(AttendanceModelTest, CreateWithInvalidMarkedByReturnsError) {
    auto result = Attendance::create(1, 2, "2024-01-15", "present", -5);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// --- Construction, serialization and getters tests ---

TEST_F(AttendanceModelTest, FromDatabaseConstructsCorrectly) {
    Attendance attendance = Attendance::fromDatabase(
        10, 1, 2, "2024-01-15", "absent", 3, "2024-01-15 10:00:00", "2024-01-15 10:00:00"
    );
    
    EXPECT_EQ(attendance.id(), 10);
    EXPECT_EQ(attendance.studentId(), 1);
    EXPECT_EQ(attendance.courseId(), 2);
    EXPECT_EQ(attendance.date(), "2024-01-15");
    EXPECT_EQ(attendance.status(), "absent");
    EXPECT_EQ(attendance.markedBy(), 3);
    EXPECT_EQ(attendance.createdAt(), "2024-01-15 10:00:00");
    EXPECT_EQ(attendance.updatedAt(), "2024-01-15 10:00:00");
}

TEST_F(AttendanceModelTest, ToJsonIncludesAllFields) {
    Attendance attendance = Attendance::fromDatabase(
        10, 1, 2, "2024-01-15", "absent", 3, "2024-01-15 10:00:00", "2024-01-15 10:00:00"
    );
    
    auto json = attendance.toJson();
    auto parsed = crow::json::load(json.dump());
    ASSERT_TRUE(parsed);
    
    EXPECT_EQ(parsed["id"].i(), 10);
    EXPECT_EQ(parsed["studentId"].i(), 1);
    EXPECT_EQ(parsed["courseId"].i(), 2);
    EXPECT_EQ(std::string(parsed["date"].s()), "2024-01-15");
    EXPECT_EQ(std::string(parsed["status"].s()), "absent");
    EXPECT_EQ(parsed["markedBy"].i(), 3);
    EXPECT_EQ(std::string(parsed["createdAt"].s()), "2024-01-15 10:00:00");
    EXPECT_EQ(std::string(parsed["updatedAt"].s()), "2024-01-15 10:00:00");
}

TEST_F(AttendanceModelTest, GettersReturnCorrectValues) {
    Attendance attendance = Attendance::fromDatabase(
        10, 1, 2, "2024-01-15", "late", 3, "now", "now"
    );
    
    EXPECT_EQ(attendance.id(), 10);
    EXPECT_EQ(attendance.studentId(), 1);
    EXPECT_EQ(attendance.courseId(), 2);
    EXPECT_EQ(attendance.date(), "2024-01-15");
    EXPECT_EQ(attendance.status(), "late");
    EXPECT_EQ(attendance.markedBy(), 3);
    EXPECT_EQ(attendance.createdAt(), "now");
    EXPECT_EQ(attendance.updatedAt(), "now");
}

// --- Helper function tests ---

TEST_F(AttendanceModelTest, AttendanceStatusToStringWorks) {
    EXPECT_EQ(attendanceStatusToString(AttendanceStatus::Present), "present");
    EXPECT_EQ(attendanceStatusToString(AttendanceStatus::Absent), "absent");
    EXPECT_EQ(attendanceStatusToString(AttendanceStatus::Late), "late");
}

TEST_F(AttendanceModelTest, StringToAttendanceStatusWorks) {
    auto p = stringToAttendanceStatus("present");
    ASSERT_TRUE(isSuccess(p));
    EXPECT_EQ(getValue(p), AttendanceStatus::Present);
    
    auto a = stringToAttendanceStatus("absent");
    ASSERT_TRUE(isSuccess(a));
    EXPECT_EQ(getValue(a), AttendanceStatus::Absent);
    
    auto l = stringToAttendanceStatus("late");
    ASSERT_TRUE(isSuccess(l));
    EXPECT_EQ(getValue(l), AttendanceStatus::Late);
}

TEST_F(AttendanceModelTest, StringToAttendanceStatusInvalidReturnsError) {
    auto result = stringToAttendanceStatus("NotThere");
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

}} // namespace ums::test
