import React, { useState, useContext } from 'react';
import { AuthContext } from '../context/AuthContext';
import { AttendanceRecord } from '../types';

export const AttendancePage: React.FC = () => {
  const auth = useContext(AuthContext);
  const user = auth?.user;

  const [logs, setLogs] = useState<AttendanceRecord[]>([
    { id: 1, date: '2026-08-01', course: 'CSE-101 Data Structures & Algorithms', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' },
    { id: 2, date: '2026-07-29', course: 'CSE-202 Database Management Systems', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' },
    { id: 3, date: '2026-07-28', course: 'MAT-301 Applied Linear Algebra', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' }
  ]);

  const [showLogModal, setShowLogModal] = useState(false);
  const [logCourse, setLogCourse] = useState('CSE-101 Data Structures');
  const [logStudent, setLogStudent] = useState('Alex Johnson');
  const [logStatus, setLogStatus] = useState<'Present' | 'Absent'>('Present');

  const handleMarkAttendance = (e: React.FormEvent) => {
    e.preventDefault();
    const newEntry: AttendanceRecord = {
      id: Date.now(),
      date: new Date().toISOString().split('T')[0],
      course: logCourse,
      student: logStudent,
      status: logStatus
    };
    setLogs([newEntry, ...logs]);
    setShowLogModal(false);
  };

  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h2 className="text-2xl font-black text-white">Attendance Management</h2>
          <p className="text-xs text-slate-400 font-medium">Verified attendance records and compliance registry</p>
        </div>
        {(user?.role === 'professor' || user?.role === 'admin') && (
          <button
            onClick={() => setShowLogModal(true)}
            className="px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors"
          >
            + Record Attendance
          </button>
        )}
      </div>

      <div className="bg-slate-900 rounded-2xl border border-slate-800 shadow-sm overflow-hidden">
        <table className="w-full text-left border-collapse text-xs">
          <thead className="bg-slate-950 text-slate-300 font-bold uppercase">
            <tr>
              <th className="p-4">Date</th>
              <th className="p-4">Course Code & Name</th>
              <th className="p-4">Student Identity</th>
              <th className="p-4">Attendance Status</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-slate-800 font-medium text-slate-300">
            {logs.map(row => (
              <tr key={row.id} className="hover:bg-slate-800/50 transition-colors">
                <td className="p-4 font-bold text-white">{row.date}</td>
                <td className="p-4 font-bold text-slate-200">{row.course}</td>
                <td className="p-4 text-slate-400">{row.student}</td>
                <td className="p-4">
                  <span
                    className={`px-3 py-1 rounded-full text-[11px] font-bold border ${
                      row.status === 'Present'
                        ? 'bg-emerald-500/20 text-emerald-400 border-emerald-500/30'
                        : 'bg-rose-500/20 text-rose-400 border-rose-500/30'
                    }`}
                  >
                    {row.status}
                  </span>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      {showLogModal && (
        <div className="fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4">
          <div className="bg-slate-900 border border-slate-800 p-6 rounded-3xl max-w-md w-full space-y-4 shadow-2xl">
            <div className="flex justify-between items-center">
              <h3 className="text-lg font-bold text-white">Record Student Attendance Log</h3>
              <button onClick={() => setShowLogModal(false)} className="text-slate-400 hover:text-white font-bold">
                ✕
              </button>
            </div>
            <form onSubmit={handleMarkAttendance} className="space-y-3 text-xs">
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Course</label>
                <input
                  type="text"
                  value={logCourse}
                  onChange={(e) => setLogCourse(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                />
              </div>
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Student Identity</label>
                <input
                  type="text"
                  value={logStudent}
                  onChange={(e) => setLogStudent(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                />
              </div>
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Attendance Status</label>
                <select
                  value={logStatus}
                  onChange={(e) => setLogStatus(e.target.value as 'Present' | 'Absent')}
                  className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                >
                  <option value="Present">Present</option>
                  <option value="Absent">Absent</option>
                </select>
              </div>
              <div className="flex justify-end gap-2 pt-2">
                <button type="button" onClick={() => setShowLogModal(false)} className="px-4 py-2 bg-slate-800 text-slate-300 rounded-xl font-bold">
                  Cancel
                </button>
                <button type="submit" className="px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white rounded-xl font-bold">
                  Record Log
                </button>
              </div>
            </form>
          </div>
        </div>
      )}
    </div>
  );
};
