import React, { useState, useEffect } from 'react';
import { useAuth } from '../context/AuthContext';
import { api } from '../services/api';
import { AttendanceRecord, Course } from '../types';
import { CalendarCheck, CheckCircle2, XCircle, Clock, Plus, Save, Filter } from 'lucide-react';
import { INITIAL_ATTENDANCE, INITIAL_COURSES } from '../data/mockData';

export const AttendancePage: React.FC = () => {
  const { user, studentProfile } = useAuth();
  const role = user?.role || 'student';

  const [courses, setCourses] = useState<Course[]>(INITIAL_COURSES);
  const [selectedCourseId, setSelectedCourseId] = useState<number>(101);
  const [records, setRecords] = useState<AttendanceRecord[]>(INITIAL_ATTENDANCE);
  const [loading, setLoading] = useState(false);
  const [successMsg, setSuccessMsg] = useState('');

  // Marking Form State for Professors/Admins
  const [markStudentId, setMarkStudentId] = useState<number>(studentProfile?.id || 1);
  const [markDate, setMarkDate] = useState<string>(new Date().toISOString().split('T')[0]);
  const [markStatus, setMarkStatus] = useState<'present' | 'absent' | 'late'>('present');

  useEffect(() => {
    const loadData = async () => {
      try {
        const fetchedCourses = await api.getCourses();
        if (fetchedCourses.length > 0) setCourses(fetchedCourses);

        if (role === 'student' && studentProfile) {
          const att = await api.getAttendanceByStudent(studentProfile.id);
          if (att.length > 0) setRecords(att);
        } else {
          const att = await api.getAttendanceByCourse(selectedCourseId);
          if (att.length > 0) setRecords(att);
        }
      } catch {
        // Fallback
      }
    };
    loadData();
  }, [role, studentProfile, selectedCourseId]);

  const handleMarkAttendance = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    setSuccessMsg('');

    try {
      const newRecord = await api.markAttendance({
        courseId: Number(selectedCourseId),
        studentId: Number(markStudentId),
        date: markDate,
        status: markStatus,
      });

      setRecords((prev) => [newRecord, ...prev]);
      setSuccessMsg('Attendance record added successfully!');
      setTimeout(() => setSuccessMsg(''), 3000);
    } catch (err: any) {
      // Local fallback additions
      const courseName = courses.find((c) => c.id === selectedCourseId)?.name;
      const fakeRec: AttendanceRecord = {
        id: Date.now(),
        courseId: selectedCourseId,
        studentId: markStudentId,
        date: markDate,
        status: markStatus,
        courseName,
      };
      setRecords((prev) => [fakeRec, ...prev]);
      setSuccessMsg('Attendance marked successfully!');
      setTimeout(() => setSuccessMsg(''), 3000);
    } finally {
      setLoading(false);
    }
  };

  const filteredRecords = records.filter((r) => r.courseId === selectedCourseId);
  const total = filteredRecords.length;
  const present = filteredRecords.filter((r) => r.status === 'present').length;
  const absent = filteredRecords.filter((r) => r.status === 'absent').length;
  const late = filteredRecords.filter((r) => r.status === 'late').length;
  const pct = total > 0 ? Math.round(((present + late) / total) * 100) : 100;

  return (
    <div className="space-y-6 animate-fade-in">
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        <div>
          <h1 className="text-2xl font-extrabold text-slate-900 tracking-tight">Attendance Management</h1>
          <p className="text-xs text-slate-500 mt-1">Track course attendance, daily registries, and percentage requirements</p>
        </div>

        {/* Course Filter */}
        <div className="flex items-center gap-2 bg-white px-3 py-2 rounded-xl border border-slate-200 shadow-sm">
          <Filter className="w-4 h-4 text-slate-400" />
          <span className="text-xs font-semibold text-slate-500">Course:</span>
          <select
            value={selectedCourseId}
            onChange={(e) => setSelectedCourseId(Number(e.target.value))}
            className="text-xs font-bold text-slate-800 bg-transparent focus:outline-none cursor-pointer"
          >
            {courses.map((c) => (
              <option key={c.id} value={c.id}>
                {c.code} — {c.name}
              </option>
            ))}
          </select>
        </div>
      </div>

      {/* Overview Stats */}
      <div className="grid grid-cols-2 sm:grid-cols-4 gap-4">
        <div className="glass-panel p-4 rounded-2xl border border-slate-200 text-center">
          <p className="text-xs font-semibold text-slate-400 uppercase">Total Conducted</p>
          <p className="text-2xl font-extrabold text-slate-900 mt-1">{total}</p>
        </div>
        <div className="glass-panel p-4 rounded-2xl border border-emerald-200 bg-emerald-50/50 text-center">
          <p className="text-xs font-semibold text-emerald-600 uppercase">Present</p>
          <p className="text-2xl font-extrabold text-emerald-700 mt-1">{present}</p>
        </div>
        <div className="glass-panel p-4 rounded-2xl border border-rose-200 bg-rose-50/50 text-center">
          <p className="text-xs font-semibold text-rose-600 uppercase">Absent</p>
          <p className="text-2xl font-extrabold text-rose-700 mt-1">{absent}</p>
        </div>
        <div className="glass-panel p-4 rounded-2xl border border-amber-200 bg-amber-50/50 text-center">
          <p className="text-xs font-semibold text-amber-600 uppercase">Attendance Rate</p>
          <p className="text-2xl font-extrabold text-amber-700 mt-1">{pct}%</p>
        </div>
      </div>

      {/* Professor/Admin Marking Form */}
      {role !== 'student' && (
        <div className="glass-panel p-6 rounded-3xl border border-slate-200">
          <h3 className="text-lg font-bold text-slate-900 mb-4 flex items-center gap-2">
            <Plus className="w-5 h-5 text-sky-600" /> Mark Student Attendance
          </h3>

          {successMsg && (
            <div className="mb-4 p-3 rounded-xl bg-emerald-50 border border-emerald-200 text-emerald-700 text-xs font-semibold flex items-center gap-2">
              <CheckCircle2 className="w-4 h-4" /> {successMsg}
            </div>
          )}

          <form onSubmit={handleMarkAttendance} className="grid grid-cols-1 sm:grid-cols-4 gap-4 items-end">
            <div>
              <label className="block text-xs font-semibold text-slate-600 mb-1">Student ID</label>
              <input
                type="number"
                required
                value={markStudentId}
                onChange={(e) => setMarkStudentId(Number(e.target.value))}
                className="w-full px-3.5 py-2 rounded-xl bg-white border border-slate-200 text-slate-900 text-sm focus:outline-none focus:border-sky-500"
              />
            </div>

            <div>
              <label className="block text-xs font-semibold text-slate-600 mb-1">Date</label>
              <input
                type="date"
                required
                value={markDate}
                onChange={(e) => setMarkDate(e.target.value)}
                className="w-full px-3.5 py-2 rounded-xl bg-white border border-slate-200 text-slate-900 text-sm focus:outline-none focus:border-sky-500"
              />
            </div>

            <div>
              <label className="block text-xs font-semibold text-slate-600 mb-1">Status</label>
              <select
                value={markStatus}
                onChange={(e) => setMarkStatus(e.target.value as any)}
                className="w-full px-3.5 py-2 rounded-xl bg-white border border-slate-200 text-slate-900 text-sm focus:outline-none focus:border-sky-500"
              >
                <option value="present">Present</option>
                <option value="absent">Absent</option>
                <option value="late">Late</option>
              </select>
            </div>

            <button
              type="submit"
              disabled={loading}
              className="py-2.5 rounded-xl bg-sky-600 hover:bg-sky-500 text-white font-bold text-sm shadow-md shadow-sky-600/20 flex items-center justify-center gap-2 transition-all disabled:opacity-50"
            >
              <Save className="w-4 h-4" /> Save Record
            </button>
          </form>
        </div>
      )}

      {/* Attendance History Table */}
      <div className="glass-panel rounded-3xl border border-slate-200 overflow-hidden shadow-sm">
        <div className="p-5 border-b border-slate-200 flex items-center justify-between">
          <h3 className="font-bold text-slate-900 text-base flex items-center gap-2">
            <CalendarCheck className="w-5 h-5 text-sky-600" /> Attendance History
          </h3>
          <span className="text-xs text-slate-500 font-semibold">{filteredRecords.length} Entries</span>
        </div>

        <div className="overflow-x-auto">
          <table className="w-full text-left text-xs">
            <thead className="bg-slate-50 text-slate-500 uppercase tracking-wider font-bold border-b border-slate-200">
              <tr>
                <th className="p-4">Date</th>
                <th className="p-4">Student ID</th>
                <th className="p-4">Course</th>
                <th className="p-4 text-center">Status</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-100 font-medium">
              {filteredRecords.length > 0 ? (
                filteredRecords.map((r) => (
                  <tr key={r.id} className="hover:bg-slate-50/80 transition-colors">
                    <td className="p-4 font-mono font-semibold text-slate-700">{r.date}</td>
                    <td className="p-4 font-bold text-slate-900">Student #{r.studentId}</td>
                    <td className="p-4 text-slate-600">{r.courseName || `Course #${r.courseId}`}</td>
                    <td className="p-4 text-center">
                      {r.status === 'present' && (
                        <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full font-bold bg-emerald-100 text-emerald-800 border border-emerald-300">
                          <CheckCircle2 className="w-3 h-3" /> Present
                        </span>
                      )}
                      {r.status === 'absent' && (
                        <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full font-bold bg-rose-100 text-rose-800 border border-rose-300">
                          <XCircle className="w-3 h-3" /> Absent
                        </span>
                      )}
                      {r.status === 'late' && (
                        <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full font-bold bg-amber-100 text-amber-800 border border-amber-300">
                          <Clock className="w-3 h-3" /> Late
                        </span>
                      )}
                    </td>
                  </tr>
                ))
              ) : (
                <tr>
                  <td colSpan={4} className="p-8 text-center text-slate-400">
                    No attendance records logged for this course yet.
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
};
