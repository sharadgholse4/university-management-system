import {
  ApiResponse,
  User,
  Student,
  Department,
  Course,
  AttendanceRecord,
  ExamResult,
  Enrollment,
  DepartmentSummary,
  StudentAttendanceSummary,
  GradeDistributionSummary,
  SystemOverview,
  Role,
} from '../types';

const API_BASE_URL = '/api';

class ApiService {
  private token: string | null = localStorage.getItem('ums_jwt_token');

  setToken(token: string | null) {
    this.token = token;
    if (token) {
      localStorage.setItem('ums_jwt_token', token);
    } else {
      localStorage.removeItem('ums_jwt_token');
    }
  }

  getToken(): string | null {
    return this.token || localStorage.getItem('ums_jwt_token');
  }

  private async request<T>(
    endpoint: string,
    options: RequestInit = {}
  ): Promise<ApiResponse<T>> {
    const headers: Record<string, string> = {
      'Content-Type': 'application/json',
      ...(options.headers as Record<string, string>),
    };

    const token = this.getToken();
    if (token) {
      headers['Authorization'] = `Bearer ${token}`;
    }

    try {
      const response = await fetch(`${API_BASE_URL}${endpoint}`, {
        ...options,
        headers,
      });

      const json = await response.json();

      if (!response.ok) {
        throw new Error(json.error || json.message || `HTTP ${response.status}`);
      }

      return json;
    } catch (err: any) {
      console.warn(`API Error [${endpoint}]:`, err.message);
      throw err;
    }
  }

  // Health Check
  async checkHealth(): Promise<{ status: string; timestamp?: string }> {
    try {
      const res = await fetch(`${API_BASE_URL}/health`);
      if (res.ok) {
        return await res.json();
      }
      return { status: 'offline' };
    } catch {
      return { status: 'offline' };
    }
  }

  // Authentication
  async register(email: string, password: string, role: Role): Promise<User> {
    const res = await this.request<User>('/auth/register', {
      method: 'POST',
      body: JSON.stringify({ email, password, role }),
    });
    return res.data;
  }

  async login(email: string, password: string): Promise<{ token: string; user: User }> {
    const res = await this.request<{ token: string; user: User }>('/auth/login', {
      method: 'POST',
      body: JSON.stringify({ email, password }),
    });
    this.setToken(res.data.token);
    return res.data;
  }

  async getCurrentUser(): Promise<User> {
    const res = await this.request<User>('/auth/me');
    return res.data;
  }

  // Student Profiles
  async createStudent(data: {
    userId: number;
    rollNumber: string;
    firstName: string;
    lastName: string;
    departmentId: number;
    semester: number;
  }): Promise<Student> {
    const res = await this.request<Student>('/students', {
      method: 'POST',
      body: JSON.stringify(data),
    });
    return res.data;
  }

  async getStudentById(id: number): Promise<Student> {
    const res = await this.request<Student>(`/students/${id}`);
    return res.data;
  }

  async getStudentByUserId(userId: number): Promise<Student> {
    const res = await this.request<Student>(`/students/user/${userId}`);
    return res.data;
  }

  async updateStudent(id: number, data: Partial<Student>): Promise<Student> {
    const res = await this.request<Student>(`/students/${id}`, {
      method: 'PUT',
      body: JSON.stringify(data),
    });
    return res.data;
  }

  // Attendance
  async markAttendance(data: {
    courseId: number;
    studentId: number;
    date: string;
    status: 'present' | 'absent' | 'late';
  }): Promise<AttendanceRecord> {
    const res = await this.request<AttendanceRecord>('/attendance', {
      method: 'POST',
      body: JSON.stringify(data),
    });
    return res.data;
  }

  async getAttendanceByStudent(studentId: number): Promise<AttendanceRecord[]> {
    const res = await this.request<AttendanceRecord[]>(`/attendance/student/${studentId}`);
    return res.data || [];
  }

