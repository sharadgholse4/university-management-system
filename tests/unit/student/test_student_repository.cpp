#include <gtest/gtest.h>
#include <memory>
#include "student/repositories/sqlite_student_repository.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "test_helpers/test_db.hpp"

using namespace ums;
using namespace ums::test;

class StudentRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize user repo first to create users table (foreign key dependency)
        userRepo_ = std::make_unique<SQLiteUserRepository>(testDb_.db());
        userRepo_->initialize();
        
        repo_ = std::make_unique<SQLiteStudentRepository>(testDb_.db());
        repo_->initialize();
        
        // Create a few users for our students to reference
        userRepo_->create("student1@example.com", "hash", Role::Student); // ID 1
        userRepo_->create("student2@example.com", "hash", Role::Student); // ID 2
        userRepo_->create("student3@example.com", "hash", Role::Student); // ID 3
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteStudentRepository> repo_;
};

// Test create student succeeds
TEST_F(StudentRepositoryTest, CreateStudentSucceeds) {
    auto result = repo_->create(1, "CS2023001", "John", "Doe", 1, 3);
    ASSERT_TRUE(isSuccess(result));
    
    const Student& student = getValue(result);
    EXPECT_GT(student.id(), 0);
    EXPECT_EQ(student.userId(), 1);
    EXPECT_EQ(student.rollNumber(), "CS2023001");
    EXPECT_EQ(student.firstName(), "John");
}

// Test create with duplicate user ID returns conflict
TEST_F(StudentRepositoryTest, CreateDuplicateUserIdReturnsConflict) {
    ASSERT_TRUE(isSuccess(repo_->create(1, "CS2023001", "John", "Doe", 1, 3)));
    
    auto result = repo_->create(1, "CS2023002", "Jane", "Doe", 1, 3);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test create with duplicate roll number returns conflict
TEST_F(StudentRepositoryTest, CreateDuplicateRollNumberReturnsConflict) {
    ASSERT_TRUE(isSuccess(repo_->create(1, "CS2023001", "John", "Doe", 1, 3)));
    
    auto result = repo_->create(2, "CS2023001", "Jane", "Doe", 1, 3);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test findById existing returns student
TEST_F(StudentRepositoryTest, FindByIdExistingReturns) {
    auto createResult = repo_->create(1, "CS2023001", "John", "Doe", 1, 3);
    int id = getValue(createResult).id();
    
    auto findResult = repo_->findById(id);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).rollNumber(), "CS2023001");
}

// Test findById non-existent returns 404 (NotFound)
TEST_F(StudentRepositoryTest, FindByIdNonExistentReturns404) {
    auto findResult = repo_->findById(999);
    ASSERT_FALSE(isSuccess(findResult));
    EXPECT_EQ(getError(findResult).code, ErrorCode::NotFound);
}

// Test findByUserId returns student
TEST_F(StudentRepositoryTest, FindByUserIdReturns) {
    repo_->create(2, "CS2023002", "Jane", "Doe", 1, 3);
    
    auto findResult = repo_->findByUserId(2);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).rollNumber(), "CS2023002");
}

// Test findAll returns all
TEST_F(StudentRepositoryTest, FindAllReturnsAll) {
    repo_->create(1, "CS2023001", "John", "Doe", 1, 3);
    repo_->create(2, "CS2023002", "Jane", "Doe", 2, 4);
    
    auto findResult = repo_->findAll();
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).size(), 2);
}

// Test update student succeeds
TEST_F(StudentRepositoryTest, UpdateStudentSucceeds) {
    auto createResult = repo_->create(1, "CS2023001", "John", "Doe", 1, 3);
    int id = getValue(createResult).id();
    
    auto updateResult = repo_->update(id, "Johnny", "Doee", 2, 4);
    ASSERT_TRUE(isSuccess(updateResult));
    
    auto findResult = repo_->findById(id);
    EXPECT_EQ(getValue(findResult).firstName(), "Johnny");
    EXPECT_EQ(getValue(findResult).lastName(), "Doee");
    EXPECT_EQ(getValue(findResult).departmentId(), 2);
    EXPECT_EQ(getValue(findResult).semester(), 4);
}

// Test findByDepartment returns
TEST_F(StudentRepositoryTest, FindByDepartmentReturns) {
    repo_->create(1, "CS2023001", "John", "Doe", 1, 3);
    repo_->create(2, "CS2023002", "Jane", "Doe", 1, 4);
    repo_->create(3, "CS2023003", "Bob", "Smith", 2, 3);
    
    auto findResult = repo_->findByDepartment(1);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).size(), 2);
}

// Test existsByUserId returns true for existing
TEST_F(StudentRepositoryTest, ExistsByUserIdReturnsTrueForExisting) {
    repo_->create(1, "CS2023001", "John", "Doe", 1, 3);
    
    auto result = repo_->existsByUserId(1);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_TRUE(getValue(result));
    
    auto resultFalse = repo_->existsByUserId(2);
    ASSERT_TRUE(isSuccess(resultFalse));
    EXPECT_FALSE(getValue(resultFalse));
}
