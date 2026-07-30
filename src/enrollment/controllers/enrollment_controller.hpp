#pragma once
#include "crow.h"
#include "enrollment/services/enrollment_service.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

class EnrollmentController {
public:
    EnrollmentController(EnrollmentService& enrollmentService, JwtService& jwtService);
    void registerRoutes(crow::SimpleApp& app);

private:
    EnrollmentService& enrollmentService_;
    JwtService& jwtService_;
};

} // namespace ums
