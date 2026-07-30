#include "results/controllers/result_controller.hpp"
#include "common/json_helpers.hpp"
#include "auth/middleware/auth_middleware.hpp"
#include <utility>

namespace ums {

ResultController::ResultController(ResultService& resultService, JwtService& jwtService)
    : resultService_(resultService), jwtService_(jwtService) {}

void ResultController::registerRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/results").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) { return handleAddResult(req); }
    );
    
    CROW_ROUTE(app, "/api/results/<int>").methods(crow::HTTPMethod::PUT)(
        [this](const crow::request& req, int id) { return handleUpdateResult(req, id); }
    );
    
    CROW_ROUTE(app, "/api/results/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int id) { return handleGetResultById(req, id); }
    );
    
    CROW_ROUTE(app, "/api/results/student/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int studentId) { return handleGetResultsByStudent(req, studentId); }
    );
    
    CROW_ROUTE(app, "/api/results/student/<int>/semester/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int studentId, int semester) { return handleGetResultsByStudentAndSemester(req, studentId, semester); }
    );
    
    CROW_ROUTE(app, "/api/results/course/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int courseId) { return handleGetResultsByCourse(req, courseId); }
    );
    
    CROW_ROUTE(app, "/api/results/cgpa/student/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int studentId) { return handleGetStudentCgpa(req, studentId); }
    );
    
    CROW_ROUTE(app, "/api/results/sgpa/student/<int>/semester/<int>").methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, int studentId, int semester) { return handleGetStudentSgpa(req, studentId, semester); }
    );
}

crow::response ResultController::handleAddResult(const crow::request& req) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    if (!authorize(getValue(authResult), Role::Professor) && !authorize(getValue(authResult), Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only professors or administrators can add results"});
    }

    auto body = crow::json::load(req.body);
    if (!body) return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});

    AddResultRequest addReq;
    if (body.count("studentId") > 0 && body["studentId"].t() == crow::json::type::Number) {
        addReq.studentId = body["studentId"].i();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "studentId is required"});

    if (body.count("courseId") > 0 && body["courseId"].t() == crow::json::type::Number) {
        addReq.courseId = body["courseId"].i();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "courseId is required"});

    if (body.count("examType") > 0 && body["examType"].t() == crow::json::type::String) {
        addReq.examType = body["examType"].s();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "examType is required"});

    if (body.count("marksObtained") > 0 && body["marksObtained"].t() == crow::json::type::Number) {
        addReq.marksObtained = body["marksObtained"].d();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "marksObtained is required"});

    if (body.count("maxMarks") > 0 && body["maxMarks"].t() == crow::json::type::Number) {
        addReq.maxMarks = body["maxMarks"].d();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "maxMarks is required"});

    if (body.count("semester") > 0 && body["semester"].t() == crow::json::type::Number) {
        addReq.semester = body["semester"].i();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "semester is required"});

    auto result = resultService_.addResult(addReq);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(201, successResponse(getValue(result).toJson(), "Result added successfully"));
}

crow::response ResultController::handleUpdateResult(const crow::request& req, int id) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    if (!authorize(getValue(authResult), Role::Professor) && !authorize(getValue(authResult), Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only professors or administrators can update results"});
    }

    auto body = crow::json::load(req.body);
    if (!body) return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});

    UpdateResultRequest updateReq;
    if (body.count("marksObtained") > 0 && body["marksObtained"].t() == crow::json::type::Number) {
        updateReq.marksObtained = body["marksObtained"].d();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "marksObtained is required"});

    if (body.count("maxMarks") > 0 && body["maxMarks"].t() == crow::json::type::Number) {
        updateReq.maxMarks = body["maxMarks"].d();
    } else return makeErrorResponse(AppError{ErrorCode::ValidationError, "maxMarks is required"});

    auto result = resultService_.updateResult(id, updateReq);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(200, successResponse(getValue(result).toJson(), "Result updated successfully"));
}

