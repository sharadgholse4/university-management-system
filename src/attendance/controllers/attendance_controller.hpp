#pragma once
#include "crow.h"
#include "attendance/services/attendance_service.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

class AttendanceController {
public:
    AttendanceController(AttendanceService& attendanceService, JwtService& jwtService);
    void registerRoutes(crow::SimpleApp& app);

private:
    AttendanceService& attendanceService_;
    JwtService& jwtService_;

    crow::response handleGetAttendanceByCourse(const crow::request& req, int courseId);
    crow::response handleGetAttendanceByStudent(const crow::request& req, int studentId);
    crow::response handleGetAttendanceByStudentAndCourse(const crow::request& req, int studentId, int courseId);
    crow::response handleMarkAttendance(const crow::request& req);
    crow::response handleUpdateAttendance(const crow::request& req, int id);
    crow::response handleGetAttendanceById(const crow::request& req, int id);
};

} // namespace ums
