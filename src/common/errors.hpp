#pragma once
#include <string>

namespace ums {

// WHY: A standardized error enumeration maps cleanly to HTTP status codes
// while keeping our core logic agnostic of the web framework (Crow).
enum class ErrorCode {
    ValidationError,
    NotFound,
    Unauthorized,
    Forbidden,
    Conflict,
    InternalError
};

// AppError structure encapsulating the type and human-readable message.
// WHY: It simplifies error passing between the repository layer, service layer, and controller layer.
struct AppError {
    ErrorCode code;
    std::string message;

    int httpStatus() const {
        switch (code) {
            case ErrorCode::ValidationError: return 400;
            case ErrorCode::NotFound: return 404;
            case ErrorCode::Unauthorized: return 401;
            case ErrorCode::Forbidden: return 403;
            case ErrorCode::Conflict: return 409;
            case ErrorCode::InternalError: return 500;
        }
        return 500;
    }

    std::string codeString() const {
        switch (code) {
            case ErrorCode::ValidationError: return "VALIDATION_ERROR";
            case ErrorCode::NotFound: return "NOT_FOUND";
            case ErrorCode::Unauthorized: return "UNAUTHORIZED";
            case ErrorCode::Forbidden: return "FORBIDDEN";
            case ErrorCode::Conflict: return "CONFLICT";
            case ErrorCode::InternalError: return "INTERNAL_ERROR";
        }
        return "UNKNOWN_ERROR";
    }
};

} // namespace ums