crow::response ResultController::handleGetResultById(const crow::request& req, int id) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    auto resultResult = resultService_.getResultById(id);
    if (!isSuccess(resultResult)) return makeErrorResponse(getError(resultResult));
    
    return makeResponse(200, successResponse(getValue(resultResult).toJson(), "Result retrieved successfully"));
}

crow::response ResultController::handleGetResultsByStudent(const crow::request& req, int studentId) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    const auto& ctx = getValue(authResult);
    if (ctx.role == Role::Student) {
        if (!authorizeOwner(ctx, studentId)) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "You can only view your own results"});
        }
    } else if (!authorize(ctx, Role::Professor) && !authorize(ctx, Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "You do not have permission to view these results"});
    }

    auto result = resultService_.getResultsByStudent(studentId);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    crow::json::wvalue::list jsonList;
    for (const auto& res : getValue(result)) {
        jsonList.push_back(res.toJson());
    }

    crow::json::wvalue respData;
    respData = std::move(jsonList);
    return makeResponse(200, successResponse(std::move(respData), "Results retrieved successfully"));
}

crow::response ResultController::handleGetResultsByStudentAndSemester(const crow::request& req, int studentId, int semester) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    const auto& ctx = getValue(authResult);
    if (ctx.role == Role::Student) {
        if (!authorizeOwner(ctx, studentId)) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "You can only view your own results"});
        }
    } else if (!authorize(ctx, Role::Professor) && !authorize(ctx, Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "You do not have permission to view these results"});
    }

    auto result = resultService_.getResultsByStudentAndSemester(studentId, semester);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    crow::json::wvalue::list jsonList;
    for (const auto& res : getValue(result)) {
        jsonList.push_back(res.toJson());
    }

    crow::json::wvalue respData;
    respData = std::move(jsonList);
    return makeResponse(200, successResponse(std::move(respData), "Results retrieved successfully"));
}

crow::response ResultController::handleGetResultsByCourse(const crow::request& req, int courseId) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    if (!authorize(getValue(authResult), Role::Professor) && !authorize(getValue(authResult), Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only professors or administrators can view course results"});
    }

    auto result = resultService_.getResultsByCourse(courseId);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    crow::json::wvalue::list jsonList;
    for (const auto& res : getValue(result)) {
        jsonList.push_back(res.toJson());
    }

    crow::json::wvalue respData;
    respData = std::move(jsonList);
    return makeResponse(200, successResponse(std::move(respData), "Results retrieved successfully"));
}

crow::response ResultController::handleGetStudentCgpa(const crow::request& req, int studentId) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    const auto& ctx = getValue(authResult);
    if (ctx.role == Role::Student) {
        if (!authorizeOwner(ctx, studentId)) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "You can only view your own CGPA"});
        }
    } else if (!authorize(ctx, Role::Professor) && !authorize(ctx, Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "You do not have permission to view this CGPA"});
    }

    auto result = resultService_.calculateCgpa(studentId);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(200, successResponse(getValue(result).toJson(), "CGPA calculated successfully"));
}

crow::response ResultController::handleGetStudentSgpa(const crow::request& req, int studentId, int semester) {
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) return makeErrorResponse(getError(authResult));
    
    const auto& ctx = getValue(authResult);
    if (ctx.role == Role::Student) {
        if (!authorizeOwner(ctx, studentId)) {
            return makeErrorResponse(AppError{ErrorCode::Forbidden, "You can only view your own SGPA"});
        }
    } else if (!authorize(ctx, Role::Professor) && !authorize(ctx, Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "You do not have permission to view this SGPA"});
    }

    auto result = resultService_.calculateSgpa(studentId, semester);
    if (!isSuccess(result)) return makeErrorResponse(getError(result));

    return makeResponse(200, successResponse(getValue(result).toJson(), "SGPA calculated successfully"));
}

} // namespace ums
