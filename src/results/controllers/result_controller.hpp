#pragma once
#include "crow.h"
#include "results/services/result_service.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

class ResultController {
public:
    ResultController(ResultService& resultService, JwtService& jwtService);
    
    void registerRoutes(crow::SimpleApp& app);

private:
    ResultService& resultService_;
    JwtService& jwtService_;

    crow::response handleAddResult(const crow::request& req);
    crow::response handleUpdateResult(const crow::request& req, int id);
    crow::response handleGetResultById(const crow::request& req, int id);
    crow::response handleGetResultsByStudent(const crow::request& req, int studentId);
    crow::response handleGetResultsByStudentAndSemester(const crow::request& req, int studentId, int semester);
    crow::response handleGetResultsByCourse(const crow::request& req, int courseId);
    crow::response handleGetStudentCgpa(const crow::request& req, int studentId);
    crow::response handleGetStudentSgpa(const crow::request& req, int studentId, int semester);
};

} // namespace ums
