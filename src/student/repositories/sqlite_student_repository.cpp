#include "student/repositories/sqlite_student_repository.hpp"
#include "common/errors.hpp"
#include <iostream>

namespace ums {

SQLiteStudentRepository::SQLiteStudentRepository(SQLite::Database& db) : db_(db) {}

void SQLiteStudentRepository::initialize() {
    try {
        db_.exec(
            "CREATE TABLE IF NOT EXISTS students ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "user_id INTEGER NOT NULL UNIQUE, "
            "roll_number TEXT NOT NULL UNIQUE, "
            "first_name TEXT NOT NULL, "
            "last_name TEXT NOT NULL, "
            "department_id INTEGER, "
            "semester INTEGER NOT NULL DEFAULT 1 CHECK(semester >= 1 AND semester <= 8), "
            "enrollment_date TEXT NOT NULL DEFAULT (datetime('now')), "
            "created_at TEXT NOT NULL DEFAULT (datetime('now')), "
            "updated_at TEXT NOT NULL DEFAULT (datetime('now')), "
            "FOREIGN KEY (user_id) REFERENCES users(id)"
            ")"
        );
        db_.exec("CREATE INDEX IF NOT EXISTS idx_students_user_id ON students(user_id)");
        db_.exec("CREATE INDEX IF NOT EXISTS idx_students_roll_number ON students(roll_number)");
        db_.exec("CREATE INDEX IF NOT EXISTS idx_students_department_id ON students(department_id)");
    } catch (const SQLite::Exception& e) {
        std::cerr << "Failed to initialize students table: " << e.what() << std::endl;
        throw;
    }
}

Student SQLiteStudentRepository::extractStudent(SQLite::Statement& stmt) {
    int id = stmt.getColumn(0);
    int userId = stmt.getColumn(1);
    std::string rollNumber = stmt.getColumn(2).getString();
    std::string firstName = stmt.getColumn(3).getString();
    std::string lastName = stmt.getColumn(4).getString();
    int departmentId = stmt.getColumn(5).isNull() ? 0 : stmt.getColumn(5).getInt();
    int semester = stmt.getColumn(6);
    std::string enrollmentDate = stmt.getColumn(7).getString();
    std::string createdAt = stmt.getColumn(8).getString();
    std::string updatedAt = stmt.getColumn(9).getString();

    return Student::fromDatabase(id, userId, rollNumber, firstName, lastName, departmentId, semester, enrollmentDate, createdAt, updatedAt);
}

Result<Student> SQLiteStudentRepository::findById(int id) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, user_id, roll_number, first_name, last_name, department_id, semester, enrollment_date, created_at, updated_at FROM students WHERE id = ?");
        stmt.bind(1, id);
        
        if (stmt.executeStep()) {
            return extractStudent(stmt);
        } else {
            return AppError{ErrorCode::NotFound, "Student not found"};
        }
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Student> SQLiteStudentRepository::findByUserId(int userId) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, user_id, roll_number, first_name, last_name, department_id, semester, enrollment_date, created_at, updated_at FROM students WHERE user_id = ?");
        stmt.bind(1, userId);
        
        if (stmt.executeStep()) {
            return extractStudent(stmt);
        } else {
            return AppError{ErrorCode::NotFound, "Student not found for this user"};
        }
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Student> SQLiteStudentRepository::findByRollNumber(const std::string& rollNumber) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, user_id, roll_number, first_name, last_name, department_id, semester, enrollment_date, created_at, updated_at FROM students WHERE roll_number = ?");
        stmt.bind(1, rollNumber);
        
        if (stmt.executeStep()) {
            return extractStudent(stmt);
        } else {
            return AppError{ErrorCode::NotFound, "Student not found with this roll number"};
        }
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Student> SQLiteStudentRepository::create(int userId, const std::string& rollNumber,
                                                const std::string& firstName, const std::string& lastName,
                                                int departmentId, int semester) {
    try {
        SQLite::Statement stmt(db_, "INSERT INTO students (user_id, roll_number, first_name, last_name, department_id, semester) VALUES (?, ?, ?, ?, ?, ?)");
        stmt.bind(1, userId);
        stmt.bind(2, rollNumber);
        stmt.bind(3, firstName);
        stmt.bind(4, lastName);
        if (departmentId > 0) stmt.bind(5, departmentId); else stmt.bind(5); // NULL if <= 0
        stmt.bind(6, semester);
        
        stmt.exec();
        
        long long lastId = db_.getLastInsertRowid();
        return findById(static_cast<int>(lastId));
    } catch (const SQLite::Exception& e) {
        std::string errorMsg = e.what();
        if (errorMsg.find("UNIQUE constraint failed") != std::string::npos) {
            return AppError{ErrorCode::Conflict, "Student with this user ID or roll number already exists"};
        }
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Student> SQLiteStudentRepository::update(int id, const std::string& firstName, const std::string& lastName,
                                                int departmentId, int semester) {
    try {
        SQLite::Statement stmt(db_, "UPDATE students SET first_name = ?, last_name = ?, department_id = ?, semester = ?, updated_at = datetime('now') WHERE id = ?");
        stmt.bind(1, firstName);
        stmt.bind(2, lastName);
        if (departmentId > 0) stmt.bind(3, departmentId); else stmt.bind(3);
        stmt.bind(4, semester);
        stmt.bind(5, id);
        
        if (stmt.exec() == 0) {
            return AppError{ErrorCode::NotFound, "Student not found"};
        }
        
        return findById(id);
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Student>> SQLiteStudentRepository::findAll() {
    try {
        SQLite::Statement stmt(db_, "SELECT id, user_id, roll_number, first_name, last_name, department_id, semester, enrollment_date, created_at, updated_at FROM students");
        std::vector<Student> students;
        
        while (stmt.executeStep()) {
            students.push_back(extractStudent(stmt));
        }
        
        return students;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Student>> SQLiteStudentRepository::findByDepartment(int departmentId) {
    try {
        SQLite::Statement stmt(db_, "SELECT id, user_id, roll_number, first_name, last_name, department_id, semester, enrollment_date, created_at, updated_at FROM students WHERE department_id = ?");
        stmt.bind(1, departmentId);
        
        std::vector<Student> students;
        while (stmt.executeStep()) {
            students.push_back(extractStudent(stmt));
        }
        
        return students;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteStudentRepository::existsByUserId(int userId) {
    try {
        SQLite::Statement stmt(db_, "SELECT COUNT(*) FROM students WHERE user_id = ?");
        stmt.bind(1, userId);
        
        if (stmt.executeStep()) {
            int count = stmt.getColumn(0);
            return count > 0;
        }
        return false;
    } catch (const SQLite::Exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteStudentRepository::existsByRollNumber(const std::string& rollNumber) {
    try {
        SQLite::Statement stmt(db_, "SELECT COUNT(*) FROM students WHERE roll_number = ?");
        stmt.bind(1, rollNumber);
        
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
