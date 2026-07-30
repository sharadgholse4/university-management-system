#pragma once
#include "crow.h"
#include "auth/services/auth_service.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

// WHY: Controllers define the HTTP interface for the application.
// They parse HTTP requests, invoke the appropriate service logic,
// and format the result back into an HTTP response.
// This is the "Thin Controller" pattern.
class AuthController {
public:
    AuthController(AuthService& authService, JwtService& jwtService);
    
    // WHY: Binds controller methods to specific HTTP routes and verbs in the Crow app.
    void registerRoutes(crow::SimpleApp& app);

private:
    AuthService& authService_;
    JwtService& jwtService_;

    crow::response handleLogin(const crow::request& req);
    crow::response handleRegister(const crow::request& req);
};

} // namespace ums
