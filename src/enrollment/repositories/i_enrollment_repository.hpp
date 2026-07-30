#pragma once
#include <vector>
#include "common/types.hpp"
#include "enrollment/models/enrollment.hpp"

namespace ums {

// WHY: Interface allows dependency injection, enabling mock repositories for testing
// the service layer in isolation.
class IEnrollmentRepository {
public:
    virtual ~IEnrollmentRepository() = default;
    virtual void initialize() = 0;
    virtual Result<Enrollment> findById(int id) = 0;
    virtual Result<Enrollment> create(int studentId, int courseId) = 0;
    virtual Result<bool> deleteById(int id) = 0;
    virtual Result<bool> deleteByStudentAndCourse(int studentId, int courseId) = 0;
    virtual Result<std::vector<Enrollment>> findByStudentId(int studentId) = 0;
    virtual Result<std::vector<Enrollment>> findByCourseId(int courseId) = 0;
    virtual Result<bool> existsByStudentAndCourse(int studentId, int courseId) = 0;
};

} // namespace ums
