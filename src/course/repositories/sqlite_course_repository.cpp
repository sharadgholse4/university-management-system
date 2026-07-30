#include "course/repositories/sqlite_course_repository.hpp"
#include "common/errors.hpp"
#include <sqlite3.h>
#include <iostream>

namespace ums {

SQLiteCourseRepository::SQLiteCourseRepository(SQLite::Database& db) : db_(db) {}

void SQLiteCourseRepository::initialize() {
    // Migrations are handled externally
}

Result<Course> SQLiteCourseRepository::findById(int id) {
    try {
        SQLite::Statement query(db_, "SELECT * FROM courses WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            return Course::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("code").getString(),
                query.getColumn("name").getString(),
                query.getColumn("department_id").getInt(),
                query.getColumn("professor_id").getInt(),
                query.getColumn("credits").getInt(),
                query.getColumn("semester").getInt(),
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            );
        }
        return AppError{ErrorCode::NotFound, "Course not found"};
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Course> SQLiteCourseRepository::findByCode(const std::string& code) {
    try {
        SQLite::Statement query(db_, "SELECT * FROM courses WHERE code = ?");
        query.bind(1, code);

        if (query.executeStep()) {
            return Course::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("code").getString(),
                query.getColumn("name").getString(),
                query.getColumn("department_id").getInt(),
                query.getColumn("professor_id").getInt(),
                query.getColumn("credits").getInt(),
                query.getColumn("semester").getInt(),
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            );
        }
        return AppError{ErrorCode::NotFound, "Course not found"};
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Course> SQLiteCourseRepository::create(const std::string& code, const std::string& name, int departmentId,
                                              int professorId, int credits, int semester) {
    try {
        SQLite::Statement query(db_, 
            "INSERT INTO courses (code, name, department_id, professor_id, credits, semester) "
            "VALUES (?, ?, ?, ?, ?, ?) RETURNING *");
        query.bind(1, code);
        query.bind(2, name);
        query.bind(3, departmentId);
        query.bind(4, professorId);
        query.bind(5, credits);
        query.bind(6, semester);

        if (query.executeStep()) {
            return Course::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("code").getString(),
                query.getColumn("name").getString(),
                query.getColumn("department_id").getInt(),
                query.getColumn("professor_id").getInt(),
                query.getColumn("credits").getInt(),
                query.getColumn("semester").getInt(),
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            );
        }
        return AppError{ErrorCode::InternalError, "Failed to create course"};
    } catch (const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE) {
            return AppError{ErrorCode::Conflict, "Course with this code already exists"};
        }
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Course> SQLiteCourseRepository::update(int id, const std::string& name, int professorId, int credits, int semester) {
    try {
        SQLite::Statement query(db_, 
            "UPDATE courses SET name = ?, professor_id = ?, credits = ?, semester = ?, updated_at = datetime('now') "
            "WHERE id = ? RETURNING *");
        query.bind(1, name);
        query.bind(2, professorId);
        query.bind(3, credits);
        query.bind(4, semester);
        query.bind(5, id);

        if (query.executeStep()) {
            return Course::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("code").getString(),
                query.getColumn("name").getString(),
                query.getColumn("department_id").getInt(),
                query.getColumn("professor_id").getInt(),
                query.getColumn("credits").getInt(),
                query.getColumn("semester").getInt(),
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            );
        }
        return AppError{ErrorCode::NotFound, "Course not found"};
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Course>> SQLiteCourseRepository::findAll() {
    try {
        std::vector<Course> courses;
        SQLite::Statement query(db_, "SELECT * FROM courses");
        
        while (query.executeStep()) {
            courses.push_back(Course::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("code").getString(),
                query.getColumn("name").getString(),
                query.getColumn("department_id").getInt(),
                query.getColumn("professor_id").getInt(),
                query.getColumn("credits").getInt(),
                query.getColumn("semester").getInt(),
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            ));
        }
        return courses;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Course>> SQLiteCourseRepository::findByDepartmentId(int departmentId) {
    try {
        std::vector<Course> courses;
        SQLite::Statement query(db_, "SELECT * FROM courses WHERE department_id = ?");
        query.bind(1, departmentId);
        
        while (query.executeStep()) {
            courses.push_back(Course::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("code").getString(),
                query.getColumn("name").getString(),
                query.getColumn("department_id").getInt(),
                query.getColumn("professor_id").getInt(),
                query.getColumn("credits").getInt(),
                query.getColumn("semester").getInt(),
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            ));
        }
        return courses;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Course>> SQLiteCourseRepository::findByProfessorId(int professorId) {
    try {
        std::vector<Course> courses;
        SQLite::Statement query(db_, "SELECT * FROM courses WHERE professor_id = ?");
        query.bind(1, professorId);
        
        while (query.executeStep()) {
            courses.push_back(Course::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("code").getString(),
                query.getColumn("name").getString(),
                query.getColumn("department_id").getInt(),
                query.getColumn("professor_id").getInt(),
                query.getColumn("credits").getInt(),
                query.getColumn("semester").getInt(),
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            ));
        }
        return courses;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Course>> SQLiteCourseRepository::findBySemester(int semester) {
    try {
        std::vector<Course> courses;
        SQLite::Statement query(db_, "SELECT * FROM courses WHERE semester = ?");
        query.bind(1, semester);
        
        while (query.executeStep()) {
            courses.push_back(Course::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("code").getString(),
                query.getColumn("name").getString(),
                query.getColumn("department_id").getInt(),
                query.getColumn("professor_id").getInt(),
                query.getColumn("credits").getInt(),
                query.getColumn("semester").getInt(),
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            ));
        }
        return courses;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteCourseRepository::existsByCode(const std::string& code) {
    try {
        SQLite::Statement query(db_, "SELECT 1 FROM courses WHERE code = ?");
        query.bind(1, code);
        return query.executeStep();
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

} // namespace ums
