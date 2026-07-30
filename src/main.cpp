#include "crow.h"
#include "common/env.hpp"
#include "database/database_manager.hpp"
// Module 1: Auth
#include "auth/repositories/sqlite_user_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"
#include "auth/services/auth_service.hpp"
#include "auth/controllers/auth_controller.hpp"
// Module 2: Student Profile
#include "student/repositories/sqlite_student_repository.hpp"
#include "student/services/student_service.hpp"
#include "student/controllers/student_controller.hpp"
// Module 3: Attendance
#include "attendance/repositories/sqlite_attendance_repository.hpp"
#include "attendance/services/attendance_service.hpp"
#include "attendance/controllers/attendance_controller.hpp"
// Module 4: Results / CGPA
#include "results/repositories/sqlite_result_repository.hpp"
#include "results/services/result_service.hpp"
#include "results/controllers/result_controller.hpp"
// Module 5: Courses & Departments
#include "course/repositories/sqlite_department_repository.hpp"
#include "course/repositories/sqlite_course_repository.hpp"
#include "course/services/course_service.hpp"
#include "course/controllers/course_controller.hpp"
// Module 6: Enrollments
#include "enrollment/repositories/sqlite_enrollment_repository.hpp"
#include "enrollment/services/enrollment_service.hpp"
#include "enrollment/controllers/enrollment_controller.hpp"
// Module 7: Analytics & Reports
#include "reports/services/report_service.hpp"
#include "reports/controllers/report_controller.hpp"
#include <iostream>

int main() {
    try {
        // Load configuration from environment variables with sensible defaults
        // WHY: Configuration should be externalized (12-factor app principles)
        // so we don't have to recompile to change settings.
        auto dbPath = ums::getEnv("DB_PATH", "./university.db");
        auto jwtSecret = ums::getEnv("JWT_SECRET", "dev-secret-change-in-production");
        auto jwtExpiry = std::stoi(ums::getEnv("JWT_EXPIRY_SECONDS", "3600"));
        auto port = std::stoi(ums::getEnv("SERVER_PORT", "8080"));
        auto bcryptCost = std::stoi(ums::getEnv("BCRYPT_COST", "12"));

        // Initialize database 
        // WHY: RAII (Resource Acquisition Is Initialization) pattern ensures the 
        // connection is automatically cleaned up when it goes out of scope.
        ums::DatabaseManager dbManager(dbPath);

        // =====================================================================
        // Module 1: Auth — repositories, services, controllers
        // =====================================================================
        ums::SQLiteUserRepository userRepo(dbManager.db());
        userRepo.initialize();

        ums::PasswordService passwordService(bcryptCost);
        ums::JwtService jwtService(jwtSecret, jwtExpiry);
        ums::AuthService authService(userRepo, passwordService, jwtService);
        ums::AuthController authController(authService, jwtService);

        // =====================================================================
        // Module 2: Student Profile — repositories, services, controllers
        // =====================================================================
        ums::SQLiteStudentRepository studentRepo(dbManager.db());
        studentRepo.initialize();

        ums::StudentService studentService(studentRepo);
        ums::StudentController studentController(studentService, jwtService);

        // =====================================================================
        // Module 3: Attendance — repositories, services, controllers
        // =====================================================================
        ums::SQLiteAttendanceRepository attendanceRepo(dbManager.db());
        attendanceRepo.initialize();

        ums::AttendanceService attendanceService(attendanceRepo);
        ums::AttendanceController attendanceController(attendanceService, jwtService);

        // =====================================================================
        // Module 4: Results / CGPA — repositories, services, controllers
        // =====================================================================
        ums::SQLiteResultRepository resultRepo(dbManager.db());
        resultRepo.initialize();

        ums::ResultService resultService(resultRepo);
        ums::ResultController resultController(resultService, jwtService);

        // =====================================================================
        // Module 5: Courses & Departments — repositories, services, controllers
        // =====================================================================
        ums::SQLiteDepartmentRepository deptRepo(dbManager.db());
        deptRepo.initialize();

        ums::SQLiteCourseRepository courseRepo(dbManager.db());
        courseRepo.initialize();

        ums::CourseService courseService(deptRepo, courseRepo);
        ums::CourseController courseController(courseService, jwtService);

        // =====================================================================
        // Module 6: Enrollments — repositories, services, controllers
        // =====================================================================
        ums::SQLiteEnrollmentRepository enrollmentRepo(dbManager.db());
        enrollmentRepo.initialize();

        ums::EnrollmentService enrollmentService(enrollmentRepo, studentRepo, courseRepo);
        ums::EnrollmentController enrollmentController(enrollmentService, jwtService);

        // =====================================================================
        // Module 7: Analytics & Reports — services, controllers
        // =====================================================================
        ums::ReportService reportService(deptRepo, courseRepo, studentRepo, attendanceRepo, resultRepo, enrollmentRepo, dbManager.db());
        ums::ReportController reportController(reportService, jwtService);

        // =====================================================================
        // Create Crow app and register all module routes
        // =====================================================================
        crow::SimpleApp app;
        authController.registerRoutes(app);
        studentController.registerRoutes(app);
        attendanceController.registerRoutes(app);
        resultController.registerRoutes(app);
        courseController.registerRoutes(app);
        enrollmentController.registerRoutes(app);
        reportController.registerRoutes(app);

        // Health check endpoint
        // WHY: Essential for container orchestration systems (like Kubernetes) 
        // or load balancers to verify the service is running and ready.
        CROW_ROUTE(app, "/api/health")([]() {
            crow::json::wvalue json;
            json["status"] = "ok";
            json["service"] = "University Management System";
            return crow::response(200, json);
        });

        std::cout << "University Management System starting on port " << port << std::endl;
        
        // Start the server
        // WHY: multithreaded() enables Crow's built-in thread pool to handle concurrent requests.
        app.port(port).multithreaded().run();
    } catch (const std::exception& e) {
        // WHY: Catching std::exception at the top level ensures we log fatal errors 
        // instead of crashing silently.
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
