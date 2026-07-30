export type Role = 'student' | 'professor' | 'admin';

export interface User {
  id: number;
  email: string;
  role: Role;
  createdAt?: string;
}

export interface Student {
  id: number;
  userId: number;
  rollNumber: string;
  departmentId: number;
  firstName: string;
  lastName: string;
  semester: number;
  departmentName?: string;
}

export interface Department {
  id: number;
  name: string;
  code: string;
  headProfessorId?: number;
  createdAt?: string;
}

export interface Course {
  id: number;
  code: string;
  name: string;
  departmentId: number;
  professorId: number;
  credits: number;
  semester: number;
  departmentName?: string;
  professorName?: string;
}

export interface AttendanceRecord {
  id: number;
  courseId: number;
  studentId: number;
  date: string;
  status: 'present' | 'absent' | 'late';
  studentName?: string;
  courseName?: string;
}

export interface ExamResult {
  id: number;
  studentId: number;
  courseId: number;
  examType: 'midterm' | 'final' | 'quiz' | 'assignment';
  marksObtained: number;
  maxMarks: number;
  percentage: number;
  grade: string;
  studentName?: string;
  courseName?: string;
}

export interface Enrollment {
  id: number;
  studentId: number;
  courseId: number;
  enrolledAt: string;
  courseCode?: string;
  courseName?: string;
}

export interface DepartmentSummary {
  departmentId: number;
  departmentName: string;
  departmentCode: string;
  totalCourses: number;
  totalStudents: number;
}

export interface StudentAttendanceSummary {
  studentId: number;
  totalClasses: number;
  presentClasses: number;
  attendedClasses: number;
  absentClasses: number;
  lateClasses: number;
  attendancePercentage: number;
}

export interface GradeDistributionSummary {
  courseId: number;
  totalExams: number;
  averagePercentage: number;
  gradeCounts: Record<string, number>;
}

export interface SystemOverview {
  totalUsers: number;
  totalStudents: number;
  totalCourses: number;
  totalDepartments: number;
  totalEnrollments: number;
}

export interface ApiResponse<T> {
  success: boolean;
  message?: string;
  data: T;
  error?: string;
}

export interface AuthState {
  token: string | null;
  user: User | null;
  studentProfile: Student | null;
  isAuthenticated: boolean;
  isLoading: boolean;
}
