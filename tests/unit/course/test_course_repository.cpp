#include <gtest/gtest.h>
#include <memory>
#include "course/repositories/sqlite_department_repository.hpp"
#include "course/repositories/sqlite_course_repository.hpp"
#include "auth/repositories/sqlite_user_repository.hpp"
#include "test_helpers/test_db.hpp"

using namespace ums;
using namespace ums::test;

class CourseRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize repos
        userRepo_ = std::make_unique<SQLiteUserRepository>(testDb_.db());
        userRepo_->initialize();
        
        deptRepo_ = std::make_unique<SQLiteDepartmentRepository>(testDb_.db());
        deptRepo_->initialize();
        
        courseRepo_ = std::make_unique<SQLiteCourseRepository>(testDb_.db());
        courseRepo_->initialize();
        
        // Create dummy users (professors)
        userRepo_->create("prof1@example.com", "hash", Role::Professor); // ID 1
        userRepo_->create("prof2@example.com", "hash", Role::Professor); // ID 2
    }

    TestDatabase testDb_;
    std::unique_ptr<SQLiteUserRepository> userRepo_;
    std::unique_ptr<SQLiteDepartmentRepository> deptRepo_;
    std::unique_ptr<SQLiteCourseRepository> courseRepo_;
};

// Test create department succeeds
TEST_F(CourseRepositoryTest, CreateDepartmentSucceeds) {
    auto result = deptRepo_->create("Computer Science", "CS", 1);
    ASSERT_TRUE(isSuccess(result));
    
    const Department& dept = getValue(result);
    EXPECT_GT(dept.id(), 0);
    EXPECT_EQ(dept.name(), "Computer Science");
    EXPECT_EQ(dept.code(), "CS");
    EXPECT_EQ(dept.headProfessorId(), 1);
}

// Test create duplicate department code returns conflict
TEST_F(CourseRepositoryTest, CreateDuplicateDepartmentCodeReturnsConflict) {
    ASSERT_TRUE(isSuccess(deptRepo_->create("Computer Science", "CS", 1)));
    
    auto result = deptRepo_->create("Cyber Security", "CS", 2);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test find department by id returns successfully
TEST_F(CourseRepositoryTest, FindDepartmentByIdReturns) {
    auto createResult = deptRepo_->create("Computer Science", "CS", 1);
    int id = getValue(createResult).id();
    
    auto findResult = deptRepo_->findById(id);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).code(), "CS");
}

// Test find all departments returns all
TEST_F(CourseRepositoryTest, FindAllDepartmentsReturns) {
    deptRepo_->create("Computer Science", "CS", 1);
    deptRepo_->create("Mathematics", "MATH", 2);
    
    auto findResult = deptRepo_->findAll();
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).size(), 2);
}

// Test create course succeeds
TEST_F(CourseRepositoryTest, CreateCourseSucceeds) {
    deptRepo_->create("Computer Science", "CS", 1); // Dept ID 1
    auto result = courseRepo_->create("CS101", "Intro to CS", 1, 1, 3, 1);
    ASSERT_TRUE(isSuccess(result));
    
    const Course& course = getValue(result);
    EXPECT_GT(course.id(), 0);
    EXPECT_EQ(course.code(), "CS101");
    EXPECT_EQ(course.name(), "Intro to CS");
}

// Test create duplicate course code returns conflict
TEST_F(CourseRepositoryTest, CreateDuplicateCourseCodeReturnsConflict) {
    deptRepo_->create("Computer Science", "CS", 1); // Dept ID 1
    ASSERT_TRUE(isSuccess(courseRepo_->create("CS101", "Intro to CS", 1, 1, 3, 1)));
    
    auto result = courseRepo_->create("CS101", "Advanced CS", 1, 1, 3, 2);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test find course by id returns successfully
TEST_F(CourseRepositoryTest, FindCourseByIdReturns) {
    deptRepo_->create("Computer Science", "CS", 1); // Dept ID 1
    auto createResult = courseRepo_->create("CS101", "Intro to CS", 1, 1, 3, 1);
    int id = getValue(createResult).id();
    
    auto findResult = courseRepo_->findById(id);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).code(), "CS101");
}

// Test find courses by department returns list
TEST_F(CourseRepositoryTest, FindCoursesByDepartmentReturns) {
    deptRepo_->create("Computer Science", "CS", 1); // Dept ID 1
    courseRepo_->create("CS101", "Intro to CS", 1, 1, 3, 1);
    courseRepo_->create("CS102", "Data Structures", 1, 2, 4, 2);
    
    auto findResult = courseRepo_->findByDepartmentId(1);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).size(), 2);
}

// Test find courses by professor returns list
TEST_F(CourseRepositoryTest, FindCoursesByProfessorReturns) {
    deptRepo_->create("Computer Science", "CS", 1); // Dept ID 1
    courseRepo_->create("CS101", "Intro to CS", 1, 1, 3, 1);
    courseRepo_->create("CS201", "Algorithms", 1, 1, 4, 3);
    
    auto findResult = courseRepo_->findByProfessorId(1);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).size(), 2);
}

// Test find courses by semester returns list
TEST_F(CourseRepositoryTest, FindCoursesBySemesterReturns) {
    deptRepo_->create("Computer Science", "CS", 1); // Dept ID 1
    courseRepo_->create("CS101", "Intro to CS", 1, 1, 3, 1);
    courseRepo_->create("MATH101", "Calculus", 1, 2, 4, 1);
    
    auto findResult = courseRepo_->findBySemester(1);
    ASSERT_TRUE(isSuccess(findResult));
    EXPECT_EQ(getValue(findResult).size(), 2);
}
