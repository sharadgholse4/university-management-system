#include "attendance/services/attendance_service.hpp"
#include "common/errors.hpp"

namespace ums {

AttendanceService::AttendanceService(IAttendanceRepository& attendanceRepo) : attendanceRepo_(attendanceRepo) {}

std::optional<AppError> AttendanceService::validateMarkRequest(const MarkAttendanceRequest& request) {
    auto modelResult = Attendance::create(request.studentId, request.courseId, request.date, request.status, request.markedBy);
    if (!isSuccess(modelResult)) {
        return getError(modelResult);
    }
    return std::nullopt;
}

std::optional<AppError> AttendanceService::validateUpdateRequest(const UpdateAttendanceRequest& request) {
    auto statusResult = stringToAttendanceStatus(request.status);
    if (!isSuccess(statusResult)) {
        return getError(statusResult);
    }
    return std::nullopt;
}

Result<Attendance> AttendanceService::markAttendance(const MarkAttendanceRequest& request) {
    if (auto err = validateMarkRequest(request)) {
        return *err;
    }

    auto existsResult = attendanceRepo_.existsByStudentCourseDate(request.studentId, request.courseId, request.date);
    if (!isSuccess(existsResult)) return getError(existsResult);
    
    if (getValue(existsResult)) {
        return AppError{ErrorCode::Conflict, "Attendance already marked for this student, course, and date"};
    }

    return attendanceRepo_.create(request.studentId, request.courseId, request.date, request.status, request.markedBy);
}

Result<Attendance> AttendanceService::updateAttendance(int id, const UpdateAttendanceRequest& request) {
    if (auto err = validateUpdateRequest(request)) {
        return *err;
    }

    auto existsResult = attendanceRepo_.findById(id);
    if (!isSuccess(existsResult)) return getError(existsResult); // returns NotFound if doesn't exist

    return attendanceRepo_.update(id, request.status);
}

Result<Attendance> AttendanceService::getAttendanceById(int id) {
    return attendanceRepo_.findById(id);
}

Result<std::vector<Attendance>> AttendanceService::getAttendanceByStudent(int studentId) {
    return attendanceRepo_.findByStudentId(studentId);
}

Result<std::vector<Attendance>> AttendanceService::getAttendanceByCourse(int courseId) {
    return attendanceRepo_.findByCourseId(courseId);
}

Result<std::vector<Attendance>> AttendanceService::getAttendanceByStudentAndCourse(int studentId, int courseId) {
    return attendanceRepo_.findByStudentAndCourse(studentId, courseId);
}

Result<std::vector<Attendance>> AttendanceService::getAttendanceByCourseAndDate(int courseId, const std::string& date) {
    return attendanceRepo_.findByCourseAndDate(courseId, date);
}

} // namespace ums
