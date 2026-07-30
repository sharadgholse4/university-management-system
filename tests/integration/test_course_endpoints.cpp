#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <string>
#include "crow.h"
#include "database/database_manager.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "course/repositories/sqlite_department_repository.hpp"
#include "course/repositories/sqlite_course_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "auth/services/auth_service.hpp"
#include "course/services/course_service.hpp"
#include "auth/controllers/auth_controller.hpp"
#include "course/controllers/course_controller.hpp"

using namespace ums;

class CourseEndpointTest : public ::testing::Test {
protected:
    void SetUp() override {
        dbManager_ = std::make_unique<DatabaseManager>(":memory:");
        
        userRepo_ = std::make_unique<SQLiteUserRepository>(dbManager_->db());
        userRepo_->initialize();
        
        deptRepo_ = std::make_unique<SQLiteDepartmentRepository>(dbManager_->db());
        deptRepo_->initialize();
        
        courseRepo_ = std::make_unique<SQLiteCourseRepository>(dbManager_->db());
        courseRepo_->initialize();
        
        passwordService_ = std::make_unique<PasswordService>(4);
        jwtService_ = std::make_unique<JwtService>("test-secret", 3600);
        
        authService_ = std::make_unique<AuthService>(*userRepo_, *passwordService_, *jwtService_);
        courseService_ = std::make_unique<CourseService>(*deptRepo_, *courseRepo_);
        
        authController_ = std::make_unique<AuthController>(*authService_, *jwtService_);
        courseController_ = std::make_unique<CourseController>(*courseService_, *jwtService_);
        
        app_ = std::make_unique<crow::SimpleApp>();
        authController_->registerRoutes(*app_);
        courseController_->registerRoutes(*app_);
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

    std::pair<int, std::string> createUserAndGetToken(const std::string& email, Role role) {
        RegisterRequest req{email, "password123", role};
        auto userResult = authService_->registerUser(req);
        EXPECT_TRUE(isSuccess(userResult));
        
        LoginRequest loginReq{email, "password123"};
        auto loginResult = authService_->login(loginReq);
        EXPECT_TRUE(isSuccess(loginResult));
        
        return {getValue(userResult).id(), getValue(loginResult).token};
    }

    std::unique_ptr<DatabaseManager> dbManager_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteDepartmentRepository> deptRepo_;
    std::unique_ptr<SQLiteCourseRepository> courseRepo_;
    std::unique_ptr<PasswordService> passwordService_;
    std::unique_ptr<JwtService> jwtService_;
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<CourseService> courseService_;
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<CourseController> courseController_;
    std::unique_ptr<crow::SimpleApp> app_;
};

// Test POST /api/departments as admin returns 201
TEST_F(CourseEndpointTest, PostDepartmentAsAdminReturns201) {
    auto [adminId, adminToken] = createUserAndGetToken("admin@example.com", Role::Admin);
    auto [profId, _] = createUserAndGetToken("prof@example.com", Role::Professor);
    
    crow::json::wvalue body;
    body["name"] = "Computer Science";
    body["code"] = "CS";
    body["headProfessorId"] = profId;
    
    auto res = makeRequest("POST", "/api/departments", body.dump(), adminToken);
    EXPECT_EQ(res.code, 201);
}

// Test POST /api/departments as student returns 403
TEST_F(CourseEndpointTest, PostDepartmentAsStudentReturns403) {
    auto [studentId, studentToken] = createUserAndGetToken("student@example.com", Role::Student);
    
    crow::json::wvalue body;
    body["name"] = "Computer Science";
    body["code"] = "CS";
    body["headProfessorId"] = 1;
    
    auto res = makeRequest("POST", "/api/departments", body.dump(), studentToken);
    EXPECT_EQ(res.code, 403);
}

// Test GET /api/departments returns 200
TEST_F(CourseEndpointTest, GetDepartmentsReturns200) {
    auto [studentId, studentToken] = createUserAndGetToken("student@example.com", Role::Student);
    
    auto res = makeRequest("GET", "/api/departments", "", studentToken);
    EXPECT_EQ(res.code, 200);
}

// Test POST /api/courses as admin returns 201
TEST_F(CourseEndpointTest, PostCourseAsAdminReturns201) {
    auto [adminId, adminToken] = createUserAndGetToken("admin@example.com", Role::Admin);
    auto [profId, _] = createUserAndGetToken("prof@example.com", Role::Professor);
    
    // Create department first
    CreateDepartmentRequest deptReq{"Computer Science", "CS", profId};
    auto deptResult = courseService_->createDepartment(deptReq);
    int deptId = getValue(deptResult).id();
    
    crow::json::wvalue body;
    body["code"] = "CS101";
    body["name"] = "Intro to CS";
    body["departmentId"] = deptId;
    body["professorId"] = profId;
    body["credits"] = 3;
    body["semester"] = 1;
    
    auto res = makeRequest("POST", "/api/courses", body.dump(), adminToken);
    EXPECT_EQ(res.code, 201);
}

// Test POST /api/courses as student returns 403
TEST_F(CourseEndpointTest, PostCourseAsStudentReturns403) {
    auto [studentId, studentToken] = createUserAndGetToken("student@example.com", Role::Student);
    
    crow::json::wvalue body;
    body["code"] = "CS101";
    body["name"] = "Intro to CS";
    body["departmentId"] = 1;
    body["professorId"] = 1;
    body["credits"] = 3;
    body["semester"] = 1;
    
    auto res = makeRequest("POST", "/api/courses", body.dump(), studentToken);
    EXPECT_EQ(res.code, 403);
}

// Test GET /api/courses returns 200
TEST_F(CourseEndpointTest, GetCoursesReturns200) {
    auto [studentId, studentToken] = createUserAndGetToken("student@example.com", Role::Student);
    
    auto res = makeRequest("GET", "/api/courses", "", studentToken);
    EXPECT_EQ(res.code, 200);
}

// Test PUT /api/courses/<id> as professor returns 200
TEST_F(CourseEndpointTest, PutCourseAsProfessorReturns200) {
    auto [profId, profToken] = createUserAndGetToken("prof@example.com", Role::Professor);
    
    // Create department and course
    CreateDepartmentRequest deptReq{"Computer Science", "CS", profId};
    auto deptResult = courseService_->createDepartment(deptReq);
    int deptId = getValue(deptResult).id();
    
    CreateCourseRequest courseReq{"CS101", "Intro to CS", deptId, profId, 3, 1};
    auto courseResult = courseService_->createCourse(courseReq);
    int courseId = getValue(courseResult).id();
    
    crow::json::wvalue body;
    body["name"] = "Advanced CS";
    body["professorId"] = profId;
    body["credits"] = 4;
    body["semester"] = 2;
    
    auto res = makeRequest("PUT", "/api/courses/" + std::to_string(courseId), body.dump(), profToken);
    EXPECT_EQ(res.code, 200);
}

// Test GET /api/courses/<id> returns 200
TEST_F(CourseEndpointTest, GetCourseByIdReturns200) {
    auto [profId, profToken] = createUserAndGetToken("prof@example.com", Role::Professor);
    auto [studentId, studentToken] = createUserAndGetToken("student@example.com", Role::Student);
    
    CreateDepartmentRequest deptReq{"Computer Science", "CS", profId};
    auto deptResult = courseService_->createDepartment(deptReq);
    int deptId = getValue(deptResult).id();
    
    CreateCourseRequest courseReq{"CS101", "Intro to CS", deptId, profId, 3, 1};
    auto courseResult = courseService_->createCourse(courseReq);
    int courseId = getValue(courseResult).id();
    
    auto res = makeRequest("GET", "/api/courses/" + std::to_string(courseId), "", studentToken);
    EXPECT_EQ(res.code, 200);
}
