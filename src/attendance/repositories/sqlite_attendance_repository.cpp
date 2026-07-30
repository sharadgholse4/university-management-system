#include "attendance/repositories/sqlite_attendance_repository.hpp"
#include "common/errors.hpp"
#include <iostream>
#include <utility>

namespace ums {

SQLiteAttendanceRepository::SQLiteAttendanceRepository(SQLite::Database& db) : db_(db) {}

void SQLiteAttendanceRepository::initialize() {
    try {
        db_.exec(
            "CREATE TABLE IF NOT EXISTS attendance ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "student_id INTEGER NOT NULL, "
            "course_id INTEGER NOT NULL, "
            "date TEXT NOT NULL, "
            "status TEXT NOT NULL CHECK(status IN ('present', 'absent', 'late')), "
            "marked_by INTEGER NOT NULL, "
            "created_at TEXT NOT NULL DEFAULT (datetime('now')), "
            "updated_at TEXT NOT NULL DEFAULT (datetime('now')), "
            "FOREIGN KEY (student_id) REFERENCES students(id), "
            "FOREIGN KEY (marked_by) REFERENCES users(id), "
            "UNIQUE(student_id, course_id, date)"
            ")"
        );
        db_.exec("CREATE INDEX IF NOT EXISTS idx_attendance_student_id ON attendance(student_id)");
        db_.exec("CREATE INDEX IF NOT EXISTS idx_attendance_course_id ON attendance(course_id)");
        db_.exec("CREATE INDEX IF NOT EXISTS idx_attendance_date ON attendance(date)");
        db_.exec("CREATE INDEX IF NOT EXISTS idx_attendance_student_course ON attendance(student_id, course_id)");
    } catch (const SQLite::Exception& e) {
        std::cerr << "Failed to initialize attendance table: " << e.what() << std::endl;
        throw;
    }
}

Attendance SQLiteAttendanceRepository::extractAttendance(SQLite::Statement& stmt) {
    int id = stmt.getColumn(0);
    int studentId = stmt.getColumn(1);
    int courseId = stmt.getColumn(2);
    std::string date = stmt.getColumn(3).getString();
    std::string status = stmt.getColumn(4).getString();
    int markedBy = stmt.getColumn(5);
    std::string createdAt = stmt.getColumn(6).getString();
    std::string updatedAt = stmt.getColumn(7).getString();

    return Attendance::fromDatabase(id, studentId, courseId, std::move(date), std::move(status), markedBy, std::move(createdAt), std::move(updatedAt));
}

Result<Attendance> SQLiteAttendanceRepository::findById(int id) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, date, status, marked_by, created_at, updated_at FROM attendance WHERE id = ?");
        stmt.bind(1, id);
        
        if (stmt.executeStep()) {
            return extractAttendance(stmt);
        } else {
            return AppError{ErrorCode::NotFound, "Attendance record not found"};
        }
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Attendance> SQLiteAttendanceRepository::create(int studentId, int courseId, const std::string& date,
                                                      const std::string& status, int markedBy) {
    try {
        SQLite::Statement stmt(db_, "INSERT INTO attendance (student_id, course_id, date, status, marked_by) VALUES (?, ?, ?, ?, ?)");
        stmt.bind(1, studentId);
        stmt.bind(2, courseId);
        stmt.bind(3, date);
        stmt.bind(4, status);
        stmt.bind(5, markedBy);
        
        stmt.exec();
        
        long long lastId = db_.getLastInsertRowid();
        return findById(static_cast<int>(lastId));
    } catch (const SQLite::Exception& e) {
        std::string errorMsg = e.what();
        if (errorMsg.find("UNIQUE constraint failed") != std::string::npos) {
            return AppError{ErrorCode::Conflict, "Attendance record already exists for this student, course, and date"};
        }
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Attendance> SQLiteAttendanceRepository::update(int id, const std::string& status) {
    try {
        SQLite::Statement stmt(db_, "UPDATE attendance SET status = ?, updated_at = datetime('now') WHERE id = ?");
        stmt.bind(1, status);
        stmt.bind(2, id);
        
        if (stmt.exec() == 0) {
            return AppError{ErrorCode::NotFound, "Attendance record not found"};
        }
        
        return findById(id);
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Attendance>> SQLiteAttendanceRepository::findByStudentId(int studentId) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, date, status, marked_by, created_at, updated_at FROM attendance WHERE student_id = ?");
        stmt.bind(1, studentId);
        
        std::vector<Attendance> records;
        while (stmt.executeStep()) {
            records.push_back(extractAttendance(stmt));
        }
        
        return records;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Attendance>> SQLiteAttendanceRepository::findByCourseId(int courseId) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, date, status, marked_by, created_at, updated_at FROM attendance WHERE course_id = ?");
        stmt.bind(1, courseId);
        
        std::vector<Attendance> records;
        while (stmt.executeStep()) {
            records.push_back(extractAttendance(stmt));
        }
        
        return records;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Attendance>> SQLiteAttendanceRepository::findByStudentAndCourse(int studentId, int courseId) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, date, status, marked_by, created_at, updated_at FROM attendance WHERE student_id = ? AND course_id = ?");
        stmt.bind(1, studentId);
        stmt.bind(2, courseId);
        
        std::vector<Attendance> records;
        while (stmt.executeStep()) {
            records.push_back(extractAttendance(stmt));
        }
        
        return records;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Attendance>> SQLiteAttendanceRepository::findByCourseAndDate(int courseId, const std::string& date) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, date, status, marked_by, created_at, updated_at FROM attendance WHERE course_id = ? AND date = ?");
        stmt.bind(1, courseId);
        stmt.bind(2, date);
        
        std::vector<Attendance> records;
        while (stmt.executeStep()) {
            records.push_back(extractAttendance(stmt));
        }
        
        return records;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteAttendanceRepository::existsByStudentCourseDate(int studentId, int courseId, const std::string& date) {
    try {
        SQLite::Statement stmt(db_, "SELECT COUNT(*) FROM attendance WHERE student_id = ? AND course_id = ? AND date = ?");
        stmt.bind(1, studentId);
        stmt.bind(2, courseId);
        stmt.bind(3, date);
        
        if (stmt.executeStep()) {
            int count = stmt.getColumn(0);
            return count > 0;
        }
        return false;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

} // namespace ums
