#include <gtest/gtest.h>
#include "crow.h"
#include "database/database_manager.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "auth/services/auth_service.hpp"
#include "auth/controllers/auth_controller.hpp"

using namespace ums;

class AuthEndpointTest : public ::testing::Test {
protected:
    void SetUp() override {
        dbManager_ = std::make_unique<DatabaseManager>(":memory:");
        userRepo_ = std::make_unique<SQLiteUserRepository>(dbManager_->db());
        userRepo_->initialize();
        
        // Fast hashing for tests
        passwordService_ = std::make_unique<PasswordService>(4);
        jwtService_ = std::make_unique<JwtService>("test-secret", 3600);
        authService_ = std::make_unique<AuthService>(*userRepo_, *passwordService_, *jwtService_);
        authController_ = std::make_unique<AuthController>(*authService_, *jwtService_);
        
        app_ = std::make_unique<crow::SimpleApp>();
        authController_->registerRoutes(*app_);
        app_->validate();
    }

    // Helper to make a request and get response
    crow::response makeRequest(const std::string& method, const std::string& url, 
                                const std::string& body = "", 
                                const std::string& authToken = "") {
        crow::request req;
        req.method = method == "POST" ? crow::HTTPMethod::POST : crow::HTTPMethod::GET;
        req.url = url;
        req.body = body;
        if (!body.empty()) {
            req.add_header("Content-Type", "application/json");
        }
        if (!authToken.empty()) {
            req.add_header("Authorization", "Bearer " + authToken);
        }
        
        crow::response res;
        app_->handle(req, res);
        return res;
    }

    // Helper to create an admin and get token
    std::string getAdminToken() {
        RegisterRequest req{"admin@example.com", "password123", Role::Admin};
        auto userResult = authService_->registerUser(req);
        if (!isSuccess(userResult)) return "";
        
        LoginRequest loginReq{"admin@example.com", "password123"};
        auto loginResult = authService_->login(loginReq);
        if (!isSuccess(loginResult)) return "";
        
        return getValue(loginResult).token;
    }
    
    // Helper to create a student and get token
    std::string getStudentToken() {
        RegisterRequest req{"student@example.com", "password123", Role::Student};
        auto userResult = authService_->registerUser(req);
        if (!isSuccess(userResult)) return "";
        
        LoginRequest loginReq{"student@example.com", "password123"};
        auto loginResult = authService_->login(loginReq);
        if (!isSuccess(loginResult)) return "";
        
        return getValue(loginResult).token;
    }

    std::unique_ptr<DatabaseManager> dbManager_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<PasswordService> passwordService_;
    std::unique_ptr<JwtService> jwtService_;
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<crow::SimpleApp> app_;
};

// Test POST /api/auth/login with valid credentials returns 200 and token
TEST_F(AuthEndpointTest, LoginValidCredentialsReturns200) {
    // Setup a user
    RegisterRequest req{"user@example.com", "password123", Role::Student};
    authService_->registerUser(req);
    
    // Login
    crow::json::wvalue body;
    body["email"] = "user@example.com";
    body["password"] = "password123";
    
    auto res = makeRequest("POST", "/api/auth/login", body.dump());
    
    EXPECT_EQ(res.code, 200);
    auto resJson = crow::json::load(res.body);
    ASSERT_TRUE(resJson);
    // Response is wrapped: {"success": true, "data": {"user": {...}, "token": "..."}}
    EXPECT_TRUE(resJson.count("data") > 0);
    EXPECT_TRUE(resJson["data"].count("token") > 0);
    EXPECT_TRUE(resJson["data"].count("user") > 0);
}

// Test POST /api/auth/login with wrong password returns 401
TEST_F(AuthEndpointTest, LoginWrongPasswordReturns401) {
    RegisterRequest req{"user@example.com", "password123", Role::Student};
    authService_->registerUser(req);
    
    crow::json::wvalue body;
    body["email"] = "user@example.com";
    body["password"] = "wrongpass";
    
    auto res = makeRequest("POST", "/api/auth/login", body.dump());
    
    EXPECT_EQ(res.code, 401);
}

// Test POST /api/auth/login with missing fields returns 400
TEST_F(AuthEndpointTest, LoginMissingFieldsReturns400) {
    crow::json::wvalue body;
    body["email"] = "user@example.com";
    // missing password
    
    auto res = makeRequest("POST", "/api/auth/login", body.dump());
    
    // Could be 400 (Bad Request from parsing) or 401/Validation
    EXPECT_TRUE(res.code == 400 || res.code == 422);
}

// Test POST /api/auth/register with admin token creates user, returns 201
TEST_F(AuthEndpointTest, RegisterWithAdminTokenReturns201) {
    auto adminToken = getAdminToken();
    ASSERT_FALSE(adminToken.empty());
    
    crow::json::wvalue body;
    body["email"] = "newuser@example.com";
    body["password"] = "newpass123";
    body["role"] = "student";
    
    auto res = makeRequest("POST", "/api/auth/register", body.dump(), adminToken);
    
    EXPECT_EQ(res.code, 201);
    auto resJson = crow::json::load(res.body);
    ASSERT_TRUE(resJson);
    // Data is nested under "data" by successResponse()
    EXPECT_EQ(resJson["data"]["email"].s(), "newuser@example.com");
}

// Test POST /api/auth/register without token returns 401
TEST_F(AuthEndpointTest, RegisterWithoutTokenReturns401) {
    crow::json::wvalue body;
    body["email"] = "newuser@example.com";
    body["password"] = "newpass123";
    body["role"] = "student";
    
    auto res = makeRequest("POST", "/api/auth/register", body.dump());
    
    EXPECT_EQ(res.code, 401);
}

// Test POST /api/auth/register with student token returns 403
TEST_F(AuthEndpointTest, RegisterWithStudentTokenReturns403) {
    auto studentToken = getStudentToken();
    ASSERT_FALSE(studentToken.empty());
    
    crow::json::wvalue body;
    body["email"] = "newuser@example.com";
    body["password"] = "newpass123";
    body["role"] = "student";
    
    auto res = makeRequest("POST", "/api/auth/register", body.dump(), studentToken);
    
    EXPECT_EQ(res.code, 403);
}

// Test POST /api/auth/register with duplicate email returns 409
TEST_F(AuthEndpointTest, RegisterDuplicateEmailReturns409) {
    auto adminToken = getAdminToken();
    
    crow::json::wvalue body;
    body["email"] = "duplicate@example.com";
    body["password"] = "pass1";
    body["role"] = "student";
    
    // First creation
    makeRequest("POST", "/api/auth/register", body.dump(), adminToken);
    
    // Second creation
    body["password"] = "pass2";
    auto res = makeRequest("POST", "/api/auth/register", body.dump(), adminToken);
    
    EXPECT_EQ(res.code, 409);
}
