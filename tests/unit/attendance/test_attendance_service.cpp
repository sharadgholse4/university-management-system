#include <gtest/gtest.h>
#include <memory>
#include "attendance/services/attendance_service.hpp"
#include "test_helpers/mock_attendance_repository.hpp"
#include "common/errors.hpp"

namespace ums { namespace test {

using ::testing::_;
using ::testing::Return;

class AttendanceServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockRepo_ = std::make_shared<MockAttendanceRepository>();
        service_ = std::make_unique<AttendanceService>(*mockRepo_);
    }

    std::shared_ptr<MockAttendanceRepository> mockRepo_;
    std::unique_ptr<AttendanceService> service_;
};

TEST_F(AttendanceServiceTest, MarkAttendanceValidDataSucceeds) {
    MarkAttendanceRequest req{1, 101, "2024-01-15", "present", 1};
    
    EXPECT_CALL(*mockRepo_, existsByStudentCourseDate(1, 101, "2024-01-15"))
        .WillOnce(Return(Result<bool>(false)));
        
    Attendance mockAtt = Attendance::fromDatabase(10, 1, 101, "2024-01-15", "present", 1, "now", "now");
    EXPECT_CALL(*mockRepo_, create(1, 101, "2024-01-15", "present", 1))
        .WillOnce(Return(Result<Attendance>(mockAtt)));
        
    auto result = service_->markAttendance(req);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).id(), 10);
}

TEST_F(AttendanceServiceTest, MarkAttendanceDuplicateReturnsConflict) {
    MarkAttendanceRequest req{1, 101, "2024-01-15", "present", 1};
    
    EXPECT_CALL(*mockRepo_, existsByStudentCourseDate(1, 101, "2024-01-15"))
        .WillOnce(Return(Result<bool>(true)));
        
    auto result = service_->markAttendance(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

TEST_F(AttendanceServiceTest, MarkAttendanceInvalidDataReturnsValidationError) {
    // Invalid student ID
    MarkAttendanceRequest req{0, 101, "2024-01-15", "present", 1};
    
    // Validates via Attendance::create(), which returns validation error
    auto result = service_->markAttendance(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

TEST_F(AttendanceServiceTest, UpdateAttendanceValidDataSucceeds) {
    UpdateAttendanceRequest req{"absent"};
    
    Attendance existingAtt = Attendance::fromDatabase(10, 1, 101, "2024-01-15", "present", 1, "now", "now");
    EXPECT_CALL(*mockRepo_, findById(10))
        .WillOnce(Return(Result<Attendance>(existingAtt)));
        
    Attendance updatedAtt = Attendance::fromDatabase(10, 1, 101, "2024-01-15", "absent", 1, "now", "now");
    EXPECT_CALL(*mockRepo_, update(10, "absent"))
        .WillOnce(Return(Result<Attendance>(updatedAtt)));
        
    auto result = service_->updateAttendance(10, req);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).status(), "absent");
}

TEST_F(AttendanceServiceTest, UpdateAttendanceInvalidStatusReturnsValidationError) {
    UpdateAttendanceRequest req{"InvalidStatus"};
    
    auto result = service_->updateAttendance(10, req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

TEST_F(AttendanceServiceTest, GetAttendanceByIdExistingReturns) {
    Attendance mockAtt = Attendance::fromDatabase(10, 1, 101, "2024-01-15", "present", 1, "now", "now");
    
    EXPECT_CALL(*mockRepo_, findById(10))
        .WillOnce(Return(Result<Attendance>(mockAtt)));
        
    auto result = service_->getAttendanceById(10);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).id(), 10);
}

TEST_F(AttendanceServiceTest, GetAttendanceByIdNonExistentReturnsNotFound) {
    EXPECT_CALL(*mockRepo_, findById(99))
        .WillOnce(Return(Result<Attendance>(AppError{ErrorCode::NotFound, "Not found"})));
        
    auto result = service_->getAttendanceById(99);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::NotFound);
}

TEST_F(AttendanceServiceTest, GetAttendanceByStudentReturns) {
    std::vector<Attendance> attList;
    attList.push_back(Attendance::fromDatabase(10, 1, 101, "2024-01-15", "present", 1, "now", "now"));
    
    EXPECT_CALL(*mockRepo_, findByStudentId(1))
        .WillOnce(Return(Result<std::vector<Attendance>>(attList)));
        
    auto result = service_->getAttendanceByStudent(1);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}

TEST_F(AttendanceServiceTest, GetAttendanceByCourseReturns) {
    std::vector<Attendance> attList;
    attList.push_back(Attendance::fromDatabase(10, 1, 101, "2024-01-15", "present", 1, "now", "now"));
    
    EXPECT_CALL(*mockRepo_, findByCourseId(101))
        .WillOnce(Return(Result<std::vector<Attendance>>(attList)));
        
    auto result = service_->getAttendanceByCourse(101);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}

TEST_F(AttendanceServiceTest, GetAttendanceByStudentAndCourseReturns) {
    std::vector<Attendance> attList;
    attList.push_back(Attendance::fromDatabase(10, 1, 101, "2024-01-15", "present", 1, "now", "now"));
    
    EXPECT_CALL(*mockRepo_, findByStudentAndCourse(1, 101))
        .WillOnce(Return(Result<std::vector<Attendance>>(attList)));
        
    auto result = service_->getAttendanceByStudentAndCourse(1, 101);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}

}} // namespace ums::test
