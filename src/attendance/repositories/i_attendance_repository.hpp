#pragma once
#include <vector>
#include <string>
#include "common/types.hpp"
#include "attendance/models/attendance.hpp"

namespace ums {

class IAttendanceRepository {
public:
    virtual ~IAttendanceRepository() = default;
    virtual void initialize() = 0;
    virtual Result<Attendance> findById(int id) = 0;
    virtual Result<Attendance> create(int studentId, int courseId, const std::string& date,
                                      const std::string& status, int markedBy) = 0;
    virtual Result<Attendance> update(int id, const std::string& status) = 0;
    virtual Result<std::vector<Attendance>> findByStudentId(int studentId) = 0;
    virtual Result<std::vector<Attendance>> findByCourseId(int courseId) = 0;
    virtual Result<std::vector<Attendance>> findByStudentAndCourse(int studentId, int courseId) = 0;
    virtual Result<std::vector<Attendance>> findByCourseAndDate(int courseId, const std::string& date) = 0;
    virtual Result<bool> existsByStudentCourseDate(int studentId, int courseId, const std::string& date) = 0;
};

} // namespace ums
