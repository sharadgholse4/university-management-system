#pragma once
#include <string>
#include <vector>
#include "common/types.hpp"
#include "course/models/department.hpp"
#include "course/models/course.hpp"
#include "course/repositories/i_department_repository.hpp"
#include "course/repositories/i_course_repository.hpp"

namespace ums {

struct CreateDepartmentRequest { std::string name; std::string code; int headProfessorId; };
struct UpdateDepartmentRequest { std::string name; std::string code; int headProfessorId; };
struct CreateCourseRequest { std::string code; std::string name; int departmentId; int professorId; int credits; int semester; };
struct UpdateCourseRequest { std::string name; int professorId; int credits; int semester; };

// WHY: CourseService coordinates business logic between departments and courses.
// It ensures that validations are run before attempting DB operations.
class CourseService {
public:
    CourseService(IDepartmentRepository& deptRepo, ICourseRepository& courseRepo);
    
    // Department operations
    Result<Department> createDepartment(const CreateDepartmentRequest& request);
    Result<Department> updateDepartment(int id, const UpdateDepartmentRequest& request);
    Result<Department> getDepartmentById(int id);
    Result<std::vector<Department>> getAllDepartments();

    // Course operations
    Result<Course> createCourse(const CreateCourseRequest& request);
    Result<Course> updateCourse(int id, const UpdateCourseRequest& request);
    Result<Course> getCourseById(int id);
    Result<std::vector<Course>> getAllCourses();
    Result<std::vector<Course>> getCoursesByDepartment(int departmentId);
    Result<std::vector<Course>> getCoursesByProfessor(int professorId);
    Result<std::vector<Course>> getCoursesBySemester(int semester);

private:
    IDepartmentRepository& deptRepo_;
    ICourseRepository& courseRepo_;
};

} // namespace ums
