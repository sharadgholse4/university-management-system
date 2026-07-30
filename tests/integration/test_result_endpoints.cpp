#include <gtest/gtest.h>
#include <crow.h>
#include <memory>
#include <utility>
#include <string>

#include "test_helpers/test_db.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "student/repositories/sqlite_student_repository.hpp"
#include "results/repositories/sqlite_result_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "auth/services/auth_service.hpp"
#include "student/services/student_service.hpp"
#include "results/services/result_service.hpp"
#include "auth/controllers/auth_controller.hpp"
#include "results/controllers/result_controller.hpp"
#include "auth/middleware/auth_middleware.hpp"

namespace ums { namespace test {

class ResultEndpointsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize repositories
        userRepo_ = std::make_unique<SQLiteUserRepository>(testDb_.db());
        userRepo_->initialize();
        studentRepo_ = std::make_unique<SQLiteStudentRepository>(testDb_.db());
        studentRepo_->initialize();
        resultRepo_ = std::make_unique<SQLiteResultRepository>(testDb_.db());
        resultRepo_->initialize();

        // Initialize services
        passwordService_ = std::make_unique<PasswordService>(4);
        jwtService_ = std::make_unique<JwtService>("test-secret", 3600);
        authService_ = std::make_unique<AuthService>(*userRepo_, *passwordService_, *jwtService_);
        studentService_ = std::make_unique<StudentService>(*studentRepo_);
        resultService_ = std::make_unique<ResultService>(*resultRepo_);

        // Set up app and controllers
        authController_ = std::make_unique<AuthController>(*authService_, *jwtService_);
        authController_->registerRoutes(app_);
        
        resultController_ = std::make_unique<ResultController>(*resultService_, *jwtService_);
        resultController_->registerRoutes(app_);

        app_.validate();

        // Create initial users and tokens
        
        // 1. Admin
        RegisterRequest reqAdmin{"admin@test.com", "password123", Role::Admin};
        authService_->registerUser(reqAdmin);
        LoginRequest reqLoginAdmin{"admin@test.com", "password123"};
        adminToken_ = getValue(authService_->login(reqLoginAdmin)).token;

        // 2. Professor
        RegisterRequest reqProf{"prof@test.com", "password123", Role::Professor};
        authService_->registerUser(reqProf);
        LoginRequest reqLoginProf{"prof@test.com", "password123"};
        profToken_ = getValue(authService_->login(reqLoginProf)).token;

        // 3. Student
        RegisterRequest reqStudent{"student@test.com", "password123", Role::Student};
        authService_->registerUser(reqStudent);
        LoginRequest reqLoginStudent{"student@test.com", "password123"};
        studentToken_ = getValue(authService_->login(reqLoginStudent)).token;
        
        // Create student profile (User ID 3 for student)
        CreateStudentRequest reqCreateStudent{3, "CS2023001", "John", "Doe", 1, 1};
        studentService_->createStudent(reqCreateStudent);
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteStudentRepository> studentRepo_;
    std::unique_ptr<SQLiteResultRepository> resultRepo_;
    std::unique_ptr<PasswordService> passwordService_;
    std::unique_ptr<JwtService> jwtService_;
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<StudentService> studentService_;
    std::unique_ptr<ResultService> resultService_;
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<ResultController> resultController_;
    crow::SimpleApp app_;

