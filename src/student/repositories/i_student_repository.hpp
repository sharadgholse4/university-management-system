#pragma once
#include <vector>
#include <string>
#include "common/types.hpp"
#include "student/models/student.hpp"

namespace ums {

class IStudentRepository {
public:
    virtual ~IStudentRepository() = default;
    virtual void initialize() = 0;
    virtual Result<Student> findById(int id) = 0;
    virtual Result<Student> findByUserId(int userId) = 0;
    virtual Result<Student> findByRollNumber(const std::string& rollNumber) = 0;
    virtual Result<Student> create(int userId, const std::string& rollNumber,
                                    const std::string& firstName, const std::string& lastName,
                                    int departmentId, int semester) = 0;
    virtual Result<Student> update(int id, const std::string& firstName, const std::string& lastName,
                                    int departmentId, int semester) = 0;
    virtual Result<std::vector<Student>> findAll() = 0;
    virtual Result<std::vector<Student>> findByDepartment(int departmentId) = 0;
    virtual Result<bool> existsByUserId(int userId) = 0;
    virtual Result<bool> existsByRollNumber(const std::string& rollNumber) = 0;
};

} // namespace ums
