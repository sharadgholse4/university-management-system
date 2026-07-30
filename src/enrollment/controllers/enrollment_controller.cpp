#include "enrollment/controllers/enrollment_controller.hpp"
#ifdef DELETE
#undef DELETE
#endif
#include "common/json_helpers.hpp"
#include "auth/middleware/auth_middleware.hpp"
#include <utility>
#include <vector>

namespace ums {

EnrollmentController::EnrollmentController(EnrollmentService& enrollmentService, JwtService& jwtService)
    : enrollmentService_(enrollmentService), jwtService_(jwtService) {}

void EnrollmentController::registerRoutes(crow::SimpleApp& app) {
    
    // POST /api/enrollments
    // WHY: Allows a student to enroll themselves, or an admin to enroll any student.
    CROW_ROUTE(app, "/api/enrollments").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
        const auto& ctx = getValue(authResult);

        auto body = crow::json::load(req.body);
        if (!body || body.count("studentId") == 0 || body.count("courseId") == 0) {
            return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});
        }

        EnrollStudentRequest request{
            static_cast<int>(body["studentId"].i()),
            static_cast<int>(body["courseId"].i())
        };

        // RBAC: Student can only enroll themselves. Admin can enroll anyone.
        if (ctx.role == Role::Student && ctx.subjectId != request.studentId) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Cannot enroll another student"});
        } else if (ctx.role != Role::Student && ctx.role != Role::Admin) {
            // Professors cannot enroll students
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Insufficient permissions"});
        }

        auto result = enrollmentService_.enrollStudent(request);
        if (!isSuccess(result)) {
            return makeErrorResponse(getError(result));
        }

        return makeResponse(201, successResponse(getValue(result).toJson(), "Enrollment successful"));
    });

    // DELETE /api/enrollments/student/<int>/course/<int>
    // WHY: Allows a student to drop a course, or an admin to drop a student.
    CROW_ROUTE(app, "/api/enrollments/student/<int>/course/<int>").methods(crow::HTTPMethod::DELETE)
    ([this](const crow::request& req, int studentId, int courseId) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
        const auto& ctx = getValue(authResult);

        // RBAC: Student can only drop themselves. Admin can drop anyone.
        if (ctx.role == Role::Student && ctx.subjectId != studentId) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Cannot drop another student"});
        } else if (ctx.role != Role::Student && ctx.role != Role::Admin) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Insufficient permissions"});
        }

        auto result = enrollmentService_.dropEnrollment(studentId, courseId);
        if (!isSuccess(result)) {
            return makeErrorResponse(getError(result));
        }

        return makeResponse(200, successResponse(crow::json::wvalue(), "Enrollment dropped successfully"));
    });

    // GET /api/enrollments/<int>
    // WHY: Fetch a specific enrollment by ID. Any authenticated user can access.
    CROW_ROUTE(app, "/api/enrollments/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, int id) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

        auto result = enrollmentService_.getEnrollmentById(id);
        if (!isSuccess(result)) {
            return makeErrorResponse(getError(result));
        }

        return makeResponse(200, successResponse(getValue(result).toJson()));
    });

    // GET /api/enrollments/student/<int>
    // WHY: Fetch all enrollments for a specific student. 
    // RBAC: The student themselves, a Professor, or an Admin can access this.
    CROW_ROUTE(app, "/api/enrollments/student/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, int studentId) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
        const auto& ctx = getValue(authResult);

        if (ctx.role == Role::Student && ctx.subjectId != studentId) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Cannot view another student's enrollments"});
        }

        auto result = enrollmentService_.getEnrollmentsByStudent(studentId);
        if (!isSuccess(result)) {
            return makeErrorResponse(getError(result));
        }

        std::vector<crow::json::wvalue> jsonList;
        for (const auto& enrollment : getValue(result)) {
            jsonList.push_back(enrollment.toJson());
        }

        return makeResponse(200, successResponse(std::move(jsonList)));
    });

    // GET /api/enrollments/course/<int>
    // WHY: Fetch all enrollments for a specific course.
    // RBAC: Only Professors or Admins can access this.
    CROW_ROUTE(app, "/api/enrollments/course/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, int courseId) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
        const auto& ctx = getValue(authResult);

        if (!authorize(ctx, Role::Professor)) { // authorize allows Admin implicitly
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only professors or admins can view course enrollments"});
        }

        auto result = enrollmentService_.getEnrollmentsByCourse(courseId);
        if (!isSuccess(result)) {
            return makeErrorResponse(getError(result));
        }

        std::vector<crow::json::wvalue> jsonList;
        for (const auto& enrollment : getValue(result)) {
            jsonList.push_back(enrollment.toJson());
        }

        return makeResponse(200, successResponse(std::move(jsonList)));
    });
}

} // namespace ums
