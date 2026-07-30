#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "reports/services/report_service.hpp"
#include "course/repositories/sqlite_department_repository.hpp"
#include "course/repositories/sqlite_course_repository.hpp"
#include "student/repositories/sqlite_student_repository.hpp"
#include "attendance/repositories/sqlite_attendance_repository.hpp"
#include "results/repositories/sqlite_result_repository.hpp"
#include "enrollment/repositories/sqlite_enrollment_repository.hpp"
#include "test_helpers/test_db.hpp"

using namespace ums;
using namespace ums::test;

class ReportServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // TestDatabase creates an in-memory DB.
        deptRepo_ = std::make_unique<SQLiteDepartmentRepository>(testDb_.db());
        deptRepo_->initialize();

        courseRepo_ = std::make_unique<SQLiteCourseRepository>(testDb_.db());
        courseRepo_->initialize();

        studentRepo_ = std::make_unique<SQLiteStudentRepository>(testDb_.db());
        studentRepo_->initialize();

        attendanceRepo_ = std::make_unique<SQLiteAttendanceRepository>(testDb_.db());
        attendanceRepo_->initialize();

        resultRepo_ = std::make_unique<SQLiteResultRepository>(testDb_.db());
        resultRepo_->initialize();

        enrollmentRepo_ = std::make_unique<SQLiteEnrollmentRepository>(testDb_.db());
        enrollmentRepo_->initialize();

        service_ = std::make_unique<ReportService>(
            *deptRepo_, *courseRepo_, *studentRepo_,
            *attendanceRepo_, *resultRepo_, *enrollmentRepo_, testDb_.db()
        );
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteDepartmentRepository> deptRepo_;
    std::unique_ptr<SQLiteCourseRepository> courseRepo_;
    std::unique_ptr<SQLiteStudentRepository> studentRepo_;
    std::unique_ptr<SQLiteAttendanceRepository> attendanceRepo_;
    std::unique_ptr<SQLiteResultRepository> resultRepo_;
    std::unique_ptr<SQLiteEnrollmentRepository> enrollmentRepo_;
    std::unique_ptr<ReportService> service_;
};

// Test GetDepartmentSummaryReturnsCorrectCounts
TEST_F(ReportServiceTest, GetDepartmentSummaryReturnsCorrectCounts) {
    deptRepo_->create("Computer Science", "CS", 1);
    courseRepo_->create(Course::create("CS101", "Intro CS", 1, 1, 3, 1).value());
    
    // Insert student via SQL to link to department 1
    testDb_.db().exec("INSERT INTO users (id, email, password_hash, role) VALUES (1, 's1@ex.com', 'h', 'student'), (2, 's2@ex.com', 'h', 'student');");
    testDb_.db().exec("INSERT INTO students (id, user_id, roll_number, department_id, first_name, last_name, semester) VALUES (1, 1, 'R1', 1, 'A', 'B', 1), (2, 2, 'R2', 1, 'C', 'D', 1);");

    auto result = service_->getDepartmentSummary(1);
    ASSERT_TRUE(isSuccess(result));
    
    auto summary = getValue(result);
    EXPECT_EQ(summary.departmentName, "Computer Science");
    EXPECT_EQ(summary.totalStudents, 2);
    EXPECT_EQ(summary.totalCourses, 1);
}

// Test GetDepartmentSummaryNonExistentReturnsNotFound
TEST_F(ReportServiceTest, GetDepartmentSummaryNonExistentReturnsNotFound) {
    auto result = service_->getDepartmentSummary(999);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::NotFound);
}

// Test GetCourseAttendanceReportCalculatesPercentages
TEST_F(ReportServiceTest, GetCourseAttendanceReportCalculatesPercentages) {
    deptRepo_->create("CS", "CS", 1);
    courseRepo_->create(Course::create("CS101", "Intro CS", 1, 1, 3, 1).value());
    testDb_.db().exec("INSERT INTO users (id, email, password_hash, role) VALUES (1, 's1@ex.com', 'h', 'student');");
    testDb_.db().exec("INSERT INTO students (id, user_id, roll_number, department_id, first_name, last_name, semester) VALUES (1, 1, 'R1', 1, 'A', 'B', 1);");

    attendanceRepo_->markAttendance(1, 1, "2024-01-01", "present");
    attendanceRepo_->markAttendance(1, 1, "2024-01-02", "absent");

    auto result = service_->getCourseAttendanceReport(1);
    ASSERT_TRUE(isSuccess(result));
    
    auto report = getValue(result);
    ASSERT_EQ(report.size(), 1);
    EXPECT_EQ(report[0].studentId, 1);
    EXPECT_EQ(report[0].totalClasses, 2);
    EXPECT_EQ(report[0].presentClasses, 1);
    EXPECT_EQ(report[0].attendancePercentage, 50.0);
}

// Test GetGradeDistributionReportGroupsGrades
TEST_F(ReportServiceTest, GetGradeDistributionReportGroupsGrades) {
    deptRepo_->create("CS", "CS", 1);
    courseRepo_->create(Course::create("CS101", "Intro CS", 1, 1, 3, 1).value());
    testDb_.db().exec("INSERT INTO users (id, email, password_hash, role) VALUES (1, 's1@ex.com', 'h', 'student');");
    testDb_.db().exec("INSERT INTO students (id, user_id, roll_number, department_id, first_name, last_name, semester) VALUES (1, 1, 'R1', 1, 'A', 'B', 1);");

    resultRepo_->recordResult(ExamResult::create(1, 1, ExamType::Final, 95.0, 100.0).value());
    resultRepo_->recordResult(ExamResult::create(1, 1, ExamType::Midterm, 85.0, 100.0).value());

    auto result = service_->getGradeDistributionReport(1);
    ASSERT_TRUE(isSuccess(result));
    
    auto report = getValue(result);
    EXPECT_EQ(report.totalExams, 2);
    EXPECT_GT(report.averagePercentage, 80.0);
}

// Test GetSystemOverviewReturnsTotals
TEST_F(ReportServiceTest, GetSystemOverviewReturnsTotals) {
    deptRepo_->create("CS", "CS", 1);
    courseRepo_->create(Course::create("CS101", "Intro CS", 1, 1, 3, 1).value());

    auto result = service_->getSystemOverview();
    ASSERT_TRUE(isSuccess(result));
    
    auto overview = getValue(result);
    EXPECT_EQ(overview.totalDepartments, 1);
    EXPECT_EQ(overview.totalCourses, 1);
}

// Test GetSystemOverviewEmptyDatabaseReturnsZeros
TEST_F(ReportServiceTest, GetSystemOverviewEmptyDatabaseReturnsZeros) {
    auto result = service_->getSystemOverview();
    ASSERT_TRUE(isSuccess(result));
    
    auto overview = getValue(result);
    EXPECT_EQ(overview.totalDepartments, 0);
    EXPECT_EQ(overview.totalCourses, 0);
    EXPECT_EQ(overview.totalStudents, 0);
}
