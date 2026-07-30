#include <gtest/gtest.h>
#include <memory>
#include "course/services/course_service.hpp"
#include "test_helpers/mock_course_repository.hpp"

using namespace ums;
using namespace ums::test;
using ::testing::_;
using ::testing::Return;

class CourseServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockDeptRepo_ = std::make_shared<MockDepartmentRepository>();
        mockCourseRepo_ = std::make_shared<MockCourseRepository>();
        service_ = std::make_unique<CourseService>(*mockDeptRepo_, *mockCourseRepo_);
    }

    std::shared_ptr<MockDepartmentRepository> mockDeptRepo_;
    std::shared_ptr<MockCourseRepository> mockCourseRepo_;
    std::unique_ptr<CourseService> service_;
};

// Test create department valid data succeeds
TEST_F(CourseServiceTest, CreateDepartmentValidDataSucceeds) {
    CreateDepartmentRequest req{"Computer Science", "CS", 101};
    Department expectedDept = Department::fromDatabase(1, "Computer Science", "CS", 101, "2024-01-01", "2024-01-01");
    
    EXPECT_CALL(*mockDeptRepo_, existsByCode("CS"))
        .WillOnce(Return(Result<bool>(false)));
    EXPECT_CALL(*mockDeptRepo_, create("Computer Science", "CS", 101))
        .WillOnce(Return(Result<Department>(expectedDept)));
        
    auto result = service_->createDepartment(req);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).code(), "CS");
}

// Test create department duplicate code returns conflict
TEST_F(CourseServiceTest, CreateDepartmentDuplicateReturnsConflict) {
    CreateDepartmentRequest req{"Computer Science", "CS", 101};
    
    EXPECT_CALL(*mockDeptRepo_, existsByCode("CS"))
        .WillOnce(Return(Result<bool>(true)));
        
    auto result = service_->createDepartment(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test create course valid data succeeds
TEST_F(CourseServiceTest, CreateCourseValidDataSucceeds) {
    CreateCourseRequest req{"CS101", "Intro to CS", 1, 101, 3, 1};
    Course expectedCourse = Course::fromDatabase(1, "CS101", "Intro to CS", 1, 101, 3, 1, "2024-01-01", "2024-01-01");
    Department dummyDept = Department::fromDatabase(1, "Computer Science", "CS", 101, "2024-01-01", "2024-01-01");
    
    EXPECT_CALL(*mockCourseRepo_, existsByCode("CS101"))
        .WillOnce(Return(Result<bool>(false)));
    EXPECT_CALL(*mockDeptRepo_, findById(1))
        .WillOnce(Return(Result<Department>(dummyDept))); // Department must exist
    EXPECT_CALL(*mockCourseRepo_, create("CS101", "Intro to CS", 1, 101, 3, 1))
        .WillOnce(Return(Result<Course>(expectedCourse)));
        
    auto result = service_->createCourse(req);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).code(), "CS101");
}

// Test create course duplicate code returns conflict
TEST_F(CourseServiceTest, CreateCourseDuplicateReturnsConflict) {
    CreateCourseRequest req{"CS101", "Intro to CS", 1, 101, 3, 1};
    
    EXPECT_CALL(*mockCourseRepo_, existsByCode("CS101"))
        .WillOnce(Return(Result<bool>(true)));
        
    auto result = service_->createCourse(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test create course invalid department returns not found
TEST_F(CourseServiceTest, CreateCourseInvalidDepartmentReturnsNotFound) {
    CreateCourseRequest req{"CS101", "Intro to CS", 999, 101, 3, 1};
    
    EXPECT_CALL(*mockCourseRepo_, existsByCode("CS101"))
        .WillOnce(Return(Result<bool>(false)));
    EXPECT_CALL(*mockDeptRepo_, findById(999))
        .WillOnce(Return(Result<Department>(AppError(ErrorCode::NotFound, "Department not found"))));
        
    auto result = service_->createCourse(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::NotFound);
}

// Test create course invalid data returns validation error
TEST_F(CourseServiceTest, CreateCourseInvalidDataReturnsValidationError) {
    CreateCourseRequest req{"", "Intro to CS", 1, 101, 3, 1}; // Empty code
    
    auto result = service_->createCourse(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test update course valid data succeeds
TEST_F(CourseServiceTest, UpdateCourseValidDataSucceeds) {
    UpdateCourseRequest req{"Advanced CS", 102, 4, 2};
    Course existingCourse = Course::fromDatabase(1, "CS101", "Intro to CS", 1, 101, 3, 1, "2024-01-01", "2024-01-01");
    Course expectedCourse = Course::fromDatabase(1, "CS101", "Advanced CS", 1, 102, 4, 2, "2024-01-01", "2024-01-02");
    
    // WHY: CourseService::updateCourse checks if course exists, then delegates to repo.update().
    EXPECT_CALL(*mockCourseRepo_, findById(1))
        .WillOnce(Return(Result<Course>(existingCourse)));
    EXPECT_CALL(*mockCourseRepo_, update(1, "Advanced CS", 102, 4, 2))
        .WillOnce(Return(Result<Course>(expectedCourse)));
        
    auto result = service_->updateCourse(1, req);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).name(), "Advanced CS");
}

// Test get course by id existing returns successfully
TEST_F(CourseServiceTest, GetCourseByIdExistingReturns) {
    Course expectedCourse = Course::fromDatabase(1, "CS101", "Intro to CS", 1, 101, 3, 1, "2024-01-01", "2024-01-01");
    
    EXPECT_CALL(*mockCourseRepo_, findById(1))
        .WillOnce(Return(Result<Course>(expectedCourse)));
        
    auto result = service_->getCourseById(1);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).code(), "CS101");
}

// Test get courses by department returns list
TEST_F(CourseServiceTest, GetCoursesByDepartmentReturns) {
    std::vector<Course> courses;
    courses.push_back(Course::fromDatabase(1, "CS101", "Intro to CS", 1, 101, 3, 1, "2024-01-01", "2024-01-01"));
    
    EXPECT_CALL(*mockCourseRepo_, findByDepartmentId(1))
        .WillOnce(Return(Result<std::vector<Course>>(courses)));
        
    auto result = service_->getCoursesByDepartment(1);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}

// Test get courses by professor returns list
TEST_F(CourseServiceTest, GetCoursesByProfessorReturns) {
    std::vector<Course> courses;
    courses.push_back(Course::fromDatabase(1, "CS101", "Intro to CS", 1, 101, 3, 1, "2024-01-01", "2024-01-01"));
    
    EXPECT_CALL(*mockCourseRepo_, findByProfessorId(101))
        .WillOnce(Return(Result<std::vector<Course>>(courses)));
        
    auto result = service_->getCoursesByProfessor(101);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}