  async getAttendanceByCourse(courseId: number): Promise<AttendanceRecord[]> {
    const res = await this.request<AttendanceRecord[]>(`/attendance/course/${courseId}`);
    return res.data || [];
  }

  // Results & CGPA
  async recordResult(data: {
    studentId: number;
    courseId: number;
    examType: string;
    marksObtained: number;
    maxMarks: number;
  }): Promise<ExamResult> {
    const res = await this.request<ExamResult>('/results', {
      method: 'POST',
      body: JSON.stringify(data),
    });
    return res.data;
  }

  async getResultsByStudent(studentId: number): Promise<ExamResult[]> {
    const res = await this.request<ExamResult[]>(`/results/student/${studentId}`);
    return res.data || [];
  }

  async getResultsByCourse(courseId: number): Promise<ExamResult[]> {
    const res = await this.request<ExamResult[]>(`/results/course/${courseId}`);
    return res.data || [];
  }

  async getStudentCGPA(studentId: number): Promise<{ studentId: number; cgpa: number; totalCredits: number }> {
    const res = await this.request<{ studentId: number; cgpa: number; totalCredits: number }>(`/results/cgpa/${studentId}`);
    return res.data;
  }

  // Departments
  async getDepartments(): Promise<Department[]> {
    const res = await this.request<Department[]>('/departments');
    return res.data || [];
  }

  async createDepartment(name: string, code: string, headProfessorId?: number): Promise<Department> {
    const res = await this.request<Department>('/departments', {
      method: 'POST',
      body: JSON.stringify({ name, code, headProfessorId }),
    });
    return res.data;
  }

  // Courses
  async getCourses(): Promise<Course[]> {
    const res = await this.request<Course[]>('/courses');
    return res.data || [];
  }

  async createCourse(data: {
    code: string;
    name: string;
    departmentId: number;
    professorId: number;
    credits: number;
    semester: number;
  }): Promise<Course> {
    const res = await this.request<Course>('/courses', {
      method: 'POST',
      body: JSON.stringify(data),
    });
    return res.data;
  }

  async getCoursesBySemester(semester: number): Promise<Course[]> {
    const res = await this.request<Course[]>(`/courses/semester/${semester}`);
    return res.data || [];
  }

  // Enrollments
  async enrollStudent(studentId: number, courseId: number): Promise<Enrollment> {
    const res = await this.request<Enrollment>('/enrollments', {
      method: 'POST',
      body: JSON.stringify({ studentId, courseId }),
    });
    return res.data;
  }

  async dropEnrollment(studentId: number, courseId: number): Promise<boolean> {
    const res = await this.request<boolean>(`/enrollments/student/${studentId}/course/${courseId}`, {
      method: 'DELETE',
    });
    return res.data;
  }

  async getEnrollmentsByStudent(studentId: number): Promise<Enrollment[]> {
    const res = await this.request<Enrollment[]>(`/enrollments/student/${studentId}`);
    return res.data || [];
  }

  async getEnrollmentsByCourse(courseId: number): Promise<Enrollment[]> {
    const res = await this.request<Enrollment[]>(`/enrollments/course/${courseId}`);
    return res.data || [];
  }

  // Reports & Analytics
  async getDepartmentReport(departmentId: number): Promise<DepartmentSummary> {
    const res = await this.request<DepartmentSummary>(`/reports/department/${departmentId}`);
    return res.data;
  }

  async getCourseAttendanceReport(courseId: number): Promise<StudentAttendanceSummary[]> {
    const res = await this.request<StudentAttendanceSummary[]>(`/reports/course/${courseId}/attendance`);
    return res.data || [];
  }

  async getGradeDistributionReport(courseId: number): Promise<GradeDistributionSummary> {
    const res = await this.request<GradeDistributionSummary>(`/reports/course/${courseId}/grades`);
    return res.data;
  }

  async getSystemOverview(): Promise<SystemOverview> {
    const res = await this.request<SystemOverview>('/reports/system');
    return res.data;
  }
}

export const api = new ApiService();
