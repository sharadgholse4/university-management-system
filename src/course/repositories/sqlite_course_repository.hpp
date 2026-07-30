#pragma once
#include <vector>
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include "course/repositories/i_course_repository.hpp"

namespace ums {

class SQLiteCourseRepository : public ICourseRepository {
public:
    explicit SQLiteCourseRepository(SQLite::Database& db);

    void initialize() override;
    Result<Course> findById(int id) override;
    Result<Course> findByCode(const std::string& code) override;
    Result<Course> create(const std::string& code, const std::string& name, int departmentId,
                          int professorId, int credits, int semester) override;
    Result<Course> update(int id, const std::string& name, int professorId, int credits, int semester) override;
    Result<std::vector<Course>> findAll() override;
    Result<std::vector<Course>> findByDepartmentId(int departmentId) override;
    Result<std::vector<Course>> findByProfessorId(int professorId) override;
    Result<std::vector<Course>> findBySemester(int semester) override;
    Result<bool> existsByCode(const std::string& code) override;

private:
    SQLite::Database& db_;
};

} // namespace ums
