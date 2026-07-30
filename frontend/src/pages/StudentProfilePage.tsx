import React, { useState, useEffect } from 'react';
import { useAuth } from '../context/AuthContext';
import { api } from '../services/api';
import { Student, Department } from '../types';
import { User, Award, BookOpen, Save, CheckCircle2, Shield } from 'lucide-react';
import { INITIAL_STUDENT_PROFILE, INITIAL_DEPARTMENTS } from '../data/mockData';

export const StudentProfilePage: React.FC = () => {
  const { user, studentProfile } = useAuth();
  const [profile, setProfile] = useState<Student>(studentProfile || INITIAL_STUDENT_PROFILE);
  const [departments, setDepartments] = useState<Department[]>(INITIAL_DEPARTMENTS);
  const [loading, setLoading] = useState(false);
  const [successMsg, setSuccessMsg] = useState('');
  const [errorMsg, setErrorMsg] = useState('');

  useEffect(() => {
    const loadDepts = async () => {
      try {
        const depts = await api.getDepartments();
        if (depts.length > 0) setDepartments(depts);
      } catch {
        // Fallback to initial
      }
    };
    loadDepts();
  }, []);

  const handleSave = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    setSuccessMsg('');
    setErrorMsg('');

    try {
      if (profile.id) {
        const updated = await api.updateStudent(profile.id, {
          firstName: profile.firstName,
          lastName: profile.lastName,
          rollNumber: profile.rollNumber,
          semester: Number(profile.semester),
        });
        setProfile(updated);
      }
      setSuccessMsg('Student profile updated successfully!');
      setTimeout(() => setSuccessMsg(''), 3000);
    } catch (err: any) {
      setErrorMsg(err.message || 'Failed to update profile.');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="space-y-6 animate-fade-in">
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-2xl font-extrabold text-slate-900 tracking-tight">Student Profile</h1>
          <p className="text-xs text-slate-500 mt-1">Manage academic identity, department affiliation, and semester standing</p>
        </div>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Profile Card */}
        <div className="glass-panel p-6 rounded-3xl border border-slate-200 text-center flex flex-col items-center justify-center">
          <div className="w-24 h-24 rounded-full bg-gradient-to-tr from-sky-500 to-indigo-600 flex items-center justify-center text-white text-3xl font-extrabold shadow-xl shadow-sky-500/20 mb-4">
            {profile.firstName[0]}
            {profile.lastName[0]}
          </div>
          <h2 className="text-xl font-bold text-slate-900">
            {profile.firstName} {profile.lastName}
          </h2>
          <p className="text-xs font-mono font-semibold text-sky-600 bg-sky-50 px-3 py-1 rounded-full border border-sky-200 mt-2">
            Roll: {profile.rollNumber}
          </p>

          <div className="w-full mt-6 pt-6 border-t border-slate-200 space-y-3 text-left">
            <div className="flex justify-between items-center text-xs">
              <span className="text-slate-500">Department</span>
              <span className="font-bold text-slate-800">
                {departments.find((d) => d.id === profile.departmentId)?.name || 'Computer Science'}
              </span>
            </div>
            <div className="flex justify-between items-center text-xs">
              <span className="text-slate-500">Academic Standing</span>
              <span className="font-bold text-slate-800">Semester {profile.semester}</span>
            </div>
            <div className="flex justify-between items-center text-xs">
              <span className="text-slate-500">User Email</span>
              <span className="font-bold text-slate-800">{user?.email}</span>
            </div>
          </div>
        </div>

        {/* Profile Edit Form */}
        <div className="lg:col-span-2 glass-panel p-6 rounded-3xl border border-slate-200">
          <h3 className="text-lg font-bold text-slate-900 mb-4 flex items-center gap-2">
            <User className="w-5 h-5 text-sky-600" /> Edit Profile Details
          </h3>

          {successMsg && (
            <div className="mb-4 p-3 rounded-xl bg-emerald-50 border border-emerald-200 text-emerald-700 text-xs font-semibold flex items-center gap-2">
              <CheckCircle2 className="w-4 h-4" /> {successMsg}
            </div>
          )}

          {errorMsg && (
            <div className="mb-4 p-3 rounded-xl bg-rose-50 border border-rose-200 text-rose-700 text-xs font-semibold">
              {errorMsg}
            </div>
          )}

          <form onSubmit={handleSave} className="space-y-4">
            <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
              <div>
                <label className="block text-xs font-semibold text-slate-600 mb-1">First Name</label>
                <input
                  type="text"
                  required
                  value={profile.firstName}
                  onChange={(e) => setProfile({ ...profile, firstName: e.target.value })}
                  className="w-full px-3.5 py-2.5 rounded-xl bg-white border border-slate-200 text-slate-900 text-sm focus:outline-none focus:border-sky-500 focus:ring-1 focus:ring-sky-500"
                />
              </div>

              <div>
                <label className="block text-xs font-semibold text-slate-600 mb-1">Last Name</label>
                <input
                  type="text"
                  required
                  value={profile.lastName}
                  onChange={(e) => setProfile({ ...profile, lastName: e.target.value })}
                  className="w-full px-3.5 py-2.5 rounded-xl bg-white border border-slate-200 text-slate-900 text-sm focus:outline-none focus:border-sky-500 focus:ring-1 focus:ring-sky-500"
                />
              </div>
            </div>

            <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
              <div>
                <label className="block text-xs font-semibold text-slate-600 mb-1">Roll Number / ID</label>
                <input
                  type="text"
                  required
                  value={profile.rollNumber}
                  onChange={(e) => setProfile({ ...profile, rollNumber: e.target.value })}
                  className="w-full px-3.5 py-2.5 rounded-xl bg-white border border-slate-200 text-slate-900 text-sm focus:outline-none focus:border-sky-500 focus:ring-1 focus:ring-sky-500"
                />
              </div>

              <div>
                <label className="block text-xs font-semibold text-slate-600 mb-1">Semester</label>
                <select
                  value={profile.semester}
                  onChange={(e) => setProfile({ ...profile, semester: Number(e.target.value) })}
                  className="w-full px-3.5 py-2.5 rounded-xl bg-white border border-slate-200 text-slate-900 text-sm focus:outline-none focus:border-sky-500"
                >
                  {[1, 2, 3, 4, 5, 6, 7, 8].map((s) => (
                    <option key={s} value={s}>
                      Semester {s}
                    </option>
                  ))}
                </select>
              </div>
            </div>

            <div>
              <label className="block text-xs font-semibold text-slate-600 mb-1">Department</label>
              <select
                value={profile.departmentId}
                onChange={(e) => setProfile({ ...profile, departmentId: Number(e.target.value) })}
                className="w-full px-3.5 py-2.5 rounded-xl bg-white border border-slate-200 text-slate-900 text-sm focus:outline-none focus:border-sky-500"
              >
                {departments.map((d) => (
                  <option key={d.id} value={d.id}>
                    {d.name} ({d.code})
                  </option>
                ))}
              </select>
            </div>

            <div className="pt-2">
              <button
                type="submit"
                disabled={loading}
                className="px-6 py-2.5 rounded-xl bg-sky-600 hover:bg-sky-500 text-white font-bold text-sm shadow-md shadow-sky-600/20 flex items-center gap-2 transition-all disabled:opacity-50"
              >
                <Save className="w-4 h-4" />
                {loading ? 'Saving...' : 'Save Profile Changes'}
              </button>
            </div>
          </form>
        </div>
      </div>
    </div>
  );
};