    std::string adminToken_;
    std::string profToken_;
    std::string studentToken_;
};

TEST_F(ResultEndpointsTest, PostResultAsProfessorReturns201) {
    crow::request req;
    req.url = "/api/results";
    req.method = crow::HTTPMethod::POST;
    req.add_header("Authorization", "Bearer " + profToken_);
    
    crow::json::wvalue reqBody;
    reqBody["studentId"] = 1;
    reqBody["courseId"] = 100;
    reqBody["examType"] = "final";
    reqBody["marksObtained"] = 85.0;
    reqBody["maxMarks"] = 100.0;
    reqBody["semester"] = 1;
    req.body = reqBody.dump();
    
    crow::response res;
    app_.handle(req, res);
    
    EXPECT_EQ(res.code, 201);
}

TEST_F(ResultEndpointsTest, PostResultAsStudentReturns403) {
    crow::request req;
    req.url = "/api/results";
    req.method = crow::HTTPMethod::POST;
    req.add_header("Authorization", "Bearer " + studentToken_);
    
    crow::json::wvalue reqBody;
    reqBody["studentId"] = 1;
    reqBody["courseId"] = 100;
    reqBody["examType"] = "final";
    reqBody["marksObtained"] = 85.0;
    reqBody["maxMarks"] = 100.0;
    reqBody["semester"] = 1;
    req.body = reqBody.dump();
    
    crow::response res;
    app_.handle(req, res);
    
    EXPECT_EQ(res.code, 403);
}

TEST_F(ResultEndpointsTest, PostResultWithoutTokenReturns401) {
    crow::request req;
    req.url = "/api/results";
    req.method = crow::HTTPMethod::POST;
    
    crow::json::wvalue reqBody;
    reqBody["studentId"] = 1;
    reqBody["courseId"] = 100;
    reqBody["examType"] = "final";
    reqBody["marksObtained"] = 85.0;
    reqBody["maxMarks"] = 100.0;
    reqBody["semester"] = 1;
    req.body = reqBody.dump();
    
    crow::response res;
    app_.handle(req, res);
    
    EXPECT_EQ(res.code, 401);
}

TEST_F(ResultEndpointsTest, GetResultsByStudentAsOwnerReturns200) {
    resultService_->addResult({1, 100, "final", 85.0, 100.0, 1});
    
    crow::request req;
    req.url = "/api/results/student/1";
    req.method = crow::HTTPMethod::GET;
    req.add_header("Authorization", "Bearer " + studentToken_);
    
    crow::response res;
    app_.handle(req, res);
    
    EXPECT_EQ(res.code, 200);
}

TEST_F(ResultEndpointsTest, GetCgpaAsStudentReturns200) {
    resultService_->addResult({1, 100, "final", 85.0, 100.0, 1});
    
    crow::request req;
    req.url = "/api/results/cgpa/student/1";
    req.method = crow::HTTPMethod::GET;
    req.add_header("Authorization", "Bearer " + studentToken_);
    
    crow::response res;
    app_.handle(req, res);
    
    EXPECT_EQ(res.code, 200);
}

TEST_F(ResultEndpointsTest, GetSgpaAsStudentReturns200) {
    resultService_->addResult({1, 100, "final", 85.0, 100.0, 1});
    
    crow::request req;
    req.url = "/api/results/sgpa/student/1/semester/1";
    req.method = crow::HTTPMethod::GET;
    req.add_header("Authorization", "Bearer " + studentToken_);
    
    crow::response res;
    app_.handle(req, res);
    
    EXPECT_EQ(res.code, 200);
}

TEST_F(ResultEndpointsTest, PutResultAsProfessorReturns200) {
    auto added = getValue(resultService_->addResult({1, 100, "final", 85.0, 100.0, 1}));
    
    crow::request req;
    req.url = "/api/results/" + std::to_string(added.id());
    req.method = crow::HTTPMethod::PUT;
    req.add_header("Authorization", "Bearer " + profToken_);
    
    crow::json::wvalue reqBody;
    reqBody["marksObtained"] = 90.0;
    reqBody["maxMarks"] = 100.0;
    req.body = reqBody.dump();
    
    crow::response res;
    app_.handle(req, res);
    
    EXPECT_EQ(res.code, 200);
}

TEST_F(ResultEndpointsTest, GetResultByIdReturns200) {
    auto added = getValue(resultService_->addResult({1, 100, "final", 85.0, 100.0, 1}));
    
    crow::request req;
    req.url = "/api/results/" + std::to_string(added.id());
    req.method = crow::HTTPMethod::GET;
    req.add_header("Authorization", "Bearer " + adminToken_);

    crow::response res;
    app_.handle(req, res);

    EXPECT_EQ(res.code, 200);
}

}} // namespace ums::test
