#pragma once
#include <string>
#include <vector>
#include "common/types.hpp"
#include "course/models/department.hpp"

namespace ums {

class IDepartmentRepository {
public:
    virtual ~IDepartmentRepository() = default;
    virtual void initialize() = 0;
    virtual Result<Department> findById(int id) = 0;
    virtual Result<Department> findByCode(const std::string& code) = 0;
    virtual Result<Department> create(const std::string& name, const std::string& code, int headProfessorId) = 0;
    virtual Result<Department> update(int id, const std::string& name, const std::string& code, int headProfessorId) = 0;
    virtual Result<std::vector<Department>> findAll() = 0;
    virtual Result<bool> existsByCode(const std::string& code) = 0;
};

} // namespace ums
