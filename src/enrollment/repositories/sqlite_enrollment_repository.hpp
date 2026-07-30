#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include "enrollment/repositories/i_enrollment_repository.hpp"

namespace ums {

class SQLiteEnrollmentRepository : public IEnrollmentRepository {
public:
    explicit SQLiteEnrollmentRepository(SQLite::Database& db);

    void initialize() override;
    Result<Enrollment> findById(int id) override;
    Result<Enrollment> create(int studentId, int courseId) override;
    Result<bool> deleteById(int id) override;
    Result<bool> deleteByStudentAndCourse(int studentId, int courseId) override;
    Result<std::vector<Enrollment>> findByStudentId(int studentId) override;
    Result<std::vector<Enrollment>> findByCourseId(int courseId) override;
    Result<bool> existsByStudentAndCourse(int studentId, int courseId) override;

private:
    SQLite::Database& db_;
};

} // namespace ums
