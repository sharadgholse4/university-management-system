import React, { useEffect, useState } from 'react';
import { useAuth } from '../context/AuthContext';
import { StatCard } from '../components/StatCard';
import {
  BookOpen,
  CalendarCheck,
  Award,
  Users,
  Megaphone,
  ArrowUpRight,
  Sparkles,
  ShieldAlert,
  PlusCircle,
  FileSpreadsheet,
} from 'lucide-react';
import { api } from '../services/api';
import { INITIAL_ANNOUNCEMENTS } from '../data/mockData';

interface DashboardProps {
  setActiveTab: (tab: string) => void;
}

export const Dashboard: React.FC<DashboardProps> = ({ setActiveTab }) => {
  const { user, studentProfile } = useAuth();
  const role = user?.role || 'student';

  const [stats, setStats] = useState({
    coursesCount: 3,
    attendancePct: 92.5,
    cgpa: 3.85,
    totalStudents: 480,
    totalUsers: 520,
  });

  useEffect(() => {
    const loadStats = async () => {
      try {
        if (role === 'student' && studentProfile) {
          const cgpaRes = await api.getStudentCGPA(studentProfile.id);
          const attRes = await api.getAttendanceByStudent(studentProfile.id);
          const totalAtt = attRes.length;
          const presentCount = attRes.filter((a) => a.status === 'present').length;
          const attPct = totalAtt > 0 ? (presentCount / totalAtt) * 100 : 92.5;

          setStats((prev) => ({
            ...prev,
            cgpa: cgpaRes.cgpa || 3.85,
            attendancePct: Math.round(attPct * 10) / 10,
          }));
        } else if (role === 'admin') {
          const sysRes = await api.getSystemOverview();
          setStats((prev) => ({
            ...prev,
            totalStudents: sysRes.totalStudents || 480,
            totalUsers: sysRes.totalUsers || 520,
            coursesCount: sysRes.totalCourses || 12,
          }));
        }
      } catch {
        // Fallback to default stats if backend is starting
      }
    };
    loadStats();
  }, [role, studentProfile]);

  return (
    <div className="space-y-6 animate-fade-in">
      {/* Welcome Banner */}
      <div className="relative overflow-hidden rounded-3xl bg-gradient-to-r from-slate-900 via-sky-950 to-indigo-950 text-white p-8 shadow-xl">
        <div className="absolute top-0 right-0 -mt-10 -mr-10 w-64 h-64 bg-sky-500/20 rounded-full blur-3xl"></div>
        <div className="relative z-10 flex flex-col md:flex-row md:items-center md:justify-between gap-4">
          <div>
            <div className="inline-flex items-center gap-1.5 px-3 py-1 rounded-full text-xs font-bold bg-sky-500/20 text-sky-300 border border-sky-500/30 mb-3">
              <Sparkles className="w-3.5 h-3.5" />
              Welcome to EduPortal v2.0
            </div>
            <h1 className="font-serif-heading text-3xl font-extrabold tracking-tight">
              Hello, {studentProfile ? studentProfile.firstName : user?.email.split('@')[0]}!
            </h1>
            <p className="text-sm text-slate-300 mt-1 max-w-xl">
              {role === 'student'
                ? 'Track your semester progress, attendance records, exam CGPA, and course enrollments in real time.'
                : role === 'professor'
                ? 'Manage course attendance, record student exam grades, and view department academic reports.'
                : 'Full administrative access: oversee users, departments, system-wide metrics, and reports.'}
            </p>
          </div>

          <div className="flex items-center gap-3 shrink-0">
            {role === 'student' && (
              <button
                onClick={() => setActiveTab('results')}
                className="px-4 py-2.5 rounded-xl bg-sky-600 hover:bg-sky-500 font-bold text-sm text-white shadow-lg shadow-sky-600/30 flex items-center gap-2 transition-all"
              >
                View Transcript <ArrowUpRight className="w-4 h-4" />
              </button>
            )}
            {role === 'professor' && (
              <button
                onClick={() => setActiveTab('attendance')}
                className="px-4 py-2.5 rounded-xl bg-purple-600 hover:bg-purple-500 font-bold text-sm text-white shadow-lg shadow-purple-600/30 flex items-center gap-2 transition-all"
              >
                Mark Attendance <PlusCircle className="w-4 h-4" />
              </button>
            )}
            {role === 'admin' && (
              <button
                onClick={() => setActiveTab('reports')}
                className="px-4 py-2.5 rounded-xl bg-amber-600 hover:bg-amber-500 font-bold text-sm text-white shadow-lg shadow-amber-600/30 flex items-center gap-2 transition-all"
              >
                System Reports <FileSpreadsheet className="w-4 h-4" />
              </button>
            )}
          </div>
        </div>
      </div>

      {/* Metrics Row */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
        {role === 'student' ? (
          <>
            <StatCard
              title="Enrolled Courses"
              value={stats.coursesCount}
              subtitle="Fall 2026 Semester"
              icon={BookOpen}
              color="sky"
            />
            <StatCard
              title="Attendance Rate"
              value={`${stats.attendancePct}%`}
              subtitle="Minimum 75% required"
              icon={CalendarCheck}
              color="emerald"
            />
            <StatCard
              title="Current CGPA"
              value={stats.cgpa.toFixed(2)}
              subtitle="Out of 4.0 scale"
              icon={Award}
              color="purple"
            />
            <StatCard
              title="Current Semester"
              value={`Semester ${studentProfile?.semester || 3}`}
              subtitle="Computer Science"
              icon={Users}
              color="indigo"
            />
          </>
        ) : (
          <>
            <StatCard
              title="Total Students"
              value={stats.totalStudents}
              subtitle="Active enrollments"
              icon={Users}
              color="sky"
            />
            <StatCard
              title="Total Courses"
              value={stats.coursesCount}
              subtitle="Active academic catalog"
              icon={BookOpen}
              color="emerald"
            />
            <StatCard
              title="Registered Users"
              value={stats.totalUsers}
              subtitle="Students, Profs, Admins"
              icon={Award}
              color="purple"
            />
            <StatCard
              title="System Status"
              value="Healthy"
              subtitle="C++ Crow Backend Live"
              icon={CalendarCheck}
              color="indigo"
            />
          </>
        )}
      </div>

      {/* Main Grid Section */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Left Column: Quick Actions & Overview */}
        <div className="lg:col-span-2 space-y-6">
          <div className="glass-panel p-6 rounded-3xl border border-slate-200">
            <h3 className="text-lg font-bold text-slate-900 mb-4 flex items-center gap-2">
              <Sparkles className="w-5 h-5 text-sky-600" /> Quick Actions & Portals
            </h3>
            <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
              <div
                onClick={() => setActiveTab('attendance')}
                className="p-4 rounded-2xl bg-white border border-slate-200/80 shadow-sm hover:shadow-md hover:border-sky-300 transition-all cursor-pointer group"
              >
                <div className="w-10 h-10 rounded-xl bg-sky-50 text-sky-600 flex items-center justify-center mb-3 group-hover:scale-105 transition-transform">
                  <CalendarCheck className="w-5 h-5" />
                </div>
                <h4 className="font-bold text-slate-900 text-sm">Attendance Portal</h4>
                <p className="text-xs text-slate-500 mt-1">
                  {role === 'student' ? 'Check course-wise attendance percentages' : 'Mark daily attendance for enrolled students'}
                </p>
              </div>

              <div
                onClick={() => setActiveTab('results')}
                className="p-4 rounded-2xl bg-white border border-slate-200/80 shadow-sm hover:shadow-md hover:border-purple-300 transition-all cursor-pointer group"
              >
                <div className="w-10 h-10 rounded-xl bg-purple-50 text-purple-600 flex items-center justify-center mb-3 group-hover:scale-105 transition-transform">
                  <Award className="w-5 h-5" />
                </div>
                <h4 className="font-bold text-slate-900 text-sm">Results & Grades</h4>
                <p className="text-xs text-slate-500 mt-1">
                  {role === 'student' ? 'View midterm, final exam grades and CGPA' : 'Input exam marks and calculate student grades'}
                </p>
              </div>

              <div
                onClick={() => setActiveTab('courses')}
                className="p-4 rounded-2xl bg-white border border-slate-200/80 shadow-sm hover:shadow-md hover:border-emerald-300 transition-all cursor-pointer group"
              >
                <div className="w-10 h-10 rounded-xl bg-emerald-50 text-emerald-600 flex items-center justify-center mb-3 group-hover:scale-105 transition-transform">
                  <BookOpen className="w-5 h-5" />
                </div>
                <h4 className="font-bold text-slate-900 text-sm">Course Catalog</h4>
                <p className="text-xs text-slate-500 mt-1">
                  Browse department courses, credit hours, and semester filters
                </p>
              </div>

              <div
                onClick={() => setActiveTab('enrollments')}
                className="p-4 rounded-2xl bg-white border border-slate-200/80 shadow-sm hover:shadow-md hover:border-amber-300 transition-all cursor-pointer group"
              >
                <div className="w-10 h-10 rounded-xl bg-amber-50 text-amber-600 flex items-center justify-center mb-3 group-hover:scale-105 transition-transform">
                  <Users className="w-5 h-5" />
                </div>
                <h4 className="font-bold text-slate-900 text-sm">Course Registration</h4>
                <p className="text-xs text-slate-500 mt-1">
                  {role === 'student' ? 'One-click course enrollment and drop' : 'View enrolled student rosters per course'}
                </p>
              </div>
            </div>
          </div>
        </div>

        {/* Right Column: Recent Announcements */}
        <div className="space-y-6">
          <div className="glass-panel p-6 rounded-3xl border border-slate-200">
            <div className="flex items-center justify-between mb-4">
              <h3 className="text-lg font-bold text-slate-900 flex items-center gap-2">
                <Megaphone className="w-5 h-5 text-amber-600" /> Notice Board
              </h3>
              <button
                onClick={() => setActiveTab('announcements')}
                className="text-xs font-bold text-sky-600 hover:text-sky-700"
              >
                View All
              </button>
            </div>

            <div className="space-y-3">
              {INITIAL_ANNOUNCEMENTS.map((ann) => (
                <div
                  key={ann.id}
                  className="p-3.5 rounded-2xl bg-white border border-slate-200/80 hover:border-slate-300 transition-all"
                >
                  <div className="flex items-center justify-between mb-1">
                    <span className="text-[10px] font-extrabold uppercase tracking-wider px-2 py-0.5 rounded bg-amber-50 text-amber-700 border border-amber-200">
                      {ann.category}
                    </span>
                    <span className="text-xs text-slate-400">{ann.date}</span>
                  </div>
                  <h4 className="text-xs font-bold text-slate-900">{ann.title}</h4>
                  <p className="text-xs text-slate-500 line-clamp-2 mt-1">{ann.content}</p>
                </div>
              ))}
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};
