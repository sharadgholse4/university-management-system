#pragma once
#include "student/repositories/i_student_repository.hpp"
#include <SQLiteCpp/SQLiteCpp.h>

namespace ums {

class SQLiteStudentRepository : public IStudentRepository {
public:
    explicit SQLiteStudentRepository(SQLite::Database& db);
    
    void initialize() override;
    
    Result<Student> findById(int id) override;
    Result<Student> findByUserId(int userId) override;
    Result<Student> findByRollNumber(const std::string& rollNumber) override;
    Result<Student> create(int userId, const std::string& rollNumber,
                           const std::string& firstName, const std::string& lastName,
                           int departmentId, int semester) override;
    Result<Student> update(int id, const std::string& firstName, const std::string& lastName,
                           int departmentId, int semester) override;
    Result<std::vector<Student>> findAll() override;
    Result<std::vector<Student>> findByDepartment(int departmentId) override;
    Result<bool> existsByUserId(int userId) override;
    Result<bool> existsByRollNumber(const std::string& rollNumber) override;

private:
    SQLite::Database& db_;
    
    Student extractStudent(SQLite::Statement& stmt);
};

} // namespace ums
