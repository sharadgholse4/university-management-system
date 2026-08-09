export type Role = 'student' | 'professor' | 'admin';

export interface User {
  id: number | string;
  username: string;
  name: string;
  email: string;
  role: Role;
  department: string;
  rollNumber?: string | null;
  designation?: string | null;
  gpa?: number;
  picture?: string;
  googleVerified?: boolean;
}

export interface DemoPreset {
  username: string;
  name: string;
  role: Role;
  email: string;
  department: string;
  rollNumber?: string;
  designation?: string;
  gpa?: number;
}

export interface Course {
  id: number | string;
  code: string;
  name: string;
  department: string;
  credits: number;
  semester: number;
  instructor: string;
  enrolled: number;
}

export interface AttendanceRecord {
  id: number | string;
  date: string;
  course: string;
  student: string;
  status: 'Present' | 'Absent';
}

export interface TranscriptResult {
  id: number | string;
  course: string;
  exam: string;
  score: string;
  grade: string;
  points: number;
}

export interface BulletinNotice {
  id: number | string;
  title: string;
  category: 'Academic' | 'Examination' | 'Research' | 'Campus Event';
  date: string;
  author: string;
  content: string;
}

export interface AuthContextType {
  user: User | null;
  token: string | null;
  loading: boolean;
  login: (username: string, password?: string) => Promise<{ success: boolean }>;
  register: (accountData: {
    name: string;
    email: string;
    username: string;
    password?: string;
    role: Role;
    department: string;
    rollNumber?: string;
    designation?: string;
  }) => Promise<{ success: boolean }>;
  loginWithGoogle: (credentialOrUser: any) => Promise<{ success: boolean }>;
  logout: () => void;
}
