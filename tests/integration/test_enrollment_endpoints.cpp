#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <string>
#include <vector>
#include "crow.h"
#include "common/types.hpp"
#include "database/database_manager.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "student/repositories/sqlite_student_repository.hpp"
#include "course/repositories/sqlite_department_repository.hpp"
#include "course/repositories/sqlite_course_repository.hpp"
#include "enrollment/repositories/sqlite_enrollment_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "auth/services/auth_service.hpp"
#include "student/services/student_service.hpp"
#include "course/services/course_service.hpp"
#include "enrollment/services/enrollment_service.hpp"
#include "auth/controllers/auth_controller.hpp"
#include "enrollment/controllers/enrollment_controller.hpp"

using namespace ums;

// WHY: Validates the full stack functionality of the Enrollment endpoints using an in-memory database,
// ensuring the controller correctly integrates with services and correctly protects endpoints using JWT.
class EnrollmentEndpointTest : public ::testing::Test {
protected:
    void SetUp() override {
        dbManager_ = std::make_unique<DatabaseManager>(":memory:");
        
        userRepo_ = std::make_unique<SQLiteUserRepository>(dbManager_->db());
        userRepo_->initialize();
        
        studentRepo_ = std::make_unique<SQLiteStudentRepository>(dbManager_->db());
        studentRepo_->initialize();
        
        deptRepo_ = std::make_unique<SQLiteDepartmentRepository>(dbManager_->db());
        deptRepo_->initialize();
        
        courseRepo_ = std::make_unique<SQLiteCourseRepository>(dbManager_->db());
        courseRepo_->initialize();
        
        enrollmentRepo_ = std::make_unique<SQLiteEnrollmentRepository>(dbManager_->db());
        enrollmentRepo_->initialize();
        
        passwordService_ = std::make_unique<PasswordService>(4);
        jwtService_ = std::make_unique<JwtService>("test-secret", 3600);
        
        authService_ = std::make_unique<AuthService>(*userRepo_, *passwordService_, *jwtService_);
        studentService_ = std::make_unique<StudentService>(*studentRepo_);
        courseService_ = std::make_unique<CourseService>(*deptRepo_, *courseRepo_);
        enrollmentService_ = std::make_unique<EnrollmentService>(*enrollmentRepo_, *studentRepo_, *courseRepo_);
        
        authController_ = std::make_unique<AuthController>(*authService_, *jwtService_);
        enrollmentController_ = std::make_unique<EnrollmentController>(*enrollmentService_, *jwtService_);
        
        app_ = std::make_unique<crow::SimpleApp>();
        authController_->registerRoutes(*app_);
        enrollmentController_->registerRoutes(*app_);
        app_->validate();
        
        // Setup base data
        deptRepo_->create("Computer Science", "CS", 1);
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
        EXPECT_TRUE(isSuccess(userResult));
        LoginRequest loginReq{"admin@example.com", "password123"};
        auto loginResult = authService_->login(loginReq);
        EXPECT_TRUE(isSuccess(loginResult));
        return {getValue(userResult).id(), getValue(loginResult).token};
    }
    
    std::pair<int, std::string> createProfessorAndGetToken(const std::string& email) {
        RegisterRequest req{email, "password123", Role::Professor};
        auto userResult = authService_->registerUser(req);
        EXPECT_TRUE(isSuccess(userResult));
        LoginRequest loginReq{email, "password123"};
        auto loginResult = authService_->login(loginReq);
        EXPECT_TRUE(isSuccess(loginResult));
        return {getValue(userResult).id(), getValue(loginResult).token};
    }

    std::pair<int, std::string> createStudentAndGetToken(const std::string& email, const std::string& rollNumber) {
        RegisterRequest req{email, "password123", Role::Student};
        auto userResult = authService_->registerUser(req);
        EXPECT_TRUE(isSuccess(userResult));
        int userId = getValue(userResult).id();
        
        LoginRequest loginReq{email, "password123"};
        auto loginResult = authService_->login(loginReq);
        EXPECT_TRUE(isSuccess(loginResult));
        
        CreateStudentRequest createReq{userId, rollNumber, "John", "Doe", 1, 3};
        auto studentResult = studentService_->createStudent(createReq);
        EXPECT_TRUE(isSuccess(studentResult));
        
        return {getValue(studentResult).id(), getValue(loginResult).token};
    }

    int createCourse(const std::string& code) {
        CreateCourseRequest req{code, "Test Course", 1, 1, 3, 1};
        auto result = courseService_->createCourse(req);
        EXPECT_TRUE(isSuccess(result));
        return getValue(result).id();
    }

    std::unique_ptr<DatabaseManager> dbManager_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteStudentRepository> studentRepo_;
    std::unique_ptr<SQLiteDepartmentRepository> deptRepo_;
    std::unique_ptr<SQLiteCourseRepository> courseRepo_;
    std::unique_ptr<SQLiteEnrollmentRepository> enrollmentRepo_;
    std::unique_ptr<PasswordService> passwordService_;
    std::unique_ptr<JwtService> jwtService_;
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<StudentService> studentService_;
    std::unique_ptr<CourseService> courseService_;
    std::unique_ptr<EnrollmentService> enrollmentService_;
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<EnrollmentController> enrollmentController_;
    std::unique_ptr<crow::SimpleApp> app_;
};

