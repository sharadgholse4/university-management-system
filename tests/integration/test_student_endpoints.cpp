#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <string>
#include "crow.h"
#include "database/database_manager.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "student/repositories/sqlite_student_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "auth/services/auth_service.hpp"
#include "student/services/student_service.hpp"
#include "auth/controllers/auth_controller.hpp"
#include "student/controllers/student_controller.hpp"

using namespace ums;

class StudentEndpointTest : public ::testing::Test {
protected:
    void SetUp() override {
        dbManager_ = std::make_unique<DatabaseManager>(":memory:");
        
        userRepo_ = std::make_unique<SQLiteUserRepository>(dbManager_->db());
        userRepo_->initialize();
        
        studentRepo_ = std::make_unique<SQLiteStudentRepository>(dbManager_->db());
        studentRepo_->initialize();
        
        passwordService_ = std::make_unique<PasswordService>(4);
        jwtService_ = std::make_unique<JwtService>("test-secret", 3600);
        
        authService_ = std::make_unique<AuthService>(*userRepo_, *passwordService_, *jwtService_);
        studentService_ = std::make_unique<StudentService>(*studentRepo_);
        
        authController_ = std::make_unique<AuthController>(*authService_, *jwtService_);
        studentController_ = std::make_unique<StudentController>(*studentService_, *jwtService_);
        
        app_ = std::make_unique<crow::SimpleApp>();
        authController_->registerRoutes(*app_);
        studentController_->registerRoutes(*app_);
        app_->validate();
    }

