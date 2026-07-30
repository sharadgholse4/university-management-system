#include <gtest/gtest.h>
#include <crow.h>
#include <memory>
#include <utility>
#include <string>

#include "test_helpers/test_db.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "student/repositories/sqlite_student_repository.hpp"
#include "attendance/repositories/sqlite_attendance_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "auth/services/auth_service.hpp"
#include "student/services/student_service.hpp"
#include "attendance/services/attendance_service.hpp"
#include "auth/controllers/auth_controller.hpp"
#include "attendance/controllers/attendance_controller.hpp"
#include "auth/middleware/auth_middleware.hpp"

namespace ums { namespace test {

class AttendanceEndpointsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize repositories
        userRepo_ = std::make_unique<SQLiteUserRepository>(testDb_.db());
        userRepo_->initialize();
        studentRepo_ = std::make_unique<SQLiteStudentRepository>(testDb_.db());
        studentRepo_->initialize();
        attendanceRepo_ = std::make_unique<SQLiteAttendanceRepository>(testDb_.db());
        attendanceRepo_->initialize();

        // Initialize services
        passwordService_ = std::make_unique<PasswordService>(4);
        jwtService_ = std::make_unique<JwtService>("test-secret", 3600);
        authService_ = std::make_unique<AuthService>(*userRepo_, *passwordService_, *jwtService_);
        studentService_ = std::make_unique<StudentService>(*studentRepo_);
        attendanceService_ = std::make_unique<AttendanceService>(*attendanceRepo_);

        // Set up app and controllers
        authController_ = std::make_unique<AuthController>(*authService_, *jwtService_);
        authController_->registerRoutes(app_);
        
        attendanceController_ = std::make_unique<AttendanceController>(*attendanceService_, *jwtService_);
        attendanceController_->registerRoutes(app_);

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
        
        // Create student profile
        // User ID 3 for student
        CreateStudentRequest reqCreateStudent{3, "CS2023001", "John", "Doe", 1, 3};
        studentService_->createStudent(reqCreateStudent);
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteStudentRepository> studentRepo_;
    std::unique_ptr<SQLiteAttendanceRepository> attendanceRepo_;
    std::unique_ptr<PasswordService> passwordService_;
    std::unique_ptr<JwtService> jwtService_;
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<StudentService> studentService_;
    std::unique_ptr<AttendanceService> attendanceService_;
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<AttendanceController> attendanceController_;
    crow::SimpleApp app_;

