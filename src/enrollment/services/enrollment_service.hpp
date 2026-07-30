#pragma once
#include <vector>
#include "common/types.hpp"
#include "enrollment/models/enrollment.hpp"
#include "enrollment/repositories/i_enrollment_repository.hpp"
#include "student/repositories/i_student_repository.hpp"
#include "course/repositories/i_course_repository.hpp"

namespace ums {

// WHY DTOs: DTOs decouple external input formats from our internal models.
struct EnrollStudentRequest {
    int studentId;
    int courseId;
};

// WHY: The Service layer coordinates multiple repositories to enforce business rules,
// keeping the controllers thin and focused on HTTP.
class EnrollmentService {
public:
    EnrollmentService(IEnrollmentRepository& enrollmentRepo, 
                      IStudentRepository& studentRepo, 
                      ICourseRepository& courseRepo);

    Result<Enrollment> enrollStudent(const EnrollStudentRequest& request);
    Result<bool> dropEnrollment(int studentId, int courseId);
    Result<Enrollment> getEnrollmentById(int id);
    Result<std::vector<Enrollment>> getEnrollmentsByStudent(int studentId);
    Result<std::vector<Enrollment>> getEnrollmentsByCourse(int courseId);

private:
    IEnrollmentRepository& enrollmentRepo_;
    IStudentRepository& studentRepo_;
    ICourseRepository& courseRepo_;
};

} // namespace ums
