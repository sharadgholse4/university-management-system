#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include "results/repositories/i_result_repository.hpp"

namespace ums {

class SQLiteResultRepository : public IResultRepository {
public:
    explicit SQLiteResultRepository(SQLite::Database& db);
    
    void initialize() override;
    Result<ExamResult> findById(int id) override;
    Result<ExamResult> create(int studentId, int courseId, const std::string& examType,
                             double marksObtained, double maxMarks, const std::string& grade,
                             int semester) override;
    Result<ExamResult> update(int id, double marksObtained, double maxMarks,
                             const std::string& grade) override;
    Result<std::vector<ExamResult>> findByStudentId(int studentId) override;
    Result<std::vector<ExamResult>> findByStudentAndSemester(int studentId, int semester) override;
    Result<std::vector<ExamResult>> findByCourseId(int courseId) override;
    Result<bool> existsByStudentCourseExamType(int studentId, int courseId, const std::string& examType) override;

private:
    SQLite::Database& db_;
    ExamResult extractResult(SQLite::Statement& stmt);
};

} // namespace ums