    std::string adminToken_;
    std::string profToken_;
    std::string studentToken_;
};

TEST_F(AttendanceEndpointsTest, PostAttendanceAsProfessorReturns201) {
    crow::request req;
    req.url = "/api/attendance";
    req.method = crow::HTTPMethod::POST;
    req.add_header("Authorization", "Bearer " + profToken_);
    
    crow::json::wvalue body;
    body["studentId"] = 1; // Student ID 1
    body["courseId"] = 101;
    body["date"] = "2024-01-15";
    body["status"] = "present";
    req.body = body.dump();

    crow::response res;
    app_.handle(req, res);

    EXPECT_EQ(res.code, 201);
    
    auto resJson = crow::json::load(res.body);
    ASSERT_TRUE(resJson);
    EXPECT_TRUE(resJson["success"].b());
    ASSERT_TRUE(resJson.count("data"));
    EXPECT_EQ(resJson["data"]["studentId"].i(), 1);
    EXPECT_EQ(resJson["data"]["courseId"].i(), 101);
    EXPECT_EQ(resJson["data"]["status"].s(), "present");
}

TEST_F(AttendanceEndpointsTest, PostAttendanceAsStudentReturns403) {
    crow::request req;
    req.url = "/api/attendance";
    req.method = crow::HTTPMethod::POST;
    req.add_header("Authorization", "Bearer " + studentToken_);
    
    crow::json::wvalue body;
    body["studentId"] = 1;
    body["courseId"] = 101;
    body["date"] = "2024-01-15";
    body["status"] = "present";
    req.body = body.dump();

    crow::response res;
    app_.handle(req, res);

    EXPECT_EQ(res.code, 403);
}

TEST_F(AttendanceEndpointsTest, PostAttendanceWithoutTokenReturns401) {
    crow::request req;
    req.url = "/api/attendance";
    req.method = crow::HTTPMethod::POST;
    
    crow::json::wvalue body;
    body["studentId"] = 1;
    body["courseId"] = 101;
    body["date"] = "2024-01-15";
    body["status"] = "present";
    req.body = body.dump();

    crow::response res;
    app_.handle(req, res);

    EXPECT_EQ(res.code, 401);
}

TEST_F(AttendanceEndpointsTest, GetAttendanceByCourseAsProfessorReturns200) {
    // Setup some data
    MarkAttendanceRequest reqAtt{1, 101, "2024-01-15", "present", 2};
    attendanceService_->markAttendance(reqAtt);

    crow::request req;
    req.url = "/api/attendance/course/101";
    req.method = crow::HTTPMethod::GET;
    req.add_header("Authorization", "Bearer " + profToken_);

    crow::response res;
    app_.handle(req, res);

    EXPECT_EQ(res.code, 200);
    auto resJson = crow::json::load(res.body);
    ASSERT_TRUE(resJson);
    EXPECT_TRUE(resJson["success"].b());
    ASSERT_TRUE(resJson.count("data"));
    
    // Expect list
    auto dataList = resJson["data"];
    EXPECT_EQ(dataList.size(), 1);
}

TEST_F(AttendanceEndpointsTest, GetAttendanceByStudentAsOwnerReturns200) {
    // Setup some data
    MarkAttendanceRequest reqAtt{1, 101, "2024-01-15", "present", 2};
    attendanceService_->markAttendance(reqAtt);

    crow::request req;
    req.url = "/api/attendance/student/1";
    req.method = crow::HTTPMethod::GET;
    req.add_header("Authorization", "Bearer " + studentToken_);

    crow::response res;
    app_.handle(req, res);

    EXPECT_EQ(res.code, 200);
    auto resJson = crow::json::load(res.body);
    ASSERT_TRUE(resJson);
    EXPECT_TRUE(resJson["success"].b());
    ASSERT_TRUE(resJson.count("data"));
    
    auto dataList = resJson["data"];
    EXPECT_EQ(dataList.size(), 1);
}

TEST_F(AttendanceEndpointsTest, PutAttendanceAsProfessorReturns200) {
    // Setup data
    MarkAttendanceRequest reqAtt{1, 101, "2024-01-15", "present", 2};
    auto created = getValue(attendanceService_->markAttendance(reqAtt));

    crow::request req;
    req.url = "/api/attendance/" + std::to_string(created.id());
    req.method = crow::HTTPMethod::PUT;
    req.add_header("Authorization", "Bearer " + profToken_);
    
    crow::json::wvalue body;
    body["status"] = "absent";
    req.body = body.dump();

    crow::response res;
    app_.handle(req, res);

    EXPECT_EQ(res.code, 200);
    auto resJson = crow::json::load(res.body);
    ASSERT_TRUE(resJson);
    EXPECT_TRUE(resJson["success"].b());
    ASSERT_TRUE(resJson.count("data"));
    EXPECT_EQ(resJson["data"]["status"].s(), "absent");
}

TEST_F(AttendanceEndpointsTest, GetAttendanceByIdReturns200) {
    // Setup data
    MarkAttendanceRequest reqAtt{1, 101, "2024-01-15", "present", 2};
    auto created = getValue(attendanceService_->markAttendance(reqAtt));

    crow::request req;
    req.url = "/api/attendance/" + std::to_string(created.id());
    req.method = crow::HTTPMethod::GET;
    req.add_header("Authorization", "Bearer " + adminToken_);

    crow::response res;
    app_.handle(req, res);

    EXPECT_EQ(res.code, 200);
    auto resJson = crow::json::load(res.body);
    ASSERT_TRUE(resJson);
    EXPECT_TRUE(resJson["success"].b());
    ASSERT_TRUE(resJson.count("data"));
    EXPECT_EQ(resJson["data"]["id"].i(), created.id());
}

}} // namespace ums::test
