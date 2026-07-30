#include "attendance/controllers/attendance_controller.hpp"
#include "common/json_helpers.hpp"
#include "auth/middleware/auth_middleware.hpp"
#include <utility>

namespace ums {

AttendanceController::AttendanceController(AttendanceService& attendanceService, JwtService& jwtService)
    : attendanceService_(attendanceService), jwtService_(jwtService) {}

void AttendanceController::registerRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/attendance/course/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int courseId) { return handleGetAttendanceByCourse(req, courseId); }
    );
    
    CROW_ROUTE(app, "/api/attendance/student/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int studentId) { return handleGetAttendanceByStudent(req, studentId); }
    );

    CROW_ROUTE(app, "/api/attendance/student/<int>/course/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int studentId, int courseId) { return handleGetAttendanceByStudentAndCourse(req, studentId, courseId); }
    );
    
    CROW_ROUTE(app, "/api/attendance").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) { return handleMarkAttendance(req); }
    );
    
    CROW_ROUTE(app, "/api/attendance/<int>").methods(crow::HTTPMethod::PUT)(
        [this](const crow::request& req, int id) { return handleUpdateAttendance(req, id); }
    );

    CROW_ROUTE(app, "/api/attendance/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int id) { return handleGetAttendanceById(req, id); }
    );
}

crow::response AttendanceController::handleGetAttendanceByCourse(const crow::request& req, int courseId) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    // For simplicity, professor role requires authentication (admin also passes).
    if (!authorize(getValue(authResult), Role::Professor)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only professors or admins can view course attendance"});
    }

    std::string dateStr;
    if (req.url_params.get("date") != nullptr) {
        dateStr = req.url_params.get("date");
    }

    Result<std::vector<Attendance>> result = std::vector<Attendance>{};
    if (!dateStr.empty()) {
        result = attendanceService_.getAttendanceByCourseAndDate(courseId, dateStr);
    } else {
        result = attendanceService_.getAttendanceByCourse(courseId);
    }

    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    crow::json::wvalue::list jsonList;
    for (const auto& record : getValue(result)) {
        jsonList.push_back(record.toJson());
    }

    crow::json::wvalue respData;
    respData = std::move(jsonList);
    return makeResponse(200, successResponse(std::move(respData), "Course attendance retrieved successfully"));
}

crow::response AttendanceController::handleGetAttendanceByStudent(const crow::request& req, int studentId) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    const auto& ctx = getValue(authResult);
    
    // TEMPORARY SIMPLIFICATION: student accessing this endpoint is accepted if authenticated.
    // In a full implementation, we'd check if the ctx.subjectId (user's ID) matches the student's user ID 
    // from the students table, but we don't have access to the studentRepo here.
    if (ctx.role == Role::Student) {
        // Here we just allow the student to view, ideally we'd tighten this.
    } else if (!authorize(ctx, Role::Professor)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "You do not have permission to view this attendance"});
    }

    auto result = attendanceService_.getAttendanceByStudent(studentId);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    crow::json::wvalue::list jsonList;
    for (const auto& record : getValue(result)) {
        jsonList.push_back(record.toJson());
    }

    crow::json::wvalue respData;
    respData = std::move(jsonList);
    return makeResponse(200, successResponse(std::move(respData), "Student attendance retrieved successfully"));
}

crow::response AttendanceController::handleGetAttendanceByStudentAndCourse(const crow::request& req, int studentId, int courseId) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    const auto& ctx = getValue(authResult);
    
    if (ctx.role == Role::Student) {
        // Simplified check, same as above
    } else if (!authorize(ctx, Role::Professor)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "You do not have permission to view this attendance"});
    }

    auto result = attendanceService_.getAttendanceByStudentAndCourse(studentId, courseId);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    crow::json::wvalue::list jsonList;
    for (const auto& record : getValue(result)) {
        jsonList.push_back(record.toJson());
    }

    crow::json::wvalue respData;
    respData = std::move(jsonList);
    return makeResponse(200, successResponse(std::move(respData), "Student course attendance retrieved successfully"));
}

crow::response AttendanceController::handleMarkAttendance(const crow::request& req) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    const auto& ctx = getValue(authResult);
    if (!authorize(ctx, Role::Professor)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only professors or admins can mark attendance"});
    }

    auto body = crow::json::load(req.body);
    if (!body) return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});

    MarkAttendanceRequest markReq;
    if (body.count("studentId") > 0 && body["studentId"].t() == crow::json::type::Number) {
        markReq.studentId = body["studentId"].i();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "studentId is required"});

    if (body.count("courseId") > 0 && body["courseId"].t() == crow::json::type::Number) {
        markReq.courseId = body["courseId"].i();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "courseId is required"});

    if (body.count("date") > 0 && body["date"].t() == crow::json::type::String) {
        markReq.date = body["date"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "date is required"});

    if (body.count("status") > 0 && body["status"].t() == crow::json::type::String) {
        markReq.status = body["status"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "status is required"});

    // The user marking attendance is the one currently authenticated
    markReq.markedBy = ctx.userId;

    auto result = attendanceService_.markAttendance(markReq);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(201, successResponse(getValue(result).toJson(), "Attendance marked successfully"));
}

crow::response AttendanceController::handleUpdateAttendance(const crow::request& req, int id) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    if (!authorize(getValue(authResult), Role::Professor)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only professors or admins can update attendance"});
    }

    auto body = crow::json::load(req.body);
    if (!body) return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});

    UpdateAttendanceRequest updateReq;
    if (body.count("status") > 0 && body["status"].t() == crow::json::type::String) {
        updateReq.status = body["status"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "status is required"});

    auto result = attendanceService_.updateAttendance(id, updateReq);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(200, successResponse(getValue(result).toJson(), "Attendance updated successfully"));
}

crow::response AttendanceController::handleGetAttendanceById(const crow::request& req, int id) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    // Any authenticated user can view (this would ideally have more checks)
    auto result = attendanceService_.getAttendanceById(id);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(200, successResponse(getValue(result).toJson(), "Attendance retrieved successfully"));
}

} // namespace ums
