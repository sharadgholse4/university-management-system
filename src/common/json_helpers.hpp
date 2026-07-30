#pragma once
#include "crow.h"
#include "common/errors.hpp"

namespace ums {

// WHY: These helper functions ensure a consistent JSON structure for all API responses.
// Standardizing responses makes frontend integration much easier.

inline crow::json::wvalue successResponse(crow::json::wvalue data, const std::string& message = "Success") {
    crow::json::wvalue res;
    res["success"] = true;
    res["message"] = message;
    res["data"] = std::move(data);
    return res;
}

inline crow::json::wvalue errorResponse(const AppError& error) {
    crow::json::wvalue res;
    res["success"] = false;
    res["error"]["code"] = error.codeString();
    res["error"]["message"] = error.message;
    return res;
}

inline crow::response makeResponse(int status, crow::json::wvalue body) {
    crow::response res{status, body.dump()};
    res.add_header("Content-Type", "application/json");
    return res;
}

inline crow::response makeErrorResponse(const AppError& error) {
    return makeResponse(error.httpStatus(), errorResponse(error));
}

} // namespace ums
