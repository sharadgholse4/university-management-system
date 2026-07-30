#include "enrollment/services/enrollment_service.hpp"

namespace ums {

EnrollmentService::EnrollmentService(IEnrollmentRepository& enrollmentRepo, 
                                     IStudentRepository& studentRepo, 
                                     ICourseRepository& courseRepo)
    : enrollmentRepo_(enrollmentRepo), studentRepo_(studentRepo), courseRepo_(courseRepo) {}

Result<Enrollment> EnrollmentService::enrollStudent(const EnrollStudentRequest& request) {
    auto validationResult = Enrollment::create(request.studentId, request.courseId);
    if (!isSuccess(validationResult)) {
        return validationResult;
    }

    // WHY: Before enrolling, we must guarantee both entities exist.
    // This maintains data integrity beyond just DB foreign key constraints.
    auto studentResult = studentRepo_.findById(request.studentId);
    if (!isSuccess(studentResult)) {
        return AppError{ErrorCode::NotFound, "Student not found"};
    }

    auto courseResult = courseRepo_.findById(request.courseId);
    if (!isSuccess(courseResult)) {
        return AppError{ErrorCode::NotFound, "Course not found"};
    }

    // WHY: Check for existing enrollment to provide a specific, friendly error message,
    // even though the DB UNIQUE constraint will also prevent it.
    auto existsResult = enrollmentRepo_.existsByStudentAndCourse(request.studentId, request.courseId);
    if (!isSuccess(existsResult)) {
        return getError(existsResult);
    }
    if (getValue(existsResult)) {
        return AppError{ErrorCode::Conflict, "Student is already enrolled in this course"};
    }

    return enrollmentRepo_.create(request.studentId, request.courseId);
}

Result<bool> EnrollmentService::dropEnrollment(int studentId, int courseId) {
    // WHY: Validate the enrollment exists before attempting to delete it.
    auto existsResult = enrollmentRepo_.existsByStudentAndCourse(studentId, courseId);
    if (!isSuccess(existsResult)) {
        return getError(existsResult);
    }
    if (!getValue(existsResult)) {
        return AppError{ErrorCode::NotFound, "Enrollment not found"};
    }

    return enrollmentRepo_.deleteByStudentAndCourse(studentId, courseId);
}

Result<Enrollment> EnrollmentService::getEnrollmentById(int id) {
    return enrollmentRepo_.findById(id);
}

Result<std::vector<Enrollment>> EnrollmentService::getEnrollmentsByStudent(int studentId) {
    return enrollmentRepo_.findByStudentId(studentId);
}

Result<std::vector<Enrollment>> EnrollmentService::getEnrollmentsByCourse(int courseId) {
    return enrollmentRepo_.findByCourseId(courseId);
}

} // namespace ums
