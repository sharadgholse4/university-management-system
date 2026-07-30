#include "enrollment/repositories/sqlite_enrollment_repository.hpp"
#include "common/errors.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>
#include <stdexcept>

namespace ums {

SQLiteEnrollmentRepository::SQLiteEnrollmentRepository(SQLite::Database& db) : db_(db) {}

void SQLiteEnrollmentRepository::initialize() {
    db_.exec(
        "CREATE TABLE IF NOT EXISTS enrollments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "student_id INTEGER NOT NULL, "
        "course_id INTEGER NOT NULL, "
        "enrolled_at TEXT NOT NULL DEFAULT (datetime('now')), "
        "UNIQUE(student_id, course_id), "
        "FOREIGN KEY (student_id) REFERENCES students(id), "
        "FOREIGN KEY (course_id) REFERENCES courses(id)"
        ");"
    );
    db_.exec("CREATE INDEX IF NOT EXISTS idx_enrollments_student ON enrollments(student_id);");
    db_.exec("CREATE INDEX IF NOT EXISTS idx_enrollments_course ON enrollments(course_id);");
}

Result<Enrollment> SQLiteEnrollmentRepository::findById(int id) {
    try {
        SQLite::Statement query(db_, "SELECT id, student_id, course_id, enrolled_at FROM enrollments WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            return Enrollment::fromDatabase(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString()
            );
        }
        return AppError{ErrorCode::NotFound, "Enrollment not found"};
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Enrollment> SQLiteEnrollmentRepository::create(int studentId, int courseId) {
    try {
        // RETURNING clause allows fetching auto-generated ID and default enrolled_at
        SQLite::Statement query(db_, "INSERT INTO enrollments (student_id, course_id) VALUES (?, ?) RETURNING id, enrolled_at");
        query.bind(1, studentId);
        query.bind(2, courseId);

        if (query.executeStep()) {
            return Enrollment::fromDatabase(
                query.getColumn(0).getInt(),
                studentId,
                courseId,
                query.getColumn(1).getString()
            );
        }
        return AppError{ErrorCode::InternalError, "Failed to create enrollment"};
    } catch (const SQLite::Exception& e) {
        // WHY: Handle SQLite UNIQUE constraint violation specifically to map it to a Conflict error.
        if (e.getErrorCode() == SQLITE_CONSTRAINT) {
            return AppError{ErrorCode::Conflict, "Student is already enrolled in this course"};
        }
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteEnrollmentRepository::deleteById(int id) {
    try {
        SQLite::Statement query(db_, "DELETE FROM enrollments WHERE id = ?");
        query.bind(1, id);
        int rows = query.exec();
        return rows > 0;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteEnrollmentRepository::deleteByStudentAndCourse(int studentId, int courseId) {
    try {
        SQLite::Statement query(db_, "DELETE FROM enrollments WHERE student_id = ? AND course_id = ?");
        query.bind(1, studentId);
        query.bind(2, courseId);
        int rows = query.exec();
        return rows > 0;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Enrollment>> SQLiteEnrollmentRepository::findByStudentId(int studentId) {
    try {
        std::vector<Enrollment> enrollments;
        SQLite::Statement query(db_, "SELECT id, student_id, course_id, enrolled_at FROM enrollments WHERE student_id = ?");
        query.bind(1, studentId);

        while (query.executeStep()) {
            enrollments.push_back(Enrollment::fromDatabase(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString()
            ));
        }
        return enrollments;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Enrollment>> SQLiteEnrollmentRepository::findByCourseId(int courseId) {
    try {
        std::vector<Enrollment> enrollments;
        SQLite::Statement query(db_, "SELECT id, student_id, course_id, enrolled_at FROM enrollments WHERE course_id = ?");
        query.bind(1, courseId);

        while (query.executeStep()) {
            enrollments.push_back(Enrollment::fromDatabase(
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString()
            ));
        }
        return enrollments;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteEnrollmentRepository::existsByStudentAndCourse(int studentId, int courseId) {
    try {
        SQLite::Statement query(db_, "SELECT 1 FROM enrollments WHERE student_id = ? AND course_id = ? LIMIT 1");
        query.bind(1, studentId);
        query.bind(2, courseId);
        
        return query.executeStep();
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

} // namespace ums
