#include "auth/controllers/auth_controller.hpp"
#include "common/json_helpers.hpp"
#include "auth/middleware/auth_middleware.hpp"
#include <iostream>

namespace ums {

AuthController::AuthController(AuthService& authService, JwtService& jwtService)
    : authService_(authService), jwtService_(jwtService) {}

void AuthController::registerRoutes(crow::SimpleApp& app) {
    // WHY: Using CROW_ROUTE macros to bind URLs and HTTP methods to our class methods.
    // The lambda wrapper captures 'this' to invoke member functions.
    
    CROW_ROUTE(app, "/api/auth/login").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) { return handleLogin(req); }
    );

    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) { return handleRegister(req); }
    );
}

crow::response AuthController::handleLogin(const crow::request& req) {
    // Parse JSON body
    auto body = crow::json::load(req.body);
    if (!body) {
        return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});
    }

    // Extract fields
    LoginRequest loginReq;
    if (body.count("email") > 0 && body["email"].t() == crow::json::type::String) {
        loginReq.email = body["email"].s();
    }
    if (body.count("password") > 0 && body["password"].t() == crow::json::type::String) {
        loginReq.password = body["password"].s();
    }

    // Delegate to service
    auto result = authService_.login(loginReq);
    if (!isSuccess(result)) {
        return makeErrorResponse(getError(result));
    }

    // Format successful response
    const auto& authResponse = getValue(result);
    crow::json::wvalue respData;
    respData["user"] = authResponse.user.toJson();
    respData["token"] = authResponse.token;
    
    return makeResponse(200, successResponse(std::move(respData), "Login successful"));
}

crow::response AuthController::handleRegister(const crow::request& req) {
    // 1. Authenticate the request
    // WHY: We restrict registration to authorized users (Admins) rather than making it public.
    auto authResult = authenticate(req, jwtService_);
    if (!isSuccess(authResult)) {
        return makeErrorResponse(getError(authResult));
    }
    
    // 2. Authorize the user (must be admin)
    const auto& ctx = getValue(authResult);
    if (!authorize(ctx, Role::Admin)) {
        return makeErrorResponse(AppError{ErrorCode::Forbidden, "Only administrators can register new users"});
    }

    // 3. Parse inputs
    auto body = crow::json::load(req.body);
    if (!body) {
        return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid JSON body"});
    }

    RegisterRequest regReq;
    if (body.count("email") > 0 && body["email"].t() == crow::json::type::String) {
        regReq.email = body["email"].s();
    }
    if (body.count("password") > 0 && body["password"].t() == crow::json::type::String) {
        regReq.password = body["password"].s();
    }
    if (body.count("role") > 0 && body["role"].t() == crow::json::type::String) {
        try {
            regReq.role = stringToRole(body["role"].s());
        } catch (const std::invalid_argument&) {
            return makeErrorResponse(AppError{ErrorCode::ValidationError, "Invalid role value"});
        }
    } else {
        return makeErrorResponse(AppError{ErrorCode::ValidationError, "Role is required"});
    }

    // 4. Delegate to service
    auto result = authService_.registerUser(regReq);
    if (!isSuccess(result)) {
        return makeErrorResponse(getError(result));
    }

    // 5. Format response
    return makeResponse(201, successResponse(getValue(result).toJson(), "User registered successfully"));
}

} // namespace ums
