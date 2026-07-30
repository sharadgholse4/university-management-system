#include <gtest/gtest.h>
#include <string>
#include "results/models/result.hpp"
#include "common/types.hpp"
#include "common/errors.hpp"

using namespace ums;

class ResultModelTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(ResultModelTest, CreateWithValidDataSucceeds) {
    auto res = ExamResult::create(1, 2, "final", 85.0, 100.0, 1);
    ASSERT_TRUE(isSuccess(res));
    auto examRes = getValue(res);
    EXPECT_EQ(examRes.studentId(), 1);
    EXPECT_EQ(examRes.courseId(), 2);
    EXPECT_EQ(examRes.examType(), "final");
    EXPECT_DOUBLE_EQ(examRes.marksObtained(), 85.0);
    EXPECT_DOUBLE_EQ(examRes.maxMarks(), 100.0);
    EXPECT_EQ(examRes.semester(), 1);
    EXPECT_EQ(examRes.grade(), "A");
}

TEST_F(ResultModelTest, CreateWithInvalidStudentIdReturnsError) {
    auto res = ExamResult::create(0, 2, "final", 85.0, 100.0, 1);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
}

TEST_F(ResultModelTest, CreateWithInvalidCourseIdReturnsError) {
    auto res = ExamResult::create(1, -1, "final", 85.0, 100.0, 1);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
}

TEST_F(ResultModelTest, CreateWithInvalidExamTypeReturnsError) {
    auto res = ExamResult::create(1, 2, "invalid", 85.0, 100.0, 1);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
}

TEST_F(ResultModelTest, CreateWithNegativeMarksObtainedReturnsError) {
    auto res = ExamResult::create(1, 2, "final", -1.0, 100.0, 1);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
}

TEST_F(ResultModelTest, CreateWithZeroMaxMarksReturnsError) {
    auto res = ExamResult::create(1, 2, "final", 85.0, 0.0, 1);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
}

TEST_F(ResultModelTest, CreateWithMarksExceedingMaxReturnsError) {
    auto res = ExamResult::create(1, 2, "final", 105.0, 100.0, 1);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
}

TEST_F(ResultModelTest, CreateWithSemesterOutOfBoundsReturnsError) {
    auto res = ExamResult::create(1, 2, "final", 85.0, 100.0, 0);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::ValidationError);
    
    auto res2 = ExamResult::create(1, 2, "final", 85.0, 100.0, 9);
    ASSERT_FALSE(isSuccess(res2));
    EXPECT_EQ(getError(res2).code, ErrorCode::ValidationError);
}

TEST_F(ResultModelTest, CalculateGradeReturnsCorrectGrade) {
    EXPECT_EQ(calculateGrade(95, 100), "A+");
    EXPECT_EQ(calculateGrade(85, 100), "A");
    EXPECT_EQ(calculateGrade(77, 100), "B+");
    EXPECT_EQ(calculateGrade(72, 100), "B");
    EXPECT_EQ(calculateGrade(67, 100), "C+");
    EXPECT_EQ(calculateGrade(62, 100), "C");
    EXPECT_EQ(calculateGrade(55, 100), "D");
    EXPECT_EQ(calculateGrade(45, 100), "F");
}

TEST_F(ResultModelTest, GradeToPointsReturnsCorrectPoints) {
    EXPECT_DOUBLE_EQ(gradeToPoints("A+"), 4.0);
    EXPECT_DOUBLE_EQ(gradeToPoints("B+"), 3.5);
    EXPECT_DOUBLE_EQ(gradeToPoints("C"), 2.0);
    EXPECT_DOUBLE_EQ(gradeToPoints("F"), 0.0);
    EXPECT_DOUBLE_EQ(gradeToPoints("Unknown"), 0.0);
}

TEST_F(ResultModelTest, FromDatabaseConstructsCorrectly) {
    auto res = ExamResult::fromDatabase(10, 1, 2, "midterm", 40.0, 50.0, "A", 2, "2023-01-01", "2023-01-02");
    EXPECT_EQ(res.id(), 10);
    EXPECT_EQ(res.studentId(), 1);
    EXPECT_EQ(res.courseId(), 2);
    EXPECT_EQ(res.examType(), "midterm");
    EXPECT_DOUBLE_EQ(res.marksObtained(), 40.0);
    EXPECT_DOUBLE_EQ(res.maxMarks(), 50.0);
    EXPECT_EQ(res.grade(), "A");
    EXPECT_EQ(res.semester(), 2);
    EXPECT_EQ(res.createdAt(), "2023-01-01");
    EXPECT_EQ(res.updatedAt(), "2023-01-02");
}

TEST_F(ResultModelTest, ToJsonIncludesAllFields) {
    auto res = ExamResult::fromDatabase(10, 1, 2, "midterm", 40.0, 50.0, "A", 2, "2023-01-01", "2023-01-02");
    auto json = res.toJson();
    auto parsed = crow::json::load(json.dump());
    ASSERT_TRUE(parsed);
    EXPECT_EQ(parsed["id"].i(), 10);
    EXPECT_EQ(parsed["studentId"].i(), 1);
    EXPECT_EQ(parsed["courseId"].i(), 2);
    EXPECT_EQ(std::string(parsed["examType"].s()), "midterm");
    EXPECT_DOUBLE_EQ(parsed["marksObtained"].d(), 40.0);
    EXPECT_DOUBLE_EQ(parsed["maxMarks"].d(), 50.0);
    EXPECT_EQ(std::string(parsed["grade"].s()), "A");
    EXPECT_EQ(parsed["semester"].i(), 2);
    EXPECT_DOUBLE_EQ(parsed["percentage"].d(), 80.0);
    EXPECT_DOUBLE_EQ(parsed["gradePoints"].d(), 4.0);
}

TEST_F(ResultModelTest, PercentageCalculationCorrect) {
    auto res = ExamResult::fromDatabase(10, 1, 2, "midterm", 37.5, 50.0, "A", 2, "", "");
    EXPECT_DOUBLE_EQ(res.percentage(), 75.0);
}

TEST_F(ResultModelTest, ExamTypeToStringAndFromStringWork) {
    EXPECT_EQ(examTypeToString(ExamType::Midterm), "midterm");
    EXPECT_EQ(examTypeToString(ExamType::Final), "final");
    EXPECT_EQ(examTypeToString(ExamType::Assignment), "assignment");
    EXPECT_EQ(examTypeToString(ExamType::Quiz), "quiz");
    
    auto t1 = stringToExamType("midterm");
    ASSERT_TRUE(isSuccess(t1));
    EXPECT_EQ(getValue(t1), ExamType::Midterm);
    
    auto t2 = stringToExamType("Invalid");
    ASSERT_FALSE(isSuccess(t2));
    EXPECT_EQ(getError(t2).code, ErrorCode::ValidationError);
}
