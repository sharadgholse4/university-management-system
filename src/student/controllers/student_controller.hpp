#pragma once
#include "crow.h"
#include "student/services/student_service.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

class StudentController {
public:
    StudentController(StudentService& studentService, JwtService& jwtService);
    void registerRoutes(crow::SimpleApp& app);

private:
    StudentService& studentService_;
    JwtService& jwtService_;

    crow::response handleGetAllStudents(const crow::request& req);
    crow::response handleGetStudentById(const crow::request& req, int id);
    crow::response handleCreateStudent(const crow::request& req);
    crow::response handleUpdateStudent(const crow::request& req, int id);
    crow::response handleGetStudentsByDepartment(const crow::request& req, int departmentId);
};

} // namespace ums
