#include "results/repositories/sqlite_result_repository.hpp"
#include "common/errors.hpp"
#include <iostream>

namespace ums {

SQLiteResultRepository::SQLiteResultRepository(SQLite::Database& db) : db_(db) {}

void SQLiteResultRepository::initialize() {
    try {
        db_.exec(
            "CREATE TABLE IF NOT EXISTS results ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "student_id INTEGER NOT NULL, "
            "course_id INTEGER NOT NULL, "
            "exam_type TEXT NOT NULL CHECK(exam_type IN ('midterm', 'final', 'assignment', 'quiz')), "
            "marks_obtained REAL NOT NULL CHECK(marks_obtained >= 0), "
            "max_marks REAL NOT NULL CHECK(max_marks > 0 AND marks_obtained <= max_marks), "
            "grade TEXT NOT NULL, "
            "semester INTEGER NOT NULL CHECK(semester >= 1 AND semester <= 8), "
            "created_at TEXT NOT NULL DEFAULT (datetime('now')), "
            "updated_at TEXT NOT NULL DEFAULT (datetime('now')), "
            "FOREIGN KEY (student_id) REFERENCES students(id), "
            "UNIQUE(student_id, course_id, exam_type)"
            ")"
        );
        db_.exec("CREATE INDEX IF NOT EXISTS idx_results_student_id ON results(student_id)");
        db_.exec("CREATE INDEX IF NOT EXISTS idx_results_course_id ON results(course_id)");
        db_.exec("CREATE INDEX IF NOT EXISTS idx_results_student_semester ON results(student_id, semester)");
    } catch (const SQLite::Exception& e) {
        std::cerr << "Failed to initialize results table: " << e.what() << std::endl;
        throw;
    }
}

ExamResult SQLiteResultRepository::extractResult(SQLite::Statement& stmt) {
    int id = stmt.getColumn(0);
    int studentId = stmt.getColumn(1);
    int courseId = stmt.getColumn(2);
    std::string examType = stmt.getColumn(3).getString();
    double marksObtained = stmt.getColumn(4).getDouble();
    double maxMarks = stmt.getColumn(5).getDouble();
    std::string grade = stmt.getColumn(6).getString();
    int semester = stmt.getColumn(7);
    std::string createdAt = stmt.getColumn(8).getString();
    std::string updatedAt = stmt.getColumn(9).getString();

    return ExamResult::fromDatabase(id, studentId, courseId, examType, marksObtained, maxMarks, grade, semester, createdAt, updatedAt);
}

Result<ExamResult> SQLiteResultRepository::findById(int id) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, exam_type, marks_obtained, max_marks, grade, semester, created_at, updated_at FROM results WHERE id = ?");
        stmt.bind(1, id);
        
        if (stmt.executeStep()) {
            return extractResult(stmt);
        } else {
            return AppError{ErrorCode::NotFound, "Result not found"};
        }
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<ExamResult> SQLiteResultRepository::create(int studentId, int courseId, const std::string& examType,
                                                double marksObtained, double maxMarks, const std::string& grade,
                                                int semester) {
    try {
        SQLite::Statement stmt(db_, "INSERT INTO results (student_id, course_id, exam_type, marks_obtained, max_marks, grade, semester) VALUES (?, ?, ?, ?, ?, ?, ?)");
        stmt.bind(1, studentId);
        stmt.bind(2, courseId);
        stmt.bind(3, examType);
        stmt.bind(4, marksObtained);
        stmt.bind(5, maxMarks);
        stmt.bind(6, grade);
        stmt.bind(7, semester);
        
        stmt.exec();
        
        long long lastId = db_.getLastInsertRowid();
        return findById(static_cast<int>(lastId));
    } catch (const SQLite::Exception& e) {
        std::string errorMsg = e.what();
        if (errorMsg.find("UNIQUE constraint failed") != std::string::npos) {
            return AppError{ErrorCode::Conflict, "Result for this student, course, and exam type already exists"};
        }
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<ExamResult> SQLiteResultRepository::update(int id, double marksObtained, double maxMarks, const std::string& grade) {
    try {
        SQLite::Statement stmt(db_, "UPDATE results SET marks_obtained = ?, max_marks = ?, grade = ?, updated_at = datetime('now') WHERE id = ?");
        stmt.bind(1, marksObtained);
        stmt.bind(2, maxMarks);
        stmt.bind(3, grade);
        stmt.bind(4, id);
        
        if (stmt.exec() == 0) {
            return AppError{ErrorCode::NotFound, "Result not found"};
        }
        
        return findById(id);
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<ExamResult>> SQLiteResultRepository::findByStudentId(int studentId) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, exam_type, marks_obtained, max_marks, grade, semester, created_at, updated_at FROM results WHERE student_id = ?");
        stmt.bind(1, studentId);
        
        std::vector<ExamResult> results;
        while (stmt.executeStep()) {
            results.push_back(extractResult(stmt));
        }
        return results;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<ExamResult>> SQLiteResultRepository::findByStudentAndSemester(int studentId, int semester) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, exam_type, marks_obtained, max_marks, grade, semester, created_at, updated_at FROM results WHERE student_id = ? AND semester = ?");
        stmt.bind(1, studentId);
        stmt.bind(2, semester);
        
        std::vector<ExamResult> results;
        while (stmt.executeStep()) {
            results.push_back(extractResult(stmt));
        }
        return results;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<ExamResult>> SQLiteResultRepository::findByCourseId(int courseId) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, student_id, course_id, exam_type, marks_obtained, max_marks, grade, semester, created_at, updated_at FROM results WHERE course_id = ?");
        stmt.bind(1, courseId);
        
        std::vector<ExamResult> results;
        while (stmt.executeStep()) {
            results.push_back(extractResult(stmt));
        }
        return results;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteResultRepository::existsByStudentCourseExamType(int studentId, int courseId, const std::string& examType) {
    try {
        SQLite::Statement stmt(db_, "SELECT COUNT(*) FROM results WHERE student_id = ? AND course_id = ? AND exam_type = ?");
        stmt.bind(1, studentId);
        stmt.bind(2, courseId);
        stmt.bind(3, examType);
        
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
