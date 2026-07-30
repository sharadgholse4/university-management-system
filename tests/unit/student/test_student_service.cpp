#include <gtest/gtest.h>
#include <memory>
#include "student/services/student_service.hpp"
#include "test_helpers/mock_student_repository.hpp"

using namespace ums;
using namespace ums::test;
using ::testing::_;
using ::testing::Return;

class StudentServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockRepo_ = std::make_shared<MockStudentRepository>();
        service_ = std::make_unique<StudentService>(*mockRepo_);
    }

    std::shared_ptr<MockStudentRepository> mockRepo_;
    std::unique_ptr<StudentService> service_;
};

// Test create student valid data succeeds
TEST_F(StudentServiceTest, CreateStudentValidDataSucceeds) {
    CreateStudentRequest req{1, "CS2023001", "John", "Doe", 1, 3};
    
    Student expectedStudent = Student::fromDatabase(1, 1, "CS2023001", "John", "Doe", 1, 3, "2024-01-01", "2024-01-01", "2024-01-01");
    
    EXPECT_CALL(*mockRepo_, existsByUserId(1))
        .WillOnce(Return(Result<bool>(false)));
    EXPECT_CALL(*mockRepo_, existsByRollNumber("CS2023001"))
        .WillOnce(Return(Result<bool>(false)));
    EXPECT_CALL(*mockRepo_, create(1, "CS2023001", "John", "Doe", 1, 3))
        .WillOnce(Return(Result<Student>(expectedStudent)));
        
    auto result = service_->createStudent(req);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).rollNumber(), "CS2023001");
}

// Test create student duplicate user ID returns conflict
TEST_F(StudentServiceTest, CreateStudentDuplicateUserIdReturnsConflict) {
    CreateStudentRequest req{1, "CS2023001", "John", "Doe", 1, 3};
    
    EXPECT_CALL(*mockRepo_, existsByUserId(1))
        .WillOnce(Return(Result<bool>(true)));
        
    auto result = service_->createStudent(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test create student duplicate roll number returns conflict
TEST_F(StudentServiceTest, CreateStudentDuplicateRollNumberReturnsConflict) {
    CreateStudentRequest req{1, "CS2023001", "John", "Doe", 1, 3};
    
    EXPECT_CALL(*mockRepo_, existsByUserId(1))
        .WillOnce(Return(Result<bool>(false)));
    EXPECT_CALL(*mockRepo_, existsByRollNumber("CS2023001"))
        .WillOnce(Return(Result<bool>(true)));
        
    auto result = service_->createStudent(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::Conflict);
}

// Test create student invalid data returns validation error
TEST_F(StudentServiceTest, CreateStudentInvalidDataReturnsValidationError) {
    CreateStudentRequest req{1, "", "John", "Doe", 1, 3};
    
    auto result = service_->createStudent(req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test get student by id existing returns
TEST_F(StudentServiceTest, GetStudentByIdExistingReturns) {
    Student expectedStudent = Student::fromDatabase(1, 1, "CS2023001", "John", "Doe", 1, 3, "2024-01-01", "2024-01-01", "2024-01-01");
    
    EXPECT_CALL(*mockRepo_, findById(1))
        .WillOnce(Return(Result<Student>(expectedStudent)));
        
    auto result = service_->getStudentById(1);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).id(), 1);
}

// Test get student by id non-existent returns not found
TEST_F(StudentServiceTest, GetStudentByIdNonExistentReturnsNotFound) {
    EXPECT_CALL(*mockRepo_, findById(999))
        .WillOnce(Return(Result<Student>(AppError(ErrorCode::NotFound, "Not found"))));
        
    auto result = service_->getStudentById(999);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::NotFound);
}

// Test update student valid data succeeds
TEST_F(StudentServiceTest, UpdateStudentValidDataSucceeds) {
    UpdateStudentRequest req{"Johnny", "Doee", 2, 4};
    Student existingStudent = Student::fromDatabase(1, 1, "CS2023001", "John", "Doe", 1, 3, "2024-01-01", "2024-01-01", "2024-01-01");
    Student expectedStudent = Student::fromDatabase(1, 1, "CS2023001", "Johnny", "Doee", 2, 4, "2024-01-01", "2024-01-01", "2024-01-01");
    
    // WHY: StudentService::updateStudent first checks if the student exists via findById,
    // then delegates to repo.update(). Both expectations must be set up.
    EXPECT_CALL(*mockRepo_, findById(1))
        .WillOnce(Return(Result<Student>(existingStudent)));
    EXPECT_CALL(*mockRepo_, update(1, "Johnny", "Doee", 2, 4))
        .WillOnce(Return(Result<Student>(expectedStudent)));
        
    auto result = service_->updateStudent(1, req);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).firstName(), "Johnny");
}

// Test update student invalid semester returns validation error
TEST_F(StudentServiceTest, UpdateStudentInvalidSemesterReturnsValidationError) {
    UpdateStudentRequest req{"Johnny", "Doee", 2, 9};
    
    auto result = service_->updateStudent(1, req);
    ASSERT_FALSE(isSuccess(result));
    EXPECT_EQ(getError(result).code, ErrorCode::ValidationError);
}

// Test get all students returns
TEST_F(StudentServiceTest, GetAllStudentsReturns) {
    std::vector<Student> expectedStudents;
    expectedStudents.push_back(Student::fromDatabase(1, 1, "CS2023001", "John", "Doe", 1, 3, "2024-01-01", "2024-01-01", "2024-01-01"));
    
    EXPECT_CALL(*mockRepo_, findAll())
        .WillOnce(Return(Result<std::vector<Student>>(expectedStudents)));
        
    auto result = service_->getAllStudents();
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}

// Test get students by department returns
TEST_F(StudentServiceTest, GetStudentsByDepartmentReturns) {
    std::vector<Student> expectedStudents;
    expectedStudents.push_back(Student::fromDatabase(1, 1, "CS2023001", "John", "Doe", 2, 3, "2024-01-01", "2024-01-01", "2024-01-01"));
    
    EXPECT_CALL(*mockRepo_, findByDepartment(2))
        .WillOnce(Return(Result<std::vector<Student>>(expectedStudents)));
        
    auto result = service_->getStudentsByDepartment(2);
    ASSERT_TRUE(isSuccess(result));
    EXPECT_EQ(getValue(result).size(), 1);
}
