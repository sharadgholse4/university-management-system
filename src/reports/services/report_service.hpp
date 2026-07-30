#pragma once
#include <vector>
#include <string>
#include <map>
#include <utility>
#include "crow.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include "common/types.hpp"
#include "database/database_manager.hpp"
#include "student/repositories/i_student_repository.hpp"
#include "course/repositories/i_course_repository.hpp"
#include "course/repositories/i_department_repository.hpp"
#include "attendance/repositories/i_attendance_repository.hpp"
#include "results/repositories/i_result_repository.hpp"
#include "enrollment/repositories/i_enrollment_repository.hpp"

namespace ums {

struct DepartmentSummary {
    int departmentId;
    std::string departmentName;
    std::string departmentCode;
    int totalCourses;
    int totalStudents;

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["departmentId"] = departmentId;
        json["departmentName"] = departmentName;
        json["departmentCode"] = departmentCode;
        json["totalCourses"] = totalCourses;
        json["totalStudents"] = totalStudents;
        return json;
    }
};

struct StudentAttendanceSummary {
    int studentId;
    int totalClasses;
    int presentClasses;
    int attendedClasses; // Alias for presentClasses
    int absentClasses;
    int lateClasses;
    double attendancePercentage;

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["studentId"] = studentId;
        json["totalClasses"] = totalClasses;
        json["presentClasses"] = presentClasses;
        json["attendedClasses"] = attendedClasses;
        json["absentClasses"] = absentClasses;
        json["lateClasses"] = lateClasses;
        json["attendancePercentage"] = attendancePercentage;
        return json;
    }
};

struct GradeDistributionSummary {
    int courseId;
    int totalExams;
    double averagePercentage;
    std::map<std::string, int> gradeCounts; // "A+": 5, "A": 10, etc.

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["courseId"] = courseId;
        json["totalExams"] = totalExams;
        json["averagePercentage"] = averagePercentage;
        crow::json::wvalue grades;
        for (const auto& [grade, count] : gradeCounts) {
            grades[grade] = count;
        }
        json["gradeCounts"] = std::move(grades);
        return json;
    }
};

struct SystemOverview {
    int totalUsers;
    int totalStudents;
    int totalCourses;
    int totalDepartments;
    int totalEnrollments;

    crow::json::wvalue toJson() const {
        crow::json::wvalue json;
        json["totalUsers"] = totalUsers;
        json["totalStudents"] = totalStudents;
        json["totalCourses"] = totalCourses;
        json["totalDepartments"] = totalDepartments;
        json["totalEnrollments"] = totalEnrollments;
        return json;
    }
};

class ReportService {
public:
    ReportService(IDepartmentRepository& deptRepo,
                  ICourseRepository& courseRepo,
                  IStudentRepository& studentRepo,
                  IAttendanceRepository& attendanceRepo,
                  IResultRepository& resultRepo,
                  IEnrollmentRepository& enrollmentRepo,
                  SQLite::Database& db);

    Result<DepartmentSummary> getDepartmentSummary(int departmentId);
    Result<std::vector<StudentAttendanceSummary>> getCourseAttendanceReport(int courseId);
    Result<GradeDistributionSummary> getGradeDistributionReport(int courseId);
    Result<SystemOverview> getSystemOverview();

private:
    IDepartmentRepository& deptRepo_;
    ICourseRepository& courseRepo_;
    IStudentRepository& studentRepo_;
    IAttendanceRepository& attendanceRepo_;
    IResultRepository& resultRepo_;
    IEnrollmentRepository& enrollmentRepo_;
    SQLite::Database& db_;
};

} // namespace ums
