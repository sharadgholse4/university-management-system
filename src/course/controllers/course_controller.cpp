#include "course/controllers/course_controller.hpp"
#include "common/json_helpers.hpp"
#include "auth/middleware/auth_middleware.hpp"
#include <utility>

namespace ums {

CourseController::CourseController(CourseService& courseService, JwtService& jwtService)
    : courseService_(courseService), jwtService_(jwtService) {}

void CourseController::registerRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/departments").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
        auto& ctx = getValue(authResult);

        if (!authorize(ctx, Role::Admin)) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Admin access required"});
        }

        auto body = crow::json::load(req.body);
        if (!body || body.count("name") == 0 || body.count("code") == 0) {
            return makeErrorResponse(AppError{ErrorCode::ValidationError, "Missing required fields"});
        }

        CreateDepartmentRequest createReq;
        createReq.name = body["name"].s();
        createReq.code = body["code"].s();
        createReq.headProfessorId = body.count("headProfessorId") > 0 ? body["headProfessorId"].i() : 0;

        auto result = courseService_.createDepartment(createReq);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));
        return makeResponse(201, successResponse(getValue(result).toJson(), "Department created"));
    });

    CROW_ROUTE(app, "/api/departments").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

        auto result = courseService_.getAllDepartments();
        if (!isSuccess(result)) return makeErrorResponse(getError(result));

        std::vector<crow::json::wvalue> deptsJson;
        for (const auto& dept : getValue(result)) {
            deptsJson.push_back(dept.toJson());
        }
        return makeResponse(200, successResponse(std::move(deptsJson)));
    });

    CROW_ROUTE(app, "/api/departments/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, int id) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

        auto result = courseService_.getDepartmentById(id);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));
        return makeResponse(200, successResponse(getValue(result).toJson()));
    });

    CROW_ROUTE(app, "/api/departments/<int>").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, int id) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
        auto& ctx = getValue(authResult);

        if (!authorize(ctx, Role::Admin)) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Admin access required"});
        }

        auto body = crow::json::load(req.body);
        if (!body || body.count("name") == 0 || body.count("code") == 0) {
            return makeErrorResponse(AppError{ErrorCode::ValidationError, "Missing required fields"});
        }

        UpdateDepartmentRequest updateReq;
        updateReq.name = body["name"].s();
        updateReq.code = body["code"].s();
        updateReq.headProfessorId = body.count("headProfessorId") > 0 ? body["headProfessorId"].i() : 0;

        auto result = courseService_.updateDepartment(id, updateReq);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));
        return makeResponse(200, successResponse(getValue(result).toJson(), "Department updated"));
    });

    CROW_ROUTE(app, "/api/courses").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
        auto& ctx = getValue(authResult);

        if (!authorize(ctx, Role::Admin)) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Admin access required"});
        }

        auto body = crow::json::load(req.body);
        if (!body || body.count("code") == 0 || body.count("name") == 0 || 
            body.count("departmentId") == 0 || body.count("professorId") == 0 || 
            body.count("credits") == 0 || body.count("semester") == 0) {
            return makeErrorResponse(AppError{ErrorCode::ValidationError, "Missing required fields"});
        }

        CreateCourseRequest createReq;
        createReq.code = body["code"].s();
        createReq.name = body["name"].s();
        createReq.departmentId = body["departmentId"].i();
        createReq.professorId = body["professorId"].i();
        createReq.credits = body["credits"].i();
        createReq.semester = body["semester"].i();

        auto result = courseService_.createCourse(createReq);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));
        return makeResponse(201, successResponse(getValue(result).toJson(), "Course created"));
    });

    CROW_ROUTE(app, "/api/courses").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

        auto result = courseService_.getAllCourses();
        if (!isSuccess(result)) return makeErrorResponse(getError(result));

        std::vector<crow::json::wvalue> coursesJson;
        for (const auto& course : getValue(result)) {
            coursesJson.push_back(course.toJson());
        }
        return makeResponse(200, successResponse(std::move(coursesJson)));
    });

    CROW_ROUTE(app, "/api/courses/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, int id) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

        auto result = courseService_.getCourseById(id);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));
        return makeResponse(200, successResponse(getValue(result).toJson()));
    });

    CROW_ROUTE(app, "/api/courses/<int>").methods(crow::HTTPMethod::PUT)
    ([this](const crow::request& req, int id) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
        auto& ctx = getValue(authResult);

        // Fetch course to check ownership if professor
        auto courseResult = courseService_.getCourseById(id);
        if (!isSuccess(courseResult)) return makeErrorResponse(getError(courseResult));
        
        if (ctx.role == Role::Professor) {
            if (getValue(courseResult).professorId() != ctx.userId) {
                return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only the assigned professor or an admin can update this course"});
            }
        } else if (ctx.role != Role::Admin) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "Professor or Admin access required"});
        }

        auto body = crow::json::load(req.body);
        if (!body || body.count("name") == 0 || body.count("professorId") == 0 || 
            body.count("credits") == 0 || body.count("semester") == 0) {
            return makeErrorResponse(AppError{ErrorCode::ValidationError, "Missing required fields"});
        }

        UpdateCourseRequest updateReq;
        updateReq.name = body["name"].s();
        updateReq.professorId = body["professorId"].i();
        updateReq.credits = body["credits"].i();
        updateReq.semester = body["semester"].i();

        auto result = courseService_.updateCourse(id, updateReq);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));
        return makeResponse(200, successResponse(getValue(result).toJson(), "Course updated"));
    });

    CROW_ROUTE(app, "/api/courses/department/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, int departmentId) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

        auto result = courseService_.getCoursesByDepartment(departmentId);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));

        std::vector<crow::json::wvalue> coursesJson;
        for (const auto& course : getValue(result)) {
            coursesJson.push_back(course.toJson());
        }
        return makeResponse(200, successResponse(std::move(coursesJson)));
    });

    CROW_ROUTE(app, "/api/courses/professor/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, int professorId) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

        auto result = courseService_.getCoursesByProfessor(professorId);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));

        std::vector<crow::json::wvalue> coursesJson;
        for (const auto& course : getValue(result)) {
            coursesJson.push_back(course.toJson());
        }
        return makeResponse(200, successResponse(std::move(coursesJson)));
    });

    CROW_ROUTE(app, "/api/courses/semester/<int>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, int semester) {
        auto authResult = authenticate(req, jwtService_);
        if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

        auto result = courseService_.getCoursesBySemester(semester);
        if (!isSuccess(result)) return makeErrorResponse(getError(result));

        std::vector<crow::json::wvalue> coursesJson;
        for (const auto& course : getValue(result)) {
            coursesJson.push_back(course.toJson());
        }
        return makeResponse(200, successResponse(std::move(coursesJson)));
    });
}

} // namespace ums