// Test POST /api/enrollments returns 201
TEST_F(EnrollmentEndpointTest, PostEnrollmentAsStudentReturns201) {
    auto [studentId, studentToken] = createStudentAndGetToken("student1@example.com", "R001");
    int courseId = createCourse("CS101");
    
    crow::json::wvalue body;
    body["studentId"] = studentId;
    body["courseId"] = courseId;
    
    auto res = makeRequest("POST", "/api/enrollments", body.dump(), studentToken);
    EXPECT_EQ(res.code, 201);
}

// Test POST /api/enrollments without token returns 401
TEST_F(EnrollmentEndpointTest, PostEnrollmentWithoutTokenReturns401) {
    auto [studentId, _] = createStudentAndGetToken("student1@example.com", "R001");
    int courseId = createCourse("CS101");
    
    crow::json::wvalue body;
    body["studentId"] = studentId;
    body["courseId"] = courseId;
    
    auto res = makeRequest("POST", "/api/enrollments", body.dump());
    EXPECT_EQ(res.code, 401);
}

// Test GET /api/enrollments/student/<id> as owner returns 200
TEST_F(EnrollmentEndpointTest, GetEnrollmentsByStudentAsOwnerReturns200) {
    auto [studentId, studentToken] = createStudentAndGetToken("student1@example.com", "R001");
    int courseId = createCourse("CS101");
    
    enrollmentService_->enrollStudent(EnrollStudentRequest{studentId, courseId});
    
    auto res = makeRequest("GET", "/api/enrollments/student/" + std::to_string(studentId), "", studentToken);
    EXPECT_EQ(res.code, 200);
}

// Test GET /api/enrollments/course/<id> as professor returns 200
TEST_F(EnrollmentEndpointTest, GetEnrollmentsByCourseAsProfessorReturns200) {
    auto [profId, profToken] = createProfessorAndGetToken("prof@example.com");
    auto [studentId, _] = createStudentAndGetToken("student1@example.com", "R001");
    int courseId = createCourse("CS101");
    
    enrollmentService_->enrollStudent(EnrollStudentRequest{studentId, courseId});
    
    auto res = makeRequest("GET", "/api/enrollments/course/" + std::to_string(courseId), "", profToken);
    EXPECT_EQ(res.code, 200);
}

// Test DELETE /api/enrollments/student/<studentId>/course/<courseId> returns 200
TEST_F(EnrollmentEndpointTest, DeleteEnrollmentAsStudentReturns200) {
    auto [studentId, studentToken] = createStudentAndGetToken("student1@example.com", "R001");
    int courseId = createCourse("CS101");
    
    auto result = enrollmentService_->enrollStudent(EnrollStudentRequest{studentId, courseId});
    EXPECT_TRUE(isSuccess(result));
    
    auto res = makeRequest("DELETE", "/api/enrollments/student/" + std::to_string(studentId) + "/course/" + std::to_string(courseId), "", studentToken);
    EXPECT_EQ(res.code, 200);
}

// Test GET /api/enrollments/<id> returns 200
TEST_F(EnrollmentEndpointTest, GetEnrollmentByIdReturns200) {
    auto [studentId, studentToken] = createStudentAndGetToken("student1@example.com", "R001");
    int courseId = createCourse("CS101");
    
    auto result = enrollmentService_->enrollStudent(EnrollStudentRequest{studentId, courseId});
    EXPECT_TRUE(isSuccess(result));
    int enrollmentId = getValue(result).id();
    
    auto res = makeRequest("GET", "/api/enrollments/" + std::to_string(enrollmentId), "", studentToken);
    EXPECT_EQ(res.code, 200);
}

// Test duplicate POST returns 409
TEST_F(EnrollmentEndpointTest, PostDuplicateEnrollmentReturns409) {
    auto [studentId, studentToken] = createStudentAndGetToken("student1@example.com", "R001");
    int courseId = createCourse("CS101");
    
    enrollmentService_->enrollStudent(EnrollStudentRequest{studentId, courseId});
    
    crow::json::wvalue body;
    body["studentId"] = studentId;
    body["courseId"] = courseId;
    
    auto res = makeRequest("POST", "/api/enrollments", body.dump(), studentToken);
    EXPECT_EQ(res.code, 409);
}

// Test GET enrollment by ID not found returns 404
TEST_F(EnrollmentEndpointTest, GetEnrollmentByIdNotFoundReturns404) {
    auto [studentId, studentToken] = createStudentAndGetToken("student1@example.com", "R001");
    
    auto res = makeRequest("GET", "/api/enrollments/999", "", studentToken);
    EXPECT_EQ(res.code, 404);
}
