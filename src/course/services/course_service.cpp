#include "course/services/course_service.hpp"
#include "common/errors.hpp"

namespace ums {

CourseService::CourseService(IDepartmentRepository& deptRepo, ICourseRepository& courseRepo)
    : deptRepo_(deptRepo), courseRepo_(courseRepo) {}

Result<Department> CourseService::createDepartment(const CreateDepartmentRequest& request) {
    auto deptResult = Department::create(request.name, request.code, request.headProfessorId);
    if (!isSuccess(deptResult)) {
        return deptResult;
    }

    auto existsResult = deptRepo_.existsByCode(request.code);
    if (isSuccess(existsResult) && getValue(existsResult)) {
        return AppError{ErrorCode::Conflict, "Department with this code already exists"};
    }

    return deptRepo_.create(request.name, request.code, request.headProfessorId);
}

Result<Department> CourseService::updateDepartment(int id, const UpdateDepartmentRequest& request) {
    auto existing = deptRepo_.findById(id);
    if (!isSuccess(existing)) {
        return existing;
    }

    auto deptResult = Department::create(request.name, request.code, request.headProfessorId);
    if (!isSuccess(deptResult)) {
        return deptResult;
    }
    return deptRepo_.update(id, request.name, request.code, request.headProfessorId);
}

Result<Department> CourseService::getDepartmentById(int id) {
    return deptRepo_.findById(id);
}

Result<std::vector<Department>> CourseService::getAllDepartments() {
    return deptRepo_.findAll();
}

Result<Course> CourseService::createCourse(const CreateCourseRequest& request) {
    auto courseResult = Course::create(request.code, request.name, request.departmentId, 
                                       request.professorId, request.credits, request.semester);
    if (!isSuccess(courseResult)) {
        return courseResult;
    }

    auto existsResult = courseRepo_.existsByCode(request.code);
    if (isSuccess(existsResult) && getValue(existsResult)) {
        return AppError{ErrorCode::Conflict, "Course with this code already exists"};
    }

    // Verify department exists
    auto deptResult = deptRepo_.findById(request.departmentId);
    if (!isSuccess(deptResult)) {
        return AppError{ErrorCode::NotFound, "Department not found"};
    }

    return courseRepo_.create(request.code, request.name, request.departmentId, 
                              request.professorId, request.credits, request.semester);
}

Result<Course> CourseService::updateCourse(int id, const UpdateCourseRequest& request) {
    auto existing = courseRepo_.findById(id);
    if (!isSuccess(existing)) {
        return existing;
    }

    // Basic validation
    if (request.name.empty()) return AppError{ErrorCode::ValidationError, "Course name cannot be empty"};
    if (request.professorId <= 0) return AppError{ErrorCode::ValidationError, "Invalid professor ID"};
    if (request.credits < 1 || request.credits > 6) return AppError{ErrorCode::ValidationError, "Invalid credits"};
    if (request.semester < 1 || request.semester > 8) return AppError{ErrorCode::ValidationError, "Invalid semester"};

    return courseRepo_.update(id, request.name, request.professorId, request.credits, request.semester);
}

Result<Course> CourseService::getCourseById(int id) {
    return courseRepo_.findById(id);
}

Result<std::vector<Course>> CourseService::getAllCourses() {
    return courseRepo_.findAll();
}

Result<std::vector<Course>> CourseService::getCoursesByDepartment(int departmentId) {
    return courseRepo_.findByDepartmentId(departmentId);
}

Result<std::vector<Course>> CourseService::getCoursesByProfessor(int professorId) {
    return courseRepo_.findByProfessorId(professorId);
}

Result<std::vector<Course>> CourseService::getCoursesBySemester(int semester) {
    return courseRepo_.findBySemester(semester);
}

} // namespace ums
