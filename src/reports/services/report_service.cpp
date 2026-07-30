#include "reports/services/report_service.hpp"
#include "common/errors.hpp"
#include <cmath>

namespace ums {

ReportService::ReportService(IDepartmentRepository& deptRepo,
                             ICourseRepository& courseRepo,
                             IStudentRepository& studentRepo,
                             IAttendanceRepository& attendanceRepo,
                             IResultRepository& resultRepo,
                             IEnrollmentRepository& enrollmentRepo,
                             SQLite::Database& db)
    : deptRepo_(deptRepo),
      courseRepo_(courseRepo),
      studentRepo_(studentRepo),
      attendanceRepo_(attendanceRepo),
      resultRepo_(resultRepo),
      enrollmentRepo_(enrollmentRepo),
      db_(db) {}

Result<DepartmentSummary> ReportService::getDepartmentSummary(int departmentId) {
    // WHY: First verify that the department exists before doing expensive operations
    auto deptResult = deptRepo_.findById(departmentId);
    if (!isSuccess(deptResult)) {
        return getError(deptResult);
    }
    const auto& dept = getValue(deptResult);

    // WHY: Use the repository abstraction to find courses
    auto coursesResult = courseRepo_.findByDepartmentId(departmentId);
    if (!isSuccess(coursesResult)) {
        return getError(coursesResult);
    }
    int totalCourses = getValue(coursesResult).size();

    // WHY: As requested, perform a direct SQL query to count students,
    // avoiding the overhead of loading full student entities into memory
    int totalStudents = 0;
    try {
        SQLite::Statement query(db_, "SELECT COUNT(*) FROM students WHERE department_id = ?");
        query.bind(1, departmentId);
        if (query.executeStep()) {
            totalStudents = query.getColumn(0).getInt();
        }
    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }

    return DepartmentSummary{
        departmentId,
        dept.name(),
        dept.code(),
        totalCourses,
        totalStudents
    };
}

Result<std::vector<StudentAttendanceSummary>> ReportService::getCourseAttendanceReport(int courseId) {
    // WHY: Validate course exists first
    auto courseResult = courseRepo_.findById(courseId);
    if (!isSuccess(courseResult)) {
        return getError(courseResult);
    }

    // WHY: Query attendance records for the course
    auto attendanceResult = attendanceRepo_.findByCourseId(courseId);
    if (!isSuccess(attendanceResult)) {
        return getError(attendanceResult);
    }
    const auto& records = getValue(attendanceResult);

    // WHY: Calculate counts in a single pass to be efficient O(N)
    std::map<int, StudentAttendanceSummary> summaries;
    for (const auto& record : records) {
        auto& summary = summaries[record.studentId()];
        summary.studentId = record.studentId();
        summary.totalClasses++;
        
        const auto& status = record.status();
        if (status == "present") summary.presentClasses++;
        else if (status == "absent") summary.absentClasses++;
        else if (status == "late") summary.lateClasses++;

        summary.attendedClasses = summary.presentClasses;
    }

    std::vector<StudentAttendanceSummary> result;
    result.reserve(summaries.size());
    for (auto& [studentId, summary] : summaries) {
        if (summary.totalClasses > 0) {
            // WHY: As per requirements, (present + late) / total * 100.0
            summary.attendancePercentage = static_cast<double>(summary.presentClasses + summary.lateClasses) / summary.totalClasses * 100.0;
        } else {
            summary.attendancePercentage = 0.0;
        }
        result.push_back(summary);
    }

    return result;
}

Result<GradeDistributionSummary> ReportService::getGradeDistributionReport(int courseId) {
    // WHY: Validate course existence
    auto courseResult = courseRepo_.findById(courseId);
    if (!isSuccess(courseResult)) {
        return getError(courseResult);
    }

    // WHY: Fetch all results for the course to calculate the distribution
    auto resultsResult = resultRepo_.findByCourseId(courseId);
    if (!isSuccess(resultsResult)) {
        return getError(resultsResult);
    }
    const auto& results = getValue(resultsResult);

    GradeDistributionSummary summary;
    summary.courseId = courseId;
    summary.totalExams = results.size();
    summary.averagePercentage = 0.0;
    
    // WHY: Initialize counts so they are always present in the output json
    std::vector<std::string> grades = {"A+", "A", "B+", "B", "C+", "C", "D", "F"};
    for (const auto& g : grades) {
        summary.gradeCounts[g] = 0;
    }

    if (summary.totalExams == 0) {
        return summary;
    }

    double totalPercentage = 0;
    for (const auto& res : results) {
        // WHY: The Result model already contains the percentage logic, we just sum it
        totalPercentage += res.percentage();
        
        // WHY: The Result model already contains the calculated grade
        summary.gradeCounts[res.grade()]++;
    }
    
    summary.averagePercentage = totalPercentage / summary.totalExams;

    return summary;
}

Result<SystemOverview> ReportService::getSystemOverview() {
    SystemOverview overview{0, 0, 0, 0, 0};
    // WHY: These counts are very fast via SQL COUNT(*). We avoid loading whole tables into memory.
    try {
        SQLite::Statement usersQuery(db_, "SELECT COUNT(*) FROM users");
        if (usersQuery.executeStep()) overview.totalUsers = usersQuery.getColumn(0).getInt();

        SQLite::Statement studentsQuery(db_, "SELECT COUNT(*) FROM students");
        if (studentsQuery.executeStep()) overview.totalStudents = studentsQuery.getColumn(0).getInt();

        SQLite::Statement coursesQuery(db_, "SELECT COUNT(*) FROM courses");
        if (coursesQuery.executeStep()) overview.totalCourses = coursesQuery.getColumn(0).getInt();

        SQLite::Statement deptsQuery(db_, "SELECT COUNT(*) FROM departments");
        if (deptsQuery.executeStep()) overview.totalDepartments = deptsQuery.getColumn(0).getInt();

        SQLite::Statement enrollQuery(db_, "SELECT COUNT(*) FROM enrollments");
        if (enrollQuery.executeStep()) overview.totalEnrollments = enrollQuery.getColumn(0).getInt();

    } catch (const std::exception& e) {
        return AppError{ErrorCode::InternalError, std::string("Database error: ") + e.what()};
    }
    return overview;
}

} // namespace ums
