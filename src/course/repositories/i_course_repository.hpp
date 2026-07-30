#pragma once
#include <string>
#include <vector>
#include "common/types.hpp"
#include "course/models/course.hpp"

namespace ums {

class ICourseRepository {
public:
    virtual ~ICourseRepository() = default;
    virtual void initialize() = 0;
    virtual Result<Course> findById(int id) = 0;
    virtual Result<Course> findByCode(const std::string& code) = 0;
    virtual Result<Course> create(const std::string& code, const std::string& name, int departmentId,
                                  int professorId, int credits, int semester) = 0;
    virtual Result<Course> update(int id, const std::string& name, int professorId, int credits, int semester) = 0;
    virtual Result<std::vector<Course>> findAll() = 0;
    virtual Result<std::vector<Course>> findByDepartmentId(int departmentId) = 0;
    virtual Result<std::vector<Course>> findByProfessorId(int professorId) = 0;
    virtual Result<std::vector<Course>> findBySemester(int semester) = 0;
    virtual Result<bool> existsByCode(const std::string& code) = 0;
};

} // namespace ums
