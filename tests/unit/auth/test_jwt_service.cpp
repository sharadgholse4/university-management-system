#include <gtest/gtest.h>
#include "auth/services/jwt_service.hpp"
#include "common/types.hpp"
#include "common/errors.hpp"
#include <thread>
#include <chrono>

using namespace ums;

// Test creating a token succeeds (non-empty string)
TEST(JwtServiceTest, CreateTokenSucceeds) {
    JwtService service("my-secret", 3600);
    JwtPayload payload{1, Role::Student, 1};
    std::string token = service.createToken(payload);
    
    EXPECT_FALSE(token.empty());
}

// Test verifying a valid token returns correct payload
TEST(JwtServiceTest, VerifyValidTokenReturnsCorrectPayload) {
    JwtService service("my-secret", 3600);
    JwtPayload payload{1, Role::Student, 1};
    std::string token = service.createToken(payload);
    
    auto result = service.verifyToken(token);
    ASSERT_TRUE(isSuccess(result));
    const auto& decoded = getValue(result);
    
    EXPECT_EQ(decoded.userId, 1);
    EXPECT_EQ(decoded.role, Role::Student);
    EXPECT_EQ(decoded.subjectId, 1);
}

// Test verifying with wrong secret fails
TEST(JwtServiceTest, VerifyWithWrongSecretFails) {
    JwtService service1("my-secret", 3600);
    JwtService service2("wrong-secret", 3600);
    JwtPayload payload{1, Role::Student, 1};
    std::string token = service1.createToken(payload);
    
    auto result = service2.verifyToken(token);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Unauthorized);
}

// Test verifying an expired token fails
TEST(JwtServiceTest, VerifyExpiredTokenFails) {
    JwtService service("my-secret", -1); // Negative expiry makes it instantly expired
    JwtPayload payload{1, Role::Student, 1};
    std::string token = service.createToken(payload);
    
    auto result = service.verifyToken(token);
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Unauthorized);
}

// Test verifying a garbage string fails
TEST(JwtServiceTest, VerifyGarbageStringFails) {
    JwtService service("my-secret", 3600);
    auto result = service.verifyToken("not.a.valid.jwt");
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Unauthorized);
}

// Test verifying an empty string fails
TEST(JwtServiceTest, VerifyEmptyStringFails) {
    JwtService service("my-secret", 3600);
    auto result = service.verifyToken("");
    EXPECT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Unauthorized);
}

// Test payload fields (userId, role, subjectId) round-trip correctly
TEST(JwtServiceTest, PayloadFieldsRoundTripCorrectly) {
    JwtService service("my-secret", 3600);
    JwtPayload payload{42, Role::Admin, 99};
    std::string token = service.createToken(payload);
    
    auto result = service.verifyToken(token);
    ASSERT_TRUE(isSuccess(result));
    
    const auto& decoded = getValue(result);
    EXPECT_EQ(decoded.userId, 42);
    EXPECT_EQ(decoded.role, Role::Admin);
    EXPECT_EQ(decoded.subjectId, 99);
}

// Test token with different roles round-trips correctly
TEST(JwtServiceTest, DifferentRolesRoundTrip) {
    JwtService service("my-secret", 3600);
    
    JwtPayload profPayload{1, Role::Professor, 1};
    auto profResult = service.verifyToken(service.createToken(profPayload));
    ASSERT_TRUE(isSuccess(profResult));
    EXPECT_EQ(getValue(profResult).role, Role::Professor);
    
    JwtPayload adminPayload{2, Role::Admin, 2};
    auto adminResult = service.verifyToken(service.createToken(adminPayload));
    ASSERT_TRUE(isSuccess(adminResult));
    EXPECT_EQ(getValue(adminResult).role, Role::Admin);
}
