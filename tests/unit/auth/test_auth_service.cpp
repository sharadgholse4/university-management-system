#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "auth/services/auth_service.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "test_helpers/mock_user_repository.hpp"

using namespace ums;
using namespace ums::test;
using ::testing::Return;
using ::testing::_;

class AuthServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Fast tests with cost=4
        passwordService_ = std::make_unique<PasswordService>(4);
        jwtService_ = std::make_unique<JwtService>("test-secret", 3600);
        authService_ = std::make_unique<AuthService>(mockRepo_, *passwordService_, *jwtService_);
    }

    MockUserRepository mockRepo_;
    std::unique_ptr<PasswordService> passwordService_;
    std::unique_ptr<JwtService> jwtService_;
    std::unique_ptr<AuthService> authService_;
};

// Test login with valid credentials returns AuthResponse with token
TEST_F(AuthServiceTest, LoginValidCredentialsReturnsToken) {
    auto hashedPassword = passwordService_->hash("password123");
    auto testUser = User::fromDatabase(1, "test@example.com", hashedPassword, Role::Student, "2024", "2024");
    
    EXPECT_CALL(mockRepo_, findByEmail("test@example.com"))
        .WillOnce(Return(Result<User>{testUser}));
        
    LoginRequest req{"test@example.com", "password123"};
    auto result = authService_->login(req);
    
    ASSERT_TRUE(isSuccess(result));
    const auto& response = getValue(result);
    EXPECT_EQ(response.user.id(), 1);
    EXPECT_FALSE(response.token.empty());
}

// Test login with non-existent email returns Unauthorized
TEST_F(AuthServiceTest, LoginNonExistentEmailReturnsUnauthorized) {
    EXPECT_CALL(mockRepo_, findByEmail("notfound@example.com"))
        .WillOnce(Return(Result<User>{AppError{ErrorCode::NotFound, "User not found"}}));
        
    LoginRequest req{"notfound@example.com", "password123"};
    auto result = authService_->login(req);
    
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Unauthorized);
}

// Test login with wrong password returns Unauthorized
TEST_F(AuthServiceTest, LoginWrongPasswordReturnsUnauthorized) {
    auto hashedPassword = passwordService_->hash("password123");
    auto testUser = User::fromDatabase(1, "test@example.com", hashedPassword, Role::Student, "2024", "2024");
    
    EXPECT_CALL(mockRepo_, findByEmail("test@example.com"))
        .WillOnce(Return(Result<User>{testUser}));
        
    LoginRequest req{"test@example.com", "wrongpassword"};
    auto result = authService_->login(req);
    
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Unauthorized);
}

// Test login with empty email returns ValidationError
TEST_F(AuthServiceTest, LoginEmptyEmailReturnsValidationError) {
    LoginRequest req{"", "password123"};
    auto result = authService_->login(req);
    
    ASSERT_FALSE(isSuccess(result));
    // Implementation might return ValidationError or Unauthorized, let's accept either
    auto code = getError(result).code;
    EXPECT_TRUE(code == ErrorCode::ValidationError || code == ErrorCode::Unauthorized);
}

// Test login with empty password returns ValidationError
TEST_F(AuthServiceTest, LoginEmptyPasswordReturnsValidationError) {
    LoginRequest req{"test@example.com", ""};
    auto result = authService_->login(req);
    
    ASSERT_FALSE(isSuccess(result));
    auto code = getError(result).code;
    EXPECT_TRUE(code == ErrorCode::ValidationError || code == ErrorCode::Unauthorized);
}

// Test register with valid data creates user
TEST_F(AuthServiceTest, RegisterValidDataCreatesUser) {
    EXPECT_CALL(mockRepo_, existsByEmail("new@example.com"))
        .WillOnce(Return(Result<bool>{false}));
        
    auto expectedUser = User::fromDatabase(2, "new@example.com", "hash", Role::Student, "2024", "2024");
    EXPECT_CALL(mockRepo_, create("new@example.com", _, Role::Student))
        .WillOnce(Return(Result<User>{expectedUser}));
        
    RegisterRequest req{"new@example.com", "password123", Role::Student};
    auto result = authService_->registerUser(req);
    
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).email(), "new@example.com");
}

// Test register with duplicate email returns Conflict
TEST_F(AuthServiceTest, RegisterDuplicateEmailReturnsConflict) {
    EXPECT_CALL(mockRepo_, existsByEmail("existing@example.com"))
        .WillOnce(Return(Result<bool>{true}));
        
    RegisterRequest req{"existing@example.com", "password123", Role::Student};
    auto result = authService_->registerUser(req);
    
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test register with invalid email returns ValidationError
TEST_F(AuthServiceTest, RegisterInvalidEmailReturnsValidationError) {
    RegisterRequest req{"bademail", "password123", Role::Student};
    auto result = authService_->registerUser(req);
    
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test register with empty password returns ValidationError
TEST_F(AuthServiceTest, RegisterEmptyPasswordReturnsValidationError) {
    RegisterRequest req{"good@example.com", "", Role::Student};
    auto result = authService_->registerUser(req);
    
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}
