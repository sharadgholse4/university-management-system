#pragma once
#include <string>
#include <vector>
#include <optional>
#include "common/types.hpp"
#include "student/models/student.hpp"
#include "student/repositories/i_student_repository.hpp"

namespace ums {

struct CreateStudentRequest {
    int userId;
    std::string rollNumber;
    std::string firstName;
    std::string lastName;
    int departmentId;
    int semester;
};

struct UpdateStudentRequest {
    std::string firstName;
    std::string lastName;
    int departmentId;
    int semester;
};

class StudentService {
public:
    StudentService(IStudentRepository& studentRepo);
    Result<Student> createStudent(const CreateStudentRequest& request);
    Result<Student> getStudentById(int id);
    Result<Student> getStudentByUserId(int userId);
    Result<Student> updateStudent(int id, const UpdateStudentRequest& request);
    Result<std::vector<Student>> getAllStudents();
    Result<std::vector<Student>> getStudentsByDepartment(int departmentId);
private:
    IStudentRepository& studentRepo_;
    std::optional<AppError> validateCreateRequest(const CreateStudentRequest& request);
    std::optional<AppError> validateUpdateRequest(const UpdateStudentRequest& request);
};

} // namespace ums
