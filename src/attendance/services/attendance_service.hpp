#pragma once
#include <string>
#include <vector>
#include <optional>
#include "common/types.hpp"
#include "attendance/models/attendance.hpp"
#include "attendance/repositories/i_attendance_repository.hpp"

namespace ums {

struct MarkAttendanceRequest {
    int studentId;
    int courseId;
    std::string date;
    std::string status;
    int markedBy;
};

struct UpdateAttendanceRequest {
    std::string status;
};

class AttendanceService {
public:
    AttendanceService(IAttendanceRepository& attendanceRepo);
    Result<Attendance> markAttendance(const MarkAttendanceRequest& request);
    Result<Attendance> updateAttendance(int id, const UpdateAttendanceRequest& request);
    Result<Attendance> getAttendanceById(int id);
    Result<std::vector<Attendance>> getAttendanceByStudent(int studentId);
    Result<std::vector<Attendance>> getAttendanceByCourse(int courseId);
    Result<std::vector<Attendance>> getAttendanceByStudentAndCourse(int studentId, int courseId);
    Result<std::vector<Attendance>> getAttendanceByCourseAndDate(int courseId, const std::string& date);
private:
    IAttendanceRepository& attendanceRepo_;
    std::optional<AppError> validateMarkRequest(const MarkAttendanceRequest& request);
    std::optional<AppError> validateUpdateRequest(const UpdateAttendanceRequest& request);
};

} // namespace ums
