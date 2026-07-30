#pragma once
#include <string>
#include <optional>
#include "crow.h"
#include "common/types.hpp"
#include "common/errors.hpp"
#include "common/json_helpers.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

// WHY: Contains the extracted, verified identity of the user making the current request.
// This is passed along to handlers to make authorization decisions and link created
// entities to the correct user.
struct RequestContext {
    int userId;
    Role role;
    int subjectId;
};

// Extract Bearer token from Authorization header
// WHY: Follows the standard HTTP OAuth2 Bearer token format (Authorization: Bearer <token>)
inline std::optional<std::string> extractBearerToken(const crow::request& req) {
    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.empty() || authHeader.length() < 7 || authHeader.substr(0, 7) != "Bearer ") {
        return std::nullopt;
    }
    return authHeader.substr(7);
}

// Authenticate a request - returns RequestContext or error response
// WHY: This acts as our authentication middleware layer. It intercepts incoming requests,
// extracts the token, and validates it. If validation fails, it provides a standard AppError.
inline Result<RequestContext> authenticate(const crow::request& req, const JwtService& jwtService) {
    auto token = extractBearerToken(req);
    if (!token) {
        return AppError{ErrorCode::Unauthorized, "Missing or invalid Authorization header"};
    }
    auto payloadResult = jwtService.verifyToken(*token);
    if (!isSuccess(payloadResult)) {
        return getError(payloadResult);
    }
    const auto& payload = getValue(payloadResult);
    return RequestContext{payload.userId, payload.role, payload.subjectId};
}

// Check if user has required role
// WHY: Implements Role-Based Access Control (RBAC).
// Admins represent super-users and are implicitly authorized for all role-based actions.
inline bool authorize(const RequestContext& ctx, Role requiredRole) {
    if (ctx.role == Role::Admin) return true; // Admin can do everything
    return ctx.role == requiredRole;
}

// Check if user owns the resource
// WHY: Implements Resource-Based Access Control.
// Prevents an authenticated Student from modifying another Student's data.
// Admins override this check as well.
inline bool authorizeOwner(const RequestContext& ctx, int resourceOwnerId) {
    if (ctx.role == Role::Admin) return true;
    return ctx.userId == resourceOwnerId;
}

} // namespace ums
