#include <gtest/gtest.h>
#include <memory>
#include "common/types.hpp"
#include "common/errors.hpp"
#include "enrollment/repositories/sqlite_enrollment_repository.hpp"
#include "test_helpers/test_db.hpp"

using namespace ums;
using namespace ums::test;

// WHY: Validates SQLite implementation of IEnrollmentRepository against an in-memory DB.
// Ensures constraints (like unique student-course pairs) and queries work correctly.
class EnrollmentRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // We use the shared TestDatabase which already creates the 'users' table
        
        // Setup dummy tables to satisfy foreign key constraints if enforced,
        // though SQLite doesn't enforce FKs by default unless PRAGMA foreign_keys = ON.
        // We will create the necessary tables just in case.
        testDb_.db().exec(
            "CREATE TABLE IF NOT EXISTS departments (id INTEGER PRIMARY KEY, name TEXT);"
            "CREATE TABLE IF NOT EXISTS students (id INTEGER PRIMARY KEY, user_id INTEGER, roll_number TEXT, department_id INTEGER, first_name TEXT, last_name TEXT, semester INTEGER);"
            "CREATE TABLE IF NOT EXISTS courses (id INTEGER PRIMARY KEY, department_id INTEGER, code TEXT, name TEXT, credits INTEGER);"
        );
        
        repo_ = std::make_unique<SQLiteEnrollmentRepository>(testDb_.db());
        repo_->initialize();
        
        // Insert dummy data
        testDb_.db().exec("INSERT INTO students (id, user_id, roll_number) VALUES (1, 1, 'R1'), (2, 2, 'R2');");
        testDb_.db().exec("INSERT INTO courses (id, code, name) VALUES (101, 'C1', 'Course 1'), (102, 'C2', 'Course 2');");
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteEnrollmentRepository> repo_;
};

// Test create enrollment succeeds
TEST_F(EnrollmentRepositoryTest, CreateEnrollmentSucceeds) {
    auto result = repo_->create(1, 101);
    ASSERT_TRUE(isSuccess(result));
    
    const Enrollment& enrollment = getValue(result);
    EXPECT_GT(enrollment.id(), 0);
    EXPECT_EQ(enrollment.studentId(), 1);
    EXPECT_EQ(enrollment.courseId(), 101);
}

// Test create duplicate enrollment returns Conflict
TEST_F(EnrollmentRepositoryTest, CreateDuplicateEnrollmentReturnsConflict) {
    ASSERT_TRUE(isSuccess(repo_->create(1, 101)));
    
    auto result = repo_->create(1, 101);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test findById returns existing enrollment
TEST_F(EnrollmentRepositoryTest, FindByIdReturns) {
    auto createResult = repo_->create(1, 101);
    int id = getValue(createResult).id();
    
    auto findResult = repo_->findById(id);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).studentId(), 1);
    EXPECT_EQ(getValue(findResult).courseId(), 101);
}

// Test findById on missing returns NotFound
TEST_F(EnrollmentRepositoryTest, FindByIdMissingReturnsNotFound) {
    auto findResult = repo_->findById(999);
    ASSERT_FALSE(isSuccess(findResult));
    EXPECT_EQ(getError(findResult).code, ErrorCode::NotFound);
}

// Test findByStudentId returns list
TEST_F(EnrollmentRepositoryTest, FindByStudentIdReturns) {
    repo_->create(1, 101);
    repo_->create(1, 102);
    repo_->create(2, 101);
    
    auto findResult = repo_->findByStudentId(1);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).size(), 2);
}

// Test findByCourseId returns list
TEST_F(EnrollmentRepositoryTest, FindByCourseIdReturns) {
    repo_->create(1, 101);
    repo_->create(2, 101);
    
    auto findResult = repo_->findByCourseId(101);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).size(), 2);
}

// Test deleteByStudentAndCourse succeeds
TEST_F(EnrollmentRepositoryTest, DeleteByStudentAndCourseSucceeds) {
    repo_->create(1, 101);
    
    auto deleteResult = repo_->deleteByStudentAndCourse(1, 101);
    ASSERT_TRUE(isSuccess(deleteResult));
    EXPECT_TRUE(getValue(deleteResult));
    
    auto existsResult = repo_->existsByStudentAndCourse(1, 101);
    EXPECT_FALSE(getValue(existsResult));
}

// Test existsByStudentAndCourse returns true when exists
TEST_F(EnrollmentRepositoryTest, ExistsByStudentAndCourseReturnsTrue) {
    repo_->create(2, 102);
    
    auto result = repo_->existsByStudentAndCourse(2, 102);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_TRUE(getValue(result));
    
    auto resultFalse = repo_->existsByStudentAndCourse(1, 102);
    ASSERT_TRUE(isSuccess(resultFalse));
    EXPECT_FALSE(getValue(resultFalse));
}
