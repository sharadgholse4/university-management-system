#include <gtest/gtest.h>
#include <memory>
#include "attendance/repositories/sqlite_attendance_repository.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "student/repositories/sqlite_student_repository.hpp"
#include "test_helpers/test_db.hpp"
#include "common/errors.hpp"

namespace ums { namespace test {

class AttendanceRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // The repository initialization must occur in sequence because of foreign keys
        userRepo_ = std::make_unique<SQLiteUserRepository>(testDb_.db());
        userRepo_->initialize();
        studentRepo_ = std::make_unique<SQLiteStudentRepository>(testDb_.db());
        studentRepo_->initialize();
        repo_ = std::make_unique<SQLiteAttendanceRepository>(testDb_.db());
        repo_->initialize();
        
        // Create test users and students
        userRepo_->create("prof@example.com", "hash", Role::Professor);  // User ID 1
        userRepo_->create("student1@example.com", "hash", Role::Student); // User ID 2
        studentRepo_->create(2, "CS2023001", "John", "Doe", 1, 3); // Student ID 1
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteStudentRepository> studentRepo_;
    std::unique_ptr<SQLiteAttendanceRepository> repo_;
};

TEST_F(AttendanceRepositoryTest, CreateAttendanceSucceeds) {
    auto result = repo_->create(1, 101, "2024-01-15", "present", 1);
    ASSERT_TRUE(isSuccess(result));
    
    auto attendance = getValue(result);
    EXPECT_EQ(attendance.studentId(), 1);
    EXPECT_EQ(attendance.courseId(), 101);
    EXPECT_EQ(attendance.date(), "2024-01-15");
    EXPECT_EQ(attendance.status(), "present");
    EXPECT_EQ(attendance.markedBy(), 1);
}

TEST_F(AttendanceRepositoryTest, CreateDuplicateStudentCourseDateReturnsConflict) {
    auto result1 = repo_->create(1, 101, "2024-01-15", "present", 1);
    ASSERT_TRUE(isSuccess(result1));
    
    auto result2 = repo_->create(1, 101, "2024-01-15", "late", 1);
    ASSERT_FALSE(isSuccess(result2));
    EXPECT_EQ(getError(result2).code, ErrorCode::Conflict);
}

TEST_F(AttendanceRepositoryTest, FindByIdExistingReturns) {
    auto created = getValue(repo_->create(1, 101, "2024-01-15", "present", 1));
    
    auto result = repo_->findById(created.id());
    ASSERT_TRUE(isSuccess(result));
    
    auto fetched = getValue(result);
    EXPECT_EQ(fetched.id(), created.id());
    EXPECT_EQ(fetched.status(), "present");
}

TEST_F(AttendanceRepositoryTest, FindByIdNonExistentReturnsNotFound) {
    auto result = repo_->findById(999);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::NotFound);
}

TEST_F(AttendanceRepositoryTest, FindByStudentIdReturns) {
    repo_->create(1, 101, "2024-01-15", "present", 1);
    repo_->create(1, 102, "2024-01-15", "absent", 1);
    
    auto result = repo_->findByStudentId(1);
    ASSERT_TRUE(isSuccess(result));
    
    auto vec = getValue(result);
    EXPECT_EQ(vec.size(), 2);
}

TEST_F(AttendanceRepositoryTest, FindByCourseIdReturns) {
    // Need another student to test properly
    userRepo_->create("student2@example.com", "hash", Role::Student); // User ID 3
    studentRepo_->create(3, "CS2023002", "Jane", "Doe", 1, 3); // Student ID 2
    
    repo_->create(1, 101, "2024-01-15", "present", 1);
    repo_->create(2, 101, "2024-01-15", "absent", 1);
    
    auto result = repo_->findByCourseId(101);
    ASSERT_TRUE(isSuccess(result));
    
    auto vec = getValue(result);
    EXPECT_EQ(vec.size(), 2);
}

TEST_F(AttendanceRepositoryTest, FindByStudentAndCourseReturns) {
    repo_->create(1, 101, "2024-01-15", "present", 1);
    repo_->create(1, 101, "2024-01-16", "late", 1);
    
    auto result = repo_->findByStudentAndCourse(1, 101);
    ASSERT_TRUE(isSuccess(result));
    
    auto vec = getValue(result);
    EXPECT_EQ(vec.size(), 2);
}

TEST_F(AttendanceRepositoryTest, FindByCourseAndDateReturns) {
    userRepo_->create("student2@example.com", "hash", Role::Student); // User ID 3
    studentRepo_->create(3, "CS2023002", "Jane", "Doe", 1, 3); // Student ID 2
    
    repo_->create(1, 101, "2024-01-15", "present", 1);
    repo_->create(2, 101, "2024-01-15", "late", 1);
    repo_->create(1, 101, "2024-01-16", "absent", 1);
    
    auto result = repo_->findByCourseAndDate(101, "2024-01-15");
    ASSERT_TRUE(isSuccess(result));
    
    auto vec = getValue(result);
    EXPECT_EQ(vec.size(), 2);
}

TEST_F(AttendanceRepositoryTest, UpdateAttendanceSucceeds) {
    auto created = getValue(repo_->create(1, 101, "2024-01-15", "present", 1));
    
    auto result = repo_->update(created.id(), "absent");
    ASSERT_TRUE(isSuccess(result));
    
    auto updated = getValue(result);
    EXPECT_EQ(updated.status(), "absent");
    
    // Verify in db
    auto fetched = getValue(repo_->findById(created.id()));
    EXPECT_EQ(fetched.status(), "absent");
}

TEST_F(AttendanceRepositoryTest, ExistsByStudentCourseDateReturnsTrue) {
    repo_->create(1, 101, "2024-01-15", "present", 1);
    
    auto result = repo_->existsByStudentCourseDate(1, 101, "2024-01-15");
    ASSERT_TRUE(isSuccess(result));
    EXPECT_TRUE(getValue(result));
}

TEST_F(AttendanceRepositoryTest, ExistsByStudentCourseDateReturnsFalse) {
    auto result = repo_->existsByStudentCourseDate(1, 101, "2024-01-15");
    ASSERT_TRUE(isSuccess(result));
    EXPECT_FALSE(getValue(result));
}

}} // namespace ums::test
