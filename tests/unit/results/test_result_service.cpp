#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "results/services/result_service.hpp"
#include "test_helpers/mock_result_repository.hpp"

using namespace ums;
using namespace ums::test;
using ::testing::_;
using ::testing::Return;

class ResultServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockResultRepository> mockRepo_;
    std::unique_ptr<ResultService> service_;

    void SetUp() override {
        mockRepo_ = std::make_shared<MockResultRepository>();
        service_ = std::make_unique<ResultService>(*mockRepo_);
    }
};

TEST_F(ResultServiceTest, AddResultValidDataSucceeds) {
    AddResultRequest req{1, 100, "final", 85.0, 100.0, 1};
    
    EXPECT_CALL(*mockRepo_, existsByStudentCourseExamType(1, 100, "final"))
        .WillOnce(Return(Result<bool>(false)));
        
    auto expectedModel = ExamResult::fromDatabase(1, 1, 100, "final", 85.0, 100.0, "A", 1, "", "");
    EXPECT_CALL(*mockRepo_, create(1, 100, "final", 85.0, 100.0, "A", 1))
        .WillOnce(Return(Result<ExamResult>(expectedModel)));
        
    auto res = service_->addResult(req);
    ASSERT_TRUE(isSuccess(res));
    EXPECT_EQ(getValue(res).id(), 1);
}

TEST_F(ResultServiceTest, AddResultDuplicateReturnsConflict) {
    AddResultRequest req{1, 100, "final", 85.0, 100.0, 1};
    
    EXPECT_CALL(*mockRepo_, existsByStudentCourseExamType(1, 100, "final"))
        .WillOnce(Return(Result<bool>(true)));
        
    auto res = service_->addResult(req);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::Conflict);
}

TEST_F(ResultServiceTest, AddResultInvalidDataReturnsValidationError) {
    AddResultRequest req{0, 100, "final", 85.0, 100.0, 1}; // invalid student id
    
    auto res = service_->addResult(req);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
}

TEST_F(ResultServiceTest, UpdateResultValidDataSucceeds) {
    UpdateResultRequest req{95.0, 100.0};
    
    auto existingModel = ExamResult::fromDatabase(1, 1, 100, "final", 85.0, 100.0, "A", 1, "", "");
    EXPECT_CALL(*mockRepo_, findById(1))
        .WillOnce(Return(Result<ExamResult>(existingModel)));
        
    auto updatedModel = ExamResult::fromDatabase(1, 1, 100, "final", 95.0, 100.0, "A+", 1, "", "");
    EXPECT_CALL(*mockRepo_, update(1, 95.0, 100.0, "A+"))
        .WillOnce(Return(Result<ExamResult>(updatedModel)));
        
    auto res = service_->updateResult(1, req);
    ASSERT_TRUE(isSuccess(res));
    EXPECT_EQ(getValue(res).grade(), "A+");
}

TEST_F(ResultServiceTest, UpdateResultInvalidMarksReturnsValidationError) {
    UpdateResultRequest req{105.0, 100.0}; // invalid marks (exceeds maxMarks)
    
    // validateUpdateRequest fails early before calling findById
    auto res = service_->updateResult(1, req);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
}

TEST_F(ResultServiceTest, GetResultByIdExistingReturns) {
    auto existingModel = ExamResult::fromDatabase(1, 1, 100, "final", 85.0, 100.0, "A", 1, "", "");
    EXPECT_CALL(*mockRepo_, findById(1))
        .WillOnce(Return(Result<ExamResult>(existingModel)));
        
    auto res = service_->getResultById(1);
    ASSERT_TRUE(isSuccess(res));
    EXPECT_EQ(getValue(res).id(), 1);
}

TEST_F(ResultServiceTest, GetResultByIdNonExistentReturnsNotFound) {
    EXPECT_CALL(*mockRepo_, findById(1))
        .WillOnce(Return(Result<ExamResult>(AppError{ErrorCode::NotFound, "Not found"})));
        
    auto res = service_->getResultById(1);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::NotFound);
}

TEST_F(ResultServiceTest, GetResultsByStudentReturns) {
    std::vector<ExamResult> results = {
        ExamResult::fromDatabase(1, 1, 100, "final", 85.0, 100.0, "A", 1, "", "")
    };
    
    EXPECT_CALL(*mockRepo_, findByStudentId(1))
        .WillOnce(Return(Result<std::vector<ExamResult>>(results)));
        
    auto res = service_->getResultsByStudent(1);
    ASSERT_TRUE(isSuccess(res));
    EXPECT_EQ(getValue(res).size(), 1);
}

TEST_F(ResultServiceTest, CalculateSgpaComputesCorrectGpa) {
    std::vector<ExamResult> results = {
        ExamResult::fromDatabase(1, 1, 100, "final", 90.0, 100.0, "A+", 1, "", ""), // 4.0
        ExamResult::fromDatabase(2, 1, 101, "final", 70.0, 100.0, "B", 1, "", "")   // 3.0
    };
    
    EXPECT_CALL(*mockRepo_, findByStudentAndSemester(1, 1))
        .WillOnce(Return(Result<std::vector<ExamResult>>(results)));
        
    auto res = service_->calculateSgpa(1, 1);
    ASSERT_TRUE(isSuccess(res));
    auto summary = getValue(res);
    EXPECT_EQ(summary.studentId, 1);
    EXPECT_EQ(summary.semester, 1);
    EXPECT_DOUBLE_EQ(summary.gpa, 3.5); // (4.0 + 3.0) / 2
    EXPECT_EQ(summary.totalExams, 2);
}

TEST_F(ResultServiceTest, CalculateCgpaComputesCorrectGpa) {
    std::vector<ExamResult> results = {
        ExamResult::fromDatabase(1, 1, 100, "final", 90.0, 100.0, "A+", 1, "", ""), // 4.0
        ExamResult::fromDatabase(2, 1, 101, "final", 70.0, 100.0, "B", 1, "", ""),  // 3.0
        ExamResult::fromDatabase(3, 1, 102, "final", 85.0, 100.0, "A", 2, "", "")   // 4.0
    };
    
    EXPECT_CALL(*mockRepo_, findByStudentId(1))
        .WillOnce(Return(Result<std::vector<ExamResult>>(results)));
        
    auto res = service_->calculateCgpa(1);
    ASSERT_TRUE(isSuccess(res));
    auto summary = getValue(res);
    EXPECT_EQ(summary.studentId, 1);
    EXPECT_EQ(summary.semester, 0); // 0 for CGPA
    EXPECT_DOUBLE_EQ(summary.gpa, (4.0 + 3.0 + 4.0) / 3.0);
    EXPECT_EQ(summary.totalExams, 3);
}
