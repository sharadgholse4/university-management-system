#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include "attendance/repositories/i_attendance_repository.hpp"

namespace ums {

class SQLiteAttendanceRepository : public IAttendanceRepository {
public:
    explicit SQLiteAttendanceRepository(SQLite::Database& db);
    
    void initialize() override;
    Result<Attendance> findById(int id) override;
    Result<Attendance> create(int studentId, int courseId, const std::string& date,
                              const std::string& status, int markedBy) override;
    Result<Attendance> update(int id, const std::string& status) override;
    Result<std::vector<Attendance>> findByStudentId(int studentId) override;
    Result<std::vector<Attendance>> findByCourseId(int courseId) override;
    Result<std::vector<Attendance>> findByStudentAndCourse(int studentId, int courseId) override;
    Result<std::vector<Attendance>> findByCourseAndDate(int courseId, const std::string& date) override;
    Result<bool> existsByStudentCourseDate(int studentId, int courseId, const std::string& date) override;

private:
    SQLite::Database& db_;
    Attendance extractAttendance(SQLite::Statement& stmt);
};

} // namespace ums
