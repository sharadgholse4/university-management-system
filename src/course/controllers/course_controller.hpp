#pragma once
#include "crow.h"
#include "course/services/course_service.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

class CourseController {
public:
    CourseController(CourseService& courseService, JwtService& jwtService);
    void registerRoutes(crow::SimpleApp& app);

private:
    CourseService& courseService_;
    JwtService& jwtService_;
};

} // namespace ums
