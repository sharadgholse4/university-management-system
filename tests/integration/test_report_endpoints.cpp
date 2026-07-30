#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <string>
#include <vector>
#include "crow.h"
#include "database/database_manager.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "student/repositories/sqlite_student_repository.hpp"
#include "course/repositories/sqlite_department_repository.hpp"
#include "course/repositories/sqlite_course_repository.hpp"
#include "attendance/repositories/sqlite_attendance_repository.hpp"
#include "results/repositories/sqlite_result_repository.hpp"
#include "enrollment/repositories/sqlite_enrollment_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "auth/services/auth_service.hpp"
#include "student/services/student_service.hpp"
#include "course/services/course_service.hpp"
#include "attendance/services/attendance_service.hpp"
#include "results/services/result_service.hpp"
#include "enrollment/services/enrollment_service.hpp"
#include "reports/services/report_service.hpp"
#include "auth/controllers/auth_controller.hpp"
#include "reports/controllers/report_controller.hpp"

using namespace ums;

class ReportEndpointTest : public ::testing::Test {
protected:
    void SetUp() override {
        // WHY: We initialize an in-memory database and all the repositories and services
        // needed to create a fully functioning stack for endpoint testing.
        dbManager_ = std::make_unique<DatabaseManager>(":memory:");
        
        userRepo_ = std::make_unique<SQLiteUserRepository>(dbManager_->db());
        userRepo_->initialize();
        studentRepo_ = std::make_unique<SQLiteStudentRepository>(dbManager_->db());
        studentRepo_->initialize();
        deptRepo_ = std::make_unique<SQLiteDepartmentRepository>(dbManager_->db());
        deptRepo_->initialize();
        courseRepo_ = std::make_unique<SQLiteCourseRepository>(dbManager_->db());
        courseRepo_->initialize();
        attendanceRepo_ = std::make_unique<SQLiteAttendanceRepository>(dbManager_->db());
        attendanceRepo_->initialize();
        resultRepo_ = std::make_unique<SQLiteResultRepository>(dbManager_->db());
        resultRepo_->initialize();
        enrollmentRepo_ = std::make_unique<SQLiteEnrollmentRepository>(dbManager_->db());
        enrollmentRepo_->initialize();
        
        passwordService_ = std::make_unique<PasswordService>(4);
        jwtService_ = std::make_unique<JwtService>("test-secret", 3600);
        
        authService_ = std::make_unique<AuthService>(*userRepo_, *passwordService_, *jwtService_);
        studentService_ = std::make_unique<StudentService>(*studentRepo_);
        courseService_ = std::make_unique<CourseService>(*deptRepo_, *courseRepo_);
        attendanceService_ = std::make_unique<AttendanceService>(*attendanceRepo_);
        resultService_ = std::make_unique<ResultService>(*resultRepo_);
        enrollmentService_ = std::make_unique<EnrollmentService>(*enrollmentRepo_, *studentRepo_, *courseRepo_);
        reportService_ = std::make_unique<ReportService>(
            *deptRepo_, *courseRepo_, *studentRepo_,
            *attendanceRepo_, *resultRepo_, *enrollmentRepo_, dbManager_->db()
        );
        
        authController_ = std::make_unique<AuthController>(*authService_, *jwtService_);
        reportController_ = std::make_unique<ReportController>(*reportService_, *jwtService_);
        
        app_ = std::make_unique<crow::SimpleApp>();
        authController_->registerRoutes(*app_);
        reportController_->registerRoutes(*app_);
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
    std::unique_ptr<SQLiteStudentRepository> studentRepo_;
    std::unique_ptr<SQLiteDepartmentRepository> deptRepo_;
    std::unique_ptr<SQLiteCourseRepository> courseRepo_;
    std::unique_ptr<SQLiteAttendanceRepository> attendanceRepo_;
    std::unique_ptr<SQLiteResultRepository> resultRepo_;
    std::unique_ptr<SQLiteEnrollmentRepository> enrollmentRepo_;
    
    std::unique_ptr<PasswordService> passwordService_;
    std::unique_ptr<JwtService> jwtService_;
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<StudentService> studentService_;
    std::unique_ptr<CourseService> courseService_;
    std::unique_ptr<AttendanceService> attendanceService_;
    std::unique_ptr<ResultService> resultService_;
    std::unique_ptr<EnrollmentService> enrollmentService_;
    std::unique_ptr<ReportService> reportService_;
    
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<ReportController> reportController_;
    std::unique_ptr<crow::SimpleApp> app_;
};

// Test GetSystemOverviewAsAdminReturns200
TEST_F(ReportEndpointTest, GetSystemOverviewAsAdminReturns200) {
    auto [adminId, adminToken] = createUserAndGetToken("admin@example.com", Role::Admin);
    
    auto res = makeRequest("GET", "/api/reports/system", "", adminToken);
    EXPECT_EQ(res.code, 200);
}

// Test GetSystemOverviewAsStudentReturns403
TEST_F(ReportEndpointTest, GetSystemOverviewAsStudentReturns403) {
    auto [studentId, studentToken] = createUserAndGetToken("student@example.com", Role::Student);
    
    auto res = makeRequest("GET", "/api/reports/system", "", studentToken);
    EXPECT_EQ(res.code, 403);
}

// Test GetDepartmentReportAsProfessorReturns200
TEST_F(ReportEndpointTest, GetDepartmentReportAsProfessorReturns200) {
    auto [profId, profToken] = createUserAndGetToken("prof@example.com", Role::Professor);
    
    deptRepo_->create("Computer Science", "CS", profId);
    
    auto res = makeRequest("GET", "/api/reports/department/1", "", profToken);
    EXPECT_EQ(res.code, 200);
}

// Test GetCourseAttendanceReportAsProfessorReturns200
TEST_F(ReportEndpointTest, GetCourseAttendanceReportAsProfessorReturns200) {
    auto [profId, profToken] = createUserAndGetToken("prof2@example.com", Role::Professor);
    
    deptRepo_->create("Computer Science", "CS", profId);
    courseRepo_->create(Course::create("CS101", "Intro CS", 1, profId, 3, 1).value());
    
    auto res = makeRequest("GET", "/api/reports/course/1/attendance", "", profToken);
    EXPECT_EQ(res.code, 200);
}

// Test GetGradeDistributionReportAsProfessorReturns200
TEST_F(ReportEndpointTest, GetGradeDistributionReportAsProfessorReturns200) {
    auto [profId, profToken] = createUserAndGetToken("prof3@example.com", Role::Professor);
    
    deptRepo_->create("Computer Science", "CS", profId);
    courseRepo_->create(Course::create("CS101", "Intro CS", 1, profId, 3, 1).value());
    
    auto res = makeRequest("GET", "/api/reports/course/1/grades", "", profToken);
    EXPECT_EQ(res.code, 200);
}

// Test GetReportsWithoutTokenReturns401
TEST_F(ReportEndpointTest, GetReportsWithoutTokenReturns401) {
    auto res = makeRequest("GET", "/api/reports/system", "");
    EXPECT_EQ(res.code, 401);
}
