#include <gtest/gtest.h>
#include "auth/models/user.hpp"
#include "common/types.hpp"
#include "common/errors.hpp"
#include "crow.h"

using namespace ums;

// Test creating a valid user with create() succeeds
TEST(UserModelTest, CreateWithValidEmailSucceeds) {
    auto result = User::create("test@example.com", "hashed_password", Role::Student);
    EXPECT_TRUE(isSuccess(result));
    const User& user = getValue(result);
    EXPECT_EQ(user.email(), "test@example.com");
    EXPECT_EQ(user.passwordHash(), "hashed_password");
    EXPECT_EQ(user.role(), Role::Student);
}

// Test creating with empty email returns ValidationError
TEST(UserModelTest, CreateWithEmptyEmailReturnsValidationError) {
    auto result = User::create("", "hashed_password", Role::Student);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with invalid email (no @) returns ValidationError
TEST(UserModelTest, CreateWithInvalidEmailNoAtReturnsValidationError) {
    auto result = User::create("testexample.com", "hashed_password", Role::Student);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with invalid email (no domain) returns ValidationError
TEST(UserModelTest, CreateWithInvalidEmailNoDomainReturnsValidationError) {
    auto result = User::create("test@", "hashed_password", Role::Student);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with empty password hash returns ValidationError
TEST(UserModelTest, CreateWithEmptyPasswordHashReturnsValidationError) {
    auto result = User::create("test@example.com", "", Role::Student);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test creating with each Role type works
TEST(UserModelTest, CreateWithAllRolesSucceeds) {
    EXPECT_TRUE(isSuccess(User::create("student@example.com", "hash", Role::Student)));
    EXPECT_TRUE(isSuccess(User::create("professor@example.com", "hash", Role::Professor)));
    EXPECT_TRUE(isSuccess(User::create("admin@example.com", "hash", Role::Admin)));
}

// Test fromDatabase constructs correctly with all fields
TEST(UserModelTest, FromDatabaseConstructsCorrectly) {
    User user = User::fromDatabase(1, "test@example.com", "hash", Role::Student, "2024-01-01", "2024-01-02");
    EXPECT_EQ(user.id(), 1);
    EXPECT_EQ(user.email(), "test@example.com");
    EXPECT_EQ(user.passwordHash(), "hash");
    EXPECT_EQ(user.role(), Role::Student);
    EXPECT_EQ(user.createdAt(), "2024-01-01");
    EXPECT_EQ(user.updatedAt(), "2024-01-02");
}

// Test toJson() never includes passwordHash
TEST(UserModelTest, ToJsonExcludesPasswordHash) {
    User user = User::fromDatabase(1, "test@example.com", "super_secret_hash", Role::Student, "2024-01-01", "2024-01-02");
    auto json = user.toJson();
    std::string jsonString = json.dump();
    EXPECT_EQ(jsonString.find("super_secret_hash"), std::string::npos);
    EXPECT_EQ(jsonString.find("passwordHash"), std::string::npos);
}

// Test toJson() includes all public fields
TEST(UserModelTest, ToJsonIncludesPublicFields) {
    User user = User::fromDatabase(1, "test@example.com", "hash", Role::Student, "2024-01-01", "2024-01-02");
    auto json = user.toJson();
    std::string jsonString = json.dump();
    EXPECT_NE(jsonString.find("1"), std::string::npos); // id
    EXPECT_NE(jsonString.find("test@example.com"), std::string::npos); // email
    EXPECT_NE(jsonString.find("student"), std::string::npos); // role
    EXPECT_NE(jsonString.find("2024-01-01"), std::string::npos); // createdAt
}

// Test getters return correct values
TEST(UserModelTest, GettersReturnCorrectValues) {
    User user = User::fromDatabase(5, "abc@def.com", "h", Role::Admin, "c", "u");
    EXPECT_EQ(user.id(), 5);
    EXPECT_EQ(user.email(), "abc@def.com");
    EXPECT_EQ(user.passwordHash(), "h");
    EXPECT_EQ(user.role(), Role::Admin);
    EXPECT_EQ(user.createdAt(), "c");
    EXPECT_EQ(user.updatedAt(), "u");
}

// Test roleToString and stringToRole work correctly
TEST(UserModelTest, RoleConversionsWork) {
    EXPECT_EQ(roleToString(Role::Student), "student");
    EXPECT_EQ(roleToString(Role::Professor), "professor");
    EXPECT_EQ(roleToString(Role::Admin), "admin");
    
    EXPECT_EQ(stringToRole("student"), Role::Student);
    EXPECT_EQ(stringToRole("professor"), Role::Professor);
    EXPECT_EQ(stringToRole("admin"), Role::Admin);
}

// Test stringToRole with invalid string throws
TEST(UserModelTest, StringToRoleInvalidThrows) {
    EXPECT_THROW(stringToRole("invalid_role"), std::invalid_argument);
    EXPECT_THROW(stringToRole(""), std::invalid_argument);
}
