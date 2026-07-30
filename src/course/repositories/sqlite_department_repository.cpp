#include "course/repositories/sqlite_department_repository.hpp"
#include "common/errors.hpp"
#include <sqlite3.h>
#include <iostream>

namespace ums {

SQLiteDepartmentRepository::SQLiteDepartmentRepository(SQLite::Database& db) : db_(db) {}

void SQLiteDepartmentRepository::initialize() {
    // Migrations are handled externally
}

Result<Department> SQLiteDepartmentRepository::findById(int id) {
    try {
        SQLite::Statement query(db_, "SELECT * FROM departments WHERE id = ?");
        query.bind(1, id);

        if (query.executeStep()) {
            int headProfId = query.getColumn("head_professor_id").isNull() ? 0 : query.getColumn("head_professor_id").getInt();
            return Department::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("name").getString(),
                query.getColumn("code").getString(),
                headProfId,
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            );
        }
        return AppError{ErrorCode::NotFound, "Department not found"};
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Department> SQLiteDepartmentRepository::findByCode(const std::string& code) {
    try {
        SQLite::Statement query(db_, "SELECT * FROM departments WHERE code = ?");
        query.bind(1, code);

        if (query.executeStep()) {
            int headProfId = query.getColumn("head_professor_id").isNull() ? 0 : query.getColumn("head_professor_id").getInt();
            return Department::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("name").getString(),
                query.getColumn("code").getString(),
                headProfId,
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            );
        }
        return AppError{ErrorCode::NotFound, "Department not found"};
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Department> SQLiteDepartmentRepository::create(const std::string& name, const std::string& code, int headProfessorId) {
    try {
        SQLite::Statement query(db_, 
            "INSERT INTO departments (name, code, head_professor_id) VALUES (?, ?, ?) RETURNING *");
        query.bind(1, name);
        query.bind(2, code);
        if (headProfessorId > 0) {
            query.bind(3, headProfessorId);
        } else {
            query.bind(3); // bind null
        }

        if (query.executeStep()) {
            int headProfId = query.getColumn("head_professor_id").isNull() ? 0 : query.getColumn("head_professor_id").getInt();
            return Department::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("name").getString(),
                query.getColumn("code").getString(),
                headProfId,
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            );
        }
        return AppError{ErrorCode::InternalError, "Failed to create department"};
    } catch (const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE) {
            return AppError{ErrorCode::Conflict, "Department with this name or code already exists"};
        }
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<Department> SQLiteDepartmentRepository::update(int id, const std::string& name, const std::string& code, int headProfessorId) {
    try {
        SQLite::Statement query(db_, 
            "UPDATE departments SET name = ?, code = ?, head_professor_id = ?, updated_at = datetime('now') WHERE id = ? RETURNING *");
        query.bind(1, name);
        query.bind(2, code);
        if (headProfessorId > 0) {
            query.bind(3, headProfessorId);
        } else {
            query.bind(3);
        }
        query.bind(4, id);

        if (query.executeStep()) {
            int headProfId = query.getColumn("head_professor_id").isNull() ? 0 : query.getColumn("head_professor_id").getInt();
            return Department::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("name").getString(),
                query.getColumn("code").getString(),
                headProfId,
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            );
        }
        return AppError{ErrorCode::NotFound, "Department not found"};
    } catch (const SQLite::Exception& e) {
        if (e.getExtendedErrorCode() == SQLITE_CONSTRAINT_UNIQUE) {
            return AppError{ErrorCode::Conflict, "Department with this name or code already exists"};
        }
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<std::vector<Department>> SQLiteDepartmentRepository::findAll() {
    try {
        std::vector<Department> departments;
        SQLite::Statement query(db_, "SELECT * FROM departments");
        
        while (query.executeStep()) {
            int headProfId = query.getColumn("head_professor_id").isNull() ? 0 : query.getColumn("head_professor_id").getInt();
            departments.push_back(Department::fromDatabase(
                query.getColumn("id").getInt(),
                query.getColumn("name").getString(),
                query.getColumn("code").getString(),
                headProfId,
                query.getColumn("created_at").getString(),
                query.getColumn("updated_at").getString()
            ));
        }
        return departments;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

Result<bool> SQLiteDepartmentRepository::existsByCode(const std::string& code) {
    try {
        SQLite::Statement query(db_, "SELECT 1 FROM departments WHERE code = ?");
        query.bind(1, code);
        return query.executeStep();
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
}

} // namespace ums