    crow::response makeRequest(const std::string& method, const std::string& url, 
                                const std::string& body = "", 
                                const std::string& authToken = "") {
        crow::request req;
        if (method == "POST") req.method = crow::HTTPMethod::POST;
        else if (method == "PUT") req.method = crow::HTTPMethod::PUT;
        else if (method == "DELETE") req.method = crow::HTTPMethod::DELETE;
        else req.method = crow::HTTPMethod::GET;
        
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

    std::pair<int, std::string> createAdminAndGetToken() {
        RegisterRequest req{"admin@example.com", "password123", Role::Admin};
        auto userResult = authService_->registerUser(req);
        
        LoginRequest loginReq{"admin@example.com", "password123"};
        auto loginResult = authService_->login(loginReq);
        
        return {getValue(userResult).id(), getValue(loginResult).token};
    }
    
    std::pair<int, std::string> createStudentAndGetToken(const std::string& email) {
        RegisterRequest req{email, "password123", Role::Student};
        auto userResult = authService_->registerUser(req);
        
        LoginRequest loginReq{email, "password123"};
        auto loginResult = authService_->login(loginReq);
        
        return {getValue(userResult).id(), getValue(loginResult).token};
    }

    std::unique_ptr<DatabaseManager> dbManager_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteStudentRepository> studentRepo_;
    std::unique_ptr<PasswordService> passwordService_;
    std::unique_ptr<JwtService> jwtService_;
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<StudentService> studentService_;
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<StudentController> studentController_;
    std::unique_ptr<crow::SimpleApp> app_;
};

// Test GET /api/students as admin returns 200
TEST_F(StudentEndpointTest, GetAllStudentsAsAdminReturns200) {
    auto [adminId, adminToken] = createAdminAndGetToken();
    
    auto res = makeRequest("GET", "/api/students", "", adminToken);
    EXPECT_EQ(res.code, 200);
}

// Test GET /api/students as student returns 403
TEST_F(StudentEndpointTest, GetAllStudentsAsStudentReturns403) {
    auto [studentId, studentToken] = createStudentAndGetToken("student1@example.com");
    
    auto res = makeRequest("GET", "/api/students", "", studentToken);
    EXPECT_EQ(res.code, 403);
}

// Test GET /api/students/<id> as owner returns 200
TEST_F(StudentEndpointTest, GetStudentByIdAsOwnerReturns200) {
    auto [studentId, studentToken] = createStudentAndGetToken("student1@example.com");
    
    CreateStudentRequest createReq{studentId, "CS2023001", "John", "Doe", 1, 3};
    auto studentResult = studentService_->createStudent(createReq);
    int profileId = getValue(studentResult).id();
    
    auto res = makeRequest("GET", "/api/students/" + std::to_string(profileId), "", studentToken);
    EXPECT_EQ(res.code, 200);
    
    auto resJson = crow::json::load(res.body);
    ASSERT_TRUE(resJson);
    EXPECT_EQ(resJson["data"]["rollNumber"].s(), "CS2023001");
}

// Test GET /api/students/<id> as non-owner returns 403
TEST_F(StudentEndpointTest, GetStudentByIdAsNonOwnerReturns403) {
    auto [studentId1, studentToken1] = createStudentAndGetToken("student1@example.com");
    auto [studentId2, studentToken2] = createStudentAndGetToken("student2@example.com");
    
    CreateStudentRequest createReq{studentId1, "CS2023001", "John", "Doe", 1, 3};
    auto studentResult = studentService_->createStudent(createReq);
    int profileId = getValue(studentResult).id();
    
    // student 2 tries to access student 1's profile
    auto res = makeRequest("GET", "/api/students/" + std::to_string(profileId), "", studentToken2);
    EXPECT_EQ(res.code, 403);
}

// Test POST /api/students as admin returns 201
TEST_F(StudentEndpointTest, CreateStudentAsAdminReturns201) {
    auto [adminId, adminToken] = createAdminAndGetToken();
    auto [studentId, _] = createStudentAndGetToken("student@example.com");
    
    crow::json::wvalue body;
    body["userId"] = studentId;
    body["rollNumber"] = "CS2023001";
    body["firstName"] = "John";
    body["lastName"] = "Doe";
    body["departmentId"] = 1;
    body["semester"] = 3;
    
    auto res = makeRequest("POST", "/api/students", body.dump(), adminToken);
    EXPECT_EQ(res.code, 201);
    
    auto resJson = crow::json::load(res.body);
    EXPECT_EQ(resJson["data"]["rollNumber"].s(), "CS2023001");
}

// Test POST /api/students without token returns 401
TEST_F(StudentEndpointTest, CreateStudentWithoutTokenReturns401) {
    crow::json::wvalue body;
    body["userId"] = 1;
    body["rollNumber"] = "CS2023001";
    body["firstName"] = "John";
    body["lastName"] = "Doe";
    body["departmentId"] = 1;
    body["semester"] = 3;
    
    auto res = makeRequest("POST", "/api/students", body.dump());
    EXPECT_EQ(res.code, 401);
}

// Test PUT /api/students/<id> as owner returns 200
TEST_F(StudentEndpointTest, UpdateStudentAsOwnerReturns200) {
    auto [studentId, studentToken] = createStudentAndGetToken("student@example.com");
    
    CreateStudentRequest createReq{studentId, "CS2023001", "John", "Doe", 1, 3};
    auto studentResult = studentService_->createStudent(createReq);
    int profileId = getValue(studentResult).id();
    
    crow::json::wvalue body;
    body["firstName"] = "Johnny";
    body["lastName"] = "Doee";
    body["departmentId"] = 2;
    body["semester"] = 4;
    
    auto res = makeRequest("PUT", "/api/students/" + std::to_string(profileId), body.dump(), studentToken);
    EXPECT_EQ(res.code, 200);
    
    auto resJson = crow::json::load(res.body);
    EXPECT_EQ(resJson["data"]["firstName"].s(), "Johnny");
}

// Test PUT /api/students/<id> as non-owner returns 403
TEST_F(StudentEndpointTest, UpdateStudentAsNonOwnerReturns403) {
    auto [studentId1, studentToken1] = createStudentAndGetToken("student1@example.com");
    auto [studentId2, studentToken2] = createStudentAndGetToken("student2@example.com");
    
    CreateStudentRequest createReq{studentId1, "CS2023001", "John", "Doe", 1, 3};
    auto studentResult = studentService_->createStudent(createReq);
    int profileId = getValue(studentResult).id();
    
    crow::json::wvalue body;
    body["firstName"] = "Johnny";
    body["lastName"] = "Doee";
    body["departmentId"] = 2;
    body["semester"] = 4;
    
    auto res = makeRequest("PUT", "/api/students/" + std::to_string(profileId), body.dump(), studentToken2);
    EXPECT_EQ(res.code, 403);
}
