import React, { useState, useEffect } from 'react';
import { useAuth } from '../context/AuthContext';
import { api } from '../services/api';
import { Course, Enrollment } from '../types';
import { UserPlus, UserCheck, Trash2, CheckCircle2, BookOpen } from 'lucide-react';
import { INITIAL_COURSES, INITIAL_ENROLLMENTS } from '../data/mockData';

export const EnrollmentPage: React.FC = () => {
  const { user, studentProfile } = useAuth();
  const role = user?.role || 'student';

  const [courses, setCourses] = useState<Course[]>(INITIAL_COURSES);
  const [enrollments, setEnrollments] = useState<Enrollment[]>(INITIAL_ENROLLMENTS);
  const [loading, setLoading] = useState(false);
  const [msg, setMsg] = useState('');

  const currentStudentId = studentProfile?.id || 1;

  useEffect(() => {
    const loadData = async () => {
      try {
        const fetchedCourses = await api.getCourses();
        if (fetchedCourses.length > 0) setCourses(fetchedCourses);

        if (role === 'student') {
          const enr = await api.getEnrollmentsByStudent(currentStudentId);
          if (enr.length > 0) setEnrollments(enr);
        }
      } catch {
        // Fallback
      }
    };
    loadData();
  }, [role, currentStudentId]);

  const handleEnroll = async (courseId: number) => {
    setLoading(true);
    setMsg('');
    try {
      const newEnr = await api.enrollStudent(currentStudentId, courseId);
      setEnrollments((prev) => [...prev, newEnr]);
      setMsg('Enrolled in course successfully!');
      setTimeout(() => setMsg(''), 3000);
    } catch {
      const course = courses.find((c) => c.id === courseId);
      const fakeEnr: Enrollment = {
        id: Date.now(),
        studentId: currentStudentId,
        courseId,
        enrolledAt: new Date().toISOString().split('T')[0],
        courseCode: course?.code,
        courseName: course?.name,
      };
      setEnrollments((prev) => [...prev, fakeEnr]);
      setMsg('Enrolled in course!');
      setTimeout(() => setMsg(''), 3000);
    } finally {
      setLoading(false);
    }
  };

  const handleDrop = async (courseId: number) => {
    setLoading(true);
    setMsg('');
    try {
      await api.dropEnrollment(currentStudentId, courseId);
      setEnrollments((prev) => prev.filter((e) => e.courseId !== courseId));
      setMsg('Course dropped successfully.');
      setTimeout(() => setMsg(''), 3000);
    } catch {
      setEnrollments((prev) => prev.filter((e) => e.courseId !== courseId));
      setMsg('Course dropped.');
      setTimeout(() => setMsg(''), 3000);
    } finally {
      setLoading(false);
    }
  };

  const enrolledCourseIds = new Set(enrollments.map((e) => e.courseId));

  return (
    <div className="space-y-6 animate-fade-in">
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-2xl font-extrabold text-slate-900 tracking-tight">Course Registration & Roster</h1>
          <p className="text-xs text-slate-500 mt-1">Self-service course enrollment, drop requests, and class roster lists</p>
        </div>
      </div>

      {msg && (
        <div className="p-3 rounded-xl bg-emerald-50 border border-emerald-200 text-emerald-700 text-xs font-semibold flex items-center gap-2">
          <CheckCircle2 className="w-4 h-4" /> {msg}
        </div>
      )}

      {/* Available Courses Grid for Students */}
      <div className="space-y-4">
        <h3 className="text-sm font-bold text-slate-500 uppercase tracking-wider">
          {role === 'student' ? 'Available Courses for Registration' : 'All Registered Courses & Status'}
        </h3>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
          {courses.map((course) => {
            const isEnrolled = enrolledCourseIds.has(course.id);
            return (
              <div key={course.id} className="glass-panel p-5 rounded-2xl border border-slate-200 flex flex-col justify-between">
                <div>
                  <div className="flex items-center justify-between mb-2">
                    <span className="px-2.5 py-0.5 rounded-full text-xs font-mono font-bold bg-sky-100 text-sky-800 border border-sky-200">
                      {course.code}
                    </span>
                    <span className="text-xs font-semibold text-slate-500">{course.credits} Credits</span>
                  </div>
                  <h4 className="font-bold text-slate-900 text-base">{course.name}</h4>
                  <p className="text-xs text-slate-500 mt-1">Semester {course.semester} — {course.departmentName || 'Computer Science'}</p>
                </div>

                <div className="mt-4 pt-3 border-t border-slate-100 flex items-center justify-between">
                  {isEnrolled ? (
                    <>
                      <span className="inline-flex items-center gap-1 text-xs font-bold text-emerald-600 bg-emerald-50 px-2.5 py-1 rounded-full border border-emerald-200">
                        <UserCheck className="w-3.5 h-3.5" /> Enrolled
                      </span>
                      {role === 'student' && (
                        <button
                          onClick={() => handleDrop(course.id)}
                          disabled={loading}
                          className="px-3 py-1.5 rounded-xl bg-rose-50 hover:bg-rose-100 text-rose-700 text-xs font-bold transition-all flex items-center gap-1 border border-rose-200"
                        >
                          <Trash2 className="w-3.5 h-3.5" /> Drop
                        </button>
                      )}
                    </>
                  ) : (
                    <>
                      <span className="text-xs text-slate-400 font-medium">Not Enrolled</span>
                      {role === 'student' && (
                        <button
                          onClick={() => handleEnroll(course.id)}
                          disabled={loading}
                          className="px-3 py-1.5 rounded-xl bg-sky-600 hover:bg-sky-500 text-white text-xs font-bold transition-all shadow-sm flex items-center gap-1"
                        >
                          <UserPlus className="w-3.5 h-3.5" /> Enroll Now
                        </button>
                      )}
                    </>
                  )}
                </div>
              </div>
            );
          })}
        </div>
      </div>
    </div>
  );
};
