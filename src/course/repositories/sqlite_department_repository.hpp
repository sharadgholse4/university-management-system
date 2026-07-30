#pragma once
#include <vector>
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include "course/repositories/i_department_repository.hpp"

namespace ums {

class SQLiteDepartmentRepository : public IDepartmentRepository {
public:
    explicit SQLiteDepartmentRepository(SQLite::Database& db);

    void initialize() override;
    Result<Department> findById(int id) override;
    Result<Department> findByCode(const std::string& code) override;
    Result<Department> create(const std::string& name, const std::string& code, int headProfessorId) override;
    Result<Department> update(int id, const std::string& name, const std::string& code, int headProfessorId) override;
    Result<std::vector<Department>> findAll() override;
    Result<bool> existsByCode(const std::string& code) override;

private:
    SQLite::Database& db_;
};

} // namespace ums
