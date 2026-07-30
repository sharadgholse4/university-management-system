CREATE TABLE IF NOT EXISTS results (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id INTEGER NOT NULL,
    course_id INTEGER NOT NULL,
    exam_type TEXT NOT NULL CHECK(exam_type IN ('midterm', 'final', 'assignment', 'quiz')),
    marks_obtained REAL NOT NULL CHECK(marks_obtained >= 0),
    max_marks REAL NOT NULL CHECK(max_marks > 0 AND marks_obtained <= max_marks),
    grade TEXT NOT NULL,
    semester INTEGER NOT NULL CHECK(semester >= 1 AND semester <= 8),
    created_at TEXT NOT NULL DEFAULT (datetime('now')),
    updated_at TEXT NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (student_id) REFERENCES students(id),
    UNIQUE(student_id, course_id, exam_type)
);
CREATE INDEX IF NOT EXISTS idx_results_student_id ON results(student_id);
CREATE INDEX IF NOT EXISTS idx_results_course_id ON results(course_id);
CREATE INDEX IF NOT EXISTS idx_results_student_semester ON results(student_id, semester);
