#include <gtest/gtest.h>
#include <memory>
#include "results/repositories/sqlite_result_repository.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "student/repositories/sqlite_student_repository.hpp"
#include "test_helpers/test_db.hpp"
#include "common/errors.hpp"

namespace ums { namespace test {

class ResultRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        userRepo_ = std::make_unique<SQLiteUserRepository>(testDb_.db());
        userRepo_->initialize();
        studentRepo_ = std::make_unique<SQLiteStudentRepository>(testDb_.db());
        studentRepo_->initialize();
        repo_ = std::make_unique<SQLiteResultRepository>(testDb_.db());
        repo_->initialize();

        userRepo_->create("student1@example.com", "hash", Role::Student); // User ID 1
        studentRepo_->create(1, "CS2023001", "John", "Doe", 1, 1);       // Student ID 1
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteStudentRepository> studentRepo_;
    std::unique_ptr<SQLiteResultRepository> repo_;
};

TEST_F(ResultRepositoryTest, CreateResultSucceeds) {
    auto res = repo_->create(1, 100, "final", 85.0, 100.0, "A", 1);
    ASSERT_TRUE(isSuccess(res));
    auto examRes = getValue(res);
    EXPECT_GT(examRes.id(), 0);
    EXPECT_EQ(examRes.studentId(), 1);
    EXPECT_EQ(examRes.grade(), "A");
}

TEST_F(ResultRepositoryTest, CreateDuplicateStudentCourseExamTypeReturnsConflict) {
    auto res1 = repo_->create(1, 100, "final", 85.0, 100.0, "A", 1);
    ASSERT_TRUE(isSuccess(res1));
    
    auto res2 = repo_->create(1, 100, "final", 90.0, 100.0, "A+", 1);
    ASSERT_FALSE(isSuccess(res2));
    EXPECT_EQ(getError(res2).code, ErrorCode::Conflict);
}

TEST_F(ResultRepositoryTest, FindByIdExistingReturns) {
    auto created = getValue(repo_->create(1, 100, "final", 85.0, 100.0, "A", 1));
    
    auto res = repo_->findById(created.id());
    ASSERT_TRUE(isSuccess(res));
    EXPECT_EQ(getValue(res).id(), created.id());
}

TEST_F(ResultRepositoryTest, FindByIdNonExistentReturnsNotFound) {
    auto res = repo_->findById(999);
    ASSERT_FALSE(isSuccess(res));
    EXPECT_EQ(getError(res).code, ErrorCode::NotFound);
}

TEST_F(ResultRepositoryTest, FindByStudentIdReturns) {
    repo_->create(1, 100, "midterm", 40.0, 50.0, "A", 1);
    repo_->create(1, 101, "final", 80.0, 100.0, "A", 1);
    
    auto res = repo_->findByStudentId(1);
    ASSERT_TRUE(isSuccess(res));
    EXPECT_EQ(getValue(res).size(), 2);
}

TEST_F(ResultRepositoryTest, FindByStudentAndSemesterReturns) {
    repo_->create(1, 100, "midterm", 40.0, 50.0, "A", 1);
    repo_->create(1, 101, "final", 80.0, 100.0, "A", 2);
    
    auto res = repo_->findByStudentAndSemester(1, 1);
    ASSERT_TRUE(isSuccess(res));
    auto vec = getValue(res);
    ASSERT_EQ(vec.size(), 1);
    EXPECT_EQ(vec[0].semester(), 1);
}

TEST_F(ResultRepositoryTest, FindByCourseIdReturns) {
    userRepo_->create("student2@example.com", "hash", Role::Student); // User ID 2
    studentRepo_->create(2, "CS2023002", "Jane", "Doe", 1, 1);       // Student ID 2

    repo_->create(1, 100, "midterm", 40.0, 50.0, "A", 1);
    repo_->create(2, 100, "midterm", 45.0, 50.0, "A+", 1);
    
    auto res = repo_->findByCourseId(100);
    ASSERT_TRUE(isSuccess(res));
    EXPECT_EQ(getValue(res).size(), 2);
}

TEST_F(ResultRepositoryTest, UpdateResultSucceeds) {
    auto created = getValue(repo_->create(1, 100, "final", 85.0, 100.0, "A", 1));
    
    auto res = repo_->update(created.id(), 95.0, 100.0, "A+");
    ASSERT_TRUE(isSuccess(res));
    
    auto updated = getValue(res);
    EXPECT_DOUBLE_EQ(updated.marksObtained(), 95.0);
    EXPECT_EQ(updated.grade(), "A+");
    
    auto fetched = getValue(repo_->findById(created.id()));
    EXPECT_DOUBLE_EQ(fetched.marksObtained(), 95.0);
    EXPECT_EQ(fetched.grade(), "A+");
}

TEST_F(ResultRepositoryTest, ExistsByStudentCourseExamTypeReturnsTrue) {
    repo_->create(1, 100, "final", 85.0, 100.0, "A", 1);
    
    auto res = repo_->existsByStudentCourseExamType(1, 100, "final");
    ASSERT_TRUE(isSuccess(res));
    EXPECT_TRUE(getValue(res));
    
    auto res2 = repo_->existsByStudentCourseExamType(1, 100, "midterm");
    ASSERT_TRUE(isSuccess(res2));
    EXPECT_FALSE(getValue(res2));
}

}} // namespace ums::test
