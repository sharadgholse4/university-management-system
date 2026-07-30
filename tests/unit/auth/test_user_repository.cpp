#include <gtest/gtest.h>
#include "auth/repositories/sqlite_user_repository.hpp"
#include "test_helpers/test_db.hpp"

using namespace ums;
using namespace ums::test;

class UserRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // testDb_ automatically creates the table on instantiation
        repo_ = std::make_unique<SQLiteUserRepository>(testDb_.db());
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteUserRepository> repo_;
};

// Test create user succeeds and returns user with ID
TEST_F(UserRepositoryTest, CreateUserSucceeds) {
    auto result = repo_->create("test@example.com", "hash123", Role::Student);
    ASSERT_TRUE(isSuccess(result));
    
    const User& user = getValue(result);
    EXPECT_GT(user.id(), 0);
    EXPECT_EQ(user.email(), "test@example.com");
    EXPECT_EQ(user.passwordHash(), "hash123");
    EXPECT_EQ(user.role(), Role::Student);
}

// Test create user with duplicate email returns Conflict error
TEST_F(UserRepositoryTest, CreateUserDuplicateEmailConflict) {
    auto result1 = repo_->create("test@example.com", "hash123", Role::Student);
    ASSERT_TRUE(isSuccess(result1));
    
    auto result2 = repo_->create("test@example.com", "hash456", Role::Admin);
    ASSERT_FALSE(isSuccess(result2));
    EXPECT_EQ(getError(result2).code, ErrorCode::Conflict);
}

// Test findById returns existing user
TEST_F(UserRepositoryTest, FindByIdReturnsExistingUser) {
    auto createResult = repo_->create("findme@example.com", "hash", Role::Professor);
    ASSERT_TRUE(isSuccess(createResult));
    int id = getValue(createResult).id();
    
    auto findResult = repo_->findById(id);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).email(), "findme@example.com");
}

// Test findById with non-existent ID returns NotFound
TEST_F(UserRepositoryTest, FindByIdNonExistentReturnsNotFound) {
    auto findResult = repo_->findById(999);
    ASSERT_FALSE(isSuccess(findResult));
    EXPECT_EQ(getError(findResult).code, ErrorCode::NotFound);
}

// Test findByEmail returns existing user
TEST_F(UserRepositoryTest, FindByEmailReturnsExistingUser) {
    auto createResult = repo_->create("email@example.com", "hash", Role::Student);
    ASSERT_TRUE(isSuccess(createResult));
    
    auto findResult = repo_->findByEmail("email@example.com");
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).id(), getValue(createResult).id());
}

// Test findByEmail with non-existent email returns NotFound
TEST_F(UserRepositoryTest, FindByEmailNonExistentReturnsNotFound) {
    auto findResult = repo_->findByEmail("no_such_email@example.com");
    ASSERT_FALSE(isSuccess(findResult));
    EXPECT_EQ(getError(findResult).code, ErrorCode::NotFound);
}

// Test existsByEmail returns true for existing email
TEST_F(UserRepositoryTest, ExistsByEmailReturnsTrueForExisting) {
    ASSERT_TRUE(isSuccess(repo_->create("exist@example.com", "hash", Role::Admin)));
    
    auto existsResult = repo_->existsByEmail("exist@example.com");
    ASSERT_TRUE(isSuccess(existsResult));
    EXPECT_TRUE(getValue(existsResult));
}

// Test existsByEmail returns false for non-existent email
TEST_F(UserRepositoryTest, ExistsByEmailReturnsFalseForNonExistent) {
    auto existsResult = repo_->existsByEmail("doesnotexist@example.com");
    ASSERT_TRUE(isSuccess(existsResult));
    EXPECT_FALSE(getValue(existsResult));
}

// Test findAll returns all users
TEST_F(UserRepositoryTest, FindAllReturnsAllUsers) {
    repo_->create("user1@example.com", "h1", Role::Student);
    repo_->create("user2@example.com", "h2", Role::Professor);
    repo_->create("user3@example.com", "h3", Role::Admin);
    
    auto findResult = repo_->findAll();
    ASSERT_TRUE(isSuccess(findResult));
    const auto& users = getValue(findResult);
    EXPECT_EQ(users.size(), 3);
}

// Test findAll on empty table returns empty vector
TEST_F(UserRepositoryTest, FindAllOnEmptyTableReturnsEmpty) {
    auto findResult = repo_->findAll();
    ASSERT_TRUE(isSuccess(findResult));
    const auto& users = getValue(findResult);
    EXPECT_TRUE(users.empty());
}
