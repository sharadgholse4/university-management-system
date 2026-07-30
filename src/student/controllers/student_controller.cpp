#include "student/controllers/student_controller.hpp"
#ifdef DELETE
#undef DELETE
#endif
#include "common/json_helpers.hpp"
#include "auth/middleware/auth_middleware.hpp"

namespace ums {

StudentController::StudentController(StudentService& studentService, JwtService& jwtService)
    : studentService_(studentService), jwtService_(jwtService) {}

void StudentController::registerRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/students").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req) { return handleGetAllStudents(req); }
    );
    
    CROW_ROUTE(app, "/api/students/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int id) { return handleGetStudentById(req, id); }
    );
    
    CROW_ROUTE(app, "/api/students").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) { return handleCreateStudent(req); }
    );
    
    CROW_ROUTE(app, "/api/students/<int>").methods(crow::HTTPMethod::PUT)(
        [this](const crow::request& req, int id) { return handleUpdateStudent(req, id); }
    );
    
    CROW_ROUTE(app, "/api/students/department/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int deptId) { return handleGetStudentsByDepartment(req, deptId); }
    );
}

crow::response StudentController::handleGetAllStudents(const crow::request& req) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    if (!authorize(getValue(authResult), Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only administrators can view all students"});
    }

    auto result = studentService_.getAllStudents();
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    crow::json::wvalue::list jsonList;
    for (const auto& student : getValue(result)) {
        jsonList.push_back(student.toJson());
    }

    crow::json::wvalue respData;
    respData = std::move(jsonList);
    return makeResponse(200, successResponse(std::move(respData), "Students retrieved successfully"));
}

crow::response StudentController::handleGetStudentById(const crow::request& req, int id) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    const auto& ctx = getValue(authResult);
    auto studentResult = studentService_.getStudentById(id);
    if (!isSuccess(studentResult)) return makeErrorResponse(getError(studentResult));
    
    const auto& student = getValue(studentResult);
    if (!authorizeOwner(ctx, student.userId())) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "You do not have permission to view this student profile"});
    }

    return makeResponse(200, successResponse(student.toJson(), "Student retrieved successfully"));
}

crow::response StudentController::handleCreateStudent(const crow::request& req) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    if (!authorize(getValue(authResult), Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only administrators can create students"});
    }

    auto body = crow::json::load(req.body);
    if (!body) return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});

    CreateStudentRequest createReq;
    if (body.count("userId") > 0 && body["userId"].t() == crow::json::type::Number) {
        createReq.userId = body["userId"].i();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "userId is required"});

    if (body.count("rollNumber") > 0 && body["rollNumber"].t() == crow::json::type::String) {
        createReq.rollNumber = body["rollNumber"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "rollNumber is required"});

    if (body.count("firstName") > 0 && body["firstName"].t() == crow::json::type::String) {
        createReq.firstName = body["firstName"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "firstName is required"});

    if (body.count("lastName") > 0 && body["lastName"].t() == crow::json::type::String) {
        createReq.lastName = body["lastName"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "lastName is required"});

    createReq.departmentId = (body.count("departmentId") > 0 && body["departmentId"].t() == crow::json::type::Number) ? body["departmentId"].i() : 0;
    
    if (body.count("semester") > 0 && body["semester"].t() == crow::json::type::Number) {
        createReq.semester = body["semester"].i();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "semester is required"});

    auto result = studentService_.createStudent(createReq);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(201, successResponse(getValue(result).toJson(), "Student created successfully"));
}

crow::response StudentController::handleUpdateStudent(const crow::request& req, int id) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));

    auto studentResult = studentService_.getStudentById(id);
    if (!isSuccess(studentResult)) return makeErrorResponse(getError(studentResult));
    
    if (!authorizeOwner(getValue(authResult), getValue(studentResult).userId())) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "You do not have permission to update this student profile"});
    }

    auto body = crow::json::load(req.body);
    if (!body) return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});

    UpdateStudentRequest updateReq;
    if (body.count("firstName") > 0 && body["firstName"].t() == crow::json::type::String) {
        updateReq.firstName = body["firstName"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "firstName is required"});

    if (body.count("lastName") > 0 && body["lastName"].t() == crow::json::type::String) {
        updateReq.lastName = body["lastName"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "lastName is required"});

    updateReq.departmentId = (body.count("departmentId") > 0 && body["departmentId"].t() == crow::json::type::Number) ? body["departmentId"].i() : 0;
    
    if (body.count("semester") > 0 && body["semester"].t() == crow::json::type::Number) {
        updateReq.semester = body["semester"].i();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "semester is required"});

    auto result = studentService_.updateStudent(id, updateReq);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(200, successResponse(getValue(result).toJson(), "Student updated successfully"));
}

crow::response StudentController::handleGetStudentsByDepartment(const crow::request& req, int deptId) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    if (!authorize(getValue(authResult), Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only administrators can view students by department"});
    }

    auto result = studentService_.getStudentsByDepartment(deptId);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    crow::json::wvalue::list jsonList;
    for (const auto& student : getValue(result)) {
        jsonList.push_back(student.toJson());
    }

    crow::json::wvalue respData;
    respData = std::move(jsonList);
    return makeResponse(200, successResponse(std::move(respData), "Students retrieved successfully"));
}

} // namespace ums
