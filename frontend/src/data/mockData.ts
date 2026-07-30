import { User, Student, Department, Course, AttendanceRecord, ExamResult, Enrollment, SystemOverview } from '../types';

export const DEMO_USERS: Record<string, { email: string; role: 'student' | 'professor' | 'admin'; pass: string; name: string }> = {
  student: {
    email: 'student@example.com',
    role: 'student',
    pass: 'password123',
    name: 'Alex Johnson',
  },
  professor: {
    email: 'prof@example.com',
    role: 'professor',
    pass: 'password123',
    name: 'Dr. Sarah Connor',
  },
  admin: {
    email: 'admin@example.com',
    role: 'admin',
    pass: 'password123',
    name: 'Dean Robert Vance',
  },
};

export const INITIAL_DEPARTMENTS: Department[] = [
  { id: 1, name: 'Computer Science & Engineering', code: 'CS', headProfessorId: 2 },
  { id: 2, name: 'Electrical & Electronics', code: 'EE', headProfessorId: 3 },
  { id: 3, name: 'Mechanical Engineering', code: 'ME', headProfessorId: 4 },
  { id: 4, name: 'Business & Management', code: 'BM', headProfessorId: 5 },
];

export const INITIAL_COURSES: Course[] = [
  { id: 101, code: 'CS101', name: 'Data Structures & Algorithms', departmentId: 1, professorId: 2, credits: 4, semester: 3, departmentName: 'Computer Science' },
  { id: 102, code: 'CS102', name: 'Database Management Systems', departmentId: 1, professorId: 2, credits: 3, semester: 3, departmentName: 'Computer Science' },
  { id: 103, code: 'CS201', name: 'Operating Systems & System Programming', departmentId: 1, professorId: 2, credits: 4, semester: 4, departmentName: 'Computer Science' },
  { id: 104, code: 'EE101', name: 'Circuit Theory & Digital Logic', departmentId: 2, professorId: 3, credits: 4, semester: 1, departmentName: 'Electrical' },
  { id: 105, code: 'BM101', name: 'Principles of Financial Accounting', departmentId: 4, professorId: 5, credits: 3, semester: 2, departmentName: 'Business' },
];

export const INITIAL_STUDENT_PROFILE: Student = {
  id: 1,
  userId: 1,
  rollNumber: 'CS2024001',
  departmentId: 1,
  firstName: 'Alex',
  lastName: 'Johnson',
  semester: 3,
  departmentName: 'Computer Science & Engineering',
};

export const INITIAL_ATTENDANCE: AttendanceRecord[] = [
  { id: 1, courseId: 101, studentId: 1, date: '2026-07-28', status: 'present', courseName: 'Data Structures & Algorithms' },
  { id: 2, courseId: 101, studentId: 1, date: '2026-07-29', status: 'present', courseName: 'Data Structures & Algorithms' },
  { id: 3, courseId: 101, studentId: 1, date: '2026-07-30', status: 'late', courseName: 'Data Structures & Algorithms' },
  { id: 4, courseId: 102, studentId: 1, date: '2026-07-28', status: 'present', courseName: 'Database Management Systems' },
  { id: 5, courseId: 102, studentId: 1, date: '2026-07-29', status: 'absent', courseName: 'Database Management Systems' },
];

export const INITIAL_RESULTS: ExamResult[] = [
  { id: 1, studentId: 1, courseId: 101, examType: 'midterm', marksObtained: 46, maxMarks: 50, percentage: 92.0, grade: 'A+', courseName: 'Data Structures & Algorithms' },
  { id: 2, studentId: 1, courseId: 101, examType: 'quiz', marksObtained: 19, maxMarks: 20, percentage: 95.0, grade: 'A+', courseName: 'Data Structures & Algorithms' },
  { id: 3, studentId: 1, courseId: 102, examType: 'midterm', marksObtained: 41, maxMarks: 50, percentage: 82.0, grade: 'A', courseName: 'Database Management Systems' },
  { id: 4, studentId: 1, courseId: 103, examType: 'assignment', marksObtained: 28, maxMarks: 30, percentage: 93.3, grade: 'A+', courseName: 'Operating Systems' },
];

export const INITIAL_ENROLLMENTS: Enrollment[] = [
  { id: 1, studentId: 1, courseId: 101, enrolledAt: '2026-01-15', courseCode: 'CS101', courseName: 'Data Structures & Algorithms' },
  { id: 2, studentId: 1, courseId: 102, enrolledAt: '2026-01-15', courseCode: 'CS102', courseName: 'Database Management Systems' },
  { id: 3, studentId: 1, courseId: 103, enrolledAt: '2026-01-15', courseCode: 'CS201', courseName: 'Operating Systems & System Programming' },
];

export const INITIAL_ANNOUNCEMENTS = [
  {
    id: 'ann_1',
    title: 'Fall 2026 Midterm Exam Schedule Published',
    content: 'The official examination timetable for all engineering and management programs is now available on the portal. Please verify your seat numbers.',
    date: '2 hours ago',
    category: 'Exam',
    author: 'Academic Registrar',
  },
  {
    id: 'ann_2',
    title: 'Annual Tech Symposium "InnoHack 2026" Registration Open',
    content: 'Participate in the flagship hackathon featuring tracks in AI/ML, Cloud Systems, and Robotics. Cash prizes worth $10,000!',
    date: '1 day ago',
    category: 'Event',
    author: 'Department of Computer Science',
  },
  {
    id: 'ann_3',
    title: 'Library Extended Hours During Finals Week',
    content: 'The central campus library will remain open 24/7 with quiet study zones and complimentary coffee facilities for students.',
    date: '3 days ago',
    category: 'Notice',
    author: 'Library Administration',
  },
];
