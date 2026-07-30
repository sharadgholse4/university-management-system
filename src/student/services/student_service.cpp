#include "student/services/student_service.hpp"
#include "common/errors.hpp"

namespace ums {

StudentService::StudentService(IStudentRepository& studentRepo) : studentRepo_(studentRepo) {}

std::optional<AppError> StudentService::validateCreateRequest(const CreateStudentRequest& request) {
    auto studentCheck = Student::create(request.userId, request.rollNumber, request.firstName, request.lastName, request.departmentId, request.semester);
    if (!isSuccess(studentCheck)) {
        return getError(studentCheck);
    }
    return std::nullopt;
}

std::optional<AppError> StudentService::validateUpdateRequest(const UpdateStudentRequest& request) {
    if (request.firstName.empty() || request.firstName.length() > 100) {
        return AppError{ErrorCode::ValidationError, "First name must be 1-100 characters"};
    }
    if (request.lastName.empty() || request.lastName.length() > 100) {
        return AppError{ErrorCode::ValidationError, "Last name must be 1-100 characters"};
    }
    if (request.semester < 1 || request.semester > 8) {
        return AppError{ErrorCode::ValidationError, "Semester must be between 1 and 8"};
    }
    return std::nullopt;
}

Result<Student> StudentService::createStudent(const CreateStudentRequest& request) {
    if (auto err = validateCreateRequest(request)) {
        return *err;
    }

    auto userExists = studentRepo_.existsByUserId(request.userId);
    if (!isSuccess(userExists)) return getError(userExists);
    if (getValue(userExists)) {
        return AppError{ErrorCode::Conflict, "User already has a student profile"};
    }

    auto rollExists = studentRepo_.existsByRollNumber(request.rollNumber);
    if (!isSuccess(rollExists)) return getError(rollExists);
    if (getValue(rollExists)) {
        return AppError{ErrorCode::Conflict, "Student with this roll number already exists"};
    }

    return studentRepo_.create(request.userId, request.rollNumber, request.firstName, request.lastName, request.departmentId, request.semester);
}

Result<Student> StudentService::getStudentById(int id) {
    return studentRepo_.findById(id);
}

Result<Student> StudentService::getStudentByUserId(int userId) {
    return studentRepo_.findByUserId(userId);
}

Result<Student> StudentService::updateStudent(int id, const UpdateStudentRequest& request) {
    if (auto err = validateUpdateRequest(request)) {
        return *err;
    }

    auto existing = studentRepo_.findById(id);
    if (!isSuccess(existing)) {
        return getError(existing);
    }

    return studentRepo_.update(id, request.firstName, request.lastName, request.departmentId, request.semester);
}

Result<std::vector<Student>> StudentService::getAllStudents() {
    return studentRepo_.findAll();
}

Result<std::vector<Student>> StudentService::getStudentsByDepartment(int departmentId) {
    return studentRepo_.findByDepartment(departmentId);
}

} // namespace ums
