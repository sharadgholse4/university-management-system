import React, { useState, useEffect } from 'react';
import { useAuth } from '../context/AuthContext';
import { api } from '../services/api';
import { Course, Department } from '../types';
import { BookOpen, Building2, Plus, Filter, CheckCircle2, Shield } from 'lucide-react';
import { INITIAL_COURSES, INITIAL_DEPARTMENTS } from '../data/mockData';

export const CoursesPage: React.FC = () => {
  const { user } = useAuth();
  const role = user?.role || 'student';

  const [courses, setCourses] = useState<Course[]>(INITIAL_COURSES);
  const [departments, setDepartments] = useState<Department[]>(INITIAL_DEPARTMENTS);
  const [selectedSem, setSelectedSem] = useState<number | 'all'>('all');
  const [showDeptModal, setShowDeptModal] = useState(false);
  const [showCourseModal, setShowCourseModal] = useState(false);

  // New Dept Form
  const [deptName, setDeptName] = useState('');
  const [deptCode, setDeptCode] = useState('');

  // New Course Form
  const [courseCode, setCourseCode] = useState('');
  const [courseName, setCourseName] = useState('');
  const [courseDeptId, setCourseDeptId] = useState<number>(1);
  const [courseCredits, setCourseCredits] = useState<number>(3);
  const [courseSem, setCourseSem] = useState<number>(1);

  const [successMsg, setSuccessMsg] = useState('');

  useEffect(() => {
    const loadData = async () => {
      try {
        const fetchedDepts = await api.getDepartments();
        if (fetchedDepts.length > 0) setDepartments(fetchedDepts);

        const fetchedCourses = await api.getCourses();
        if (fetchedCourses.length > 0) setCourses(fetchedCourses);
      } catch {
        // Fallback
      }
    };
    loadData();
  }, []);

  const handleCreateDept = async (e: React.FormEvent) => {
    e.preventDefault();
    try {
      const newDept = await api.createDepartment(deptName, deptCode);
      setDepartments((prev) => [...prev, newDept]);
      setSuccessMsg('Department created successfully!');
      setShowDeptModal(false);
      setDeptName('');
      setDeptCode('');
      setTimeout(() => setSuccessMsg(''), 3000);
    } catch {
      const fakeDept: Department = { id: Date.now(), name: deptName, code: deptCode };
      setDepartments((prev) => [...prev, fakeDept]);
      setSuccessMsg('Department added!');
      setShowDeptModal(false);
      setTimeout(() => setSuccessMsg(''), 3000);
    }
  };

  const handleCreateCourse = async (e: React.FormEvent) => {
    e.preventDefault();
    try {
      const newCourse = await api.createCourse({
        code: courseCode,
        name: courseName,
        departmentId: courseDeptId,
        professorId: 2,
        credits: courseCredits,
        semester: courseSem,
      });
      setCourses((prev) => [...prev, newCourse]);
      setSuccessMsg('Course created successfully!');
      setShowCourseModal(false);
      setCourseCode('');
      setCourseName('');
      setTimeout(() => setSuccessMsg(''), 3000);
    } catch {
      const fakeCourse: Course = {
        id: Date.now(),
        code: courseCode,
        name: courseName,
        departmentId: courseDeptId,
        professorId: 2,
        credits: courseCredits,
        semester: courseSem,
        departmentName: departments.find((d) => d.id === courseDeptId)?.name,
      };
      setCourses((prev) => [...prev, fakeCourse]);
      setSuccessMsg('Course added to catalog!');
      setShowCourseModal(false);
      setTimeout(() => setSuccessMsg(''), 3000);
    }
  };

  const filteredCourses = selectedSem === 'all' ? courses : courses.filter((c) => c.semester === selectedSem);

  return (
    <div className="space-y-6 animate-fade-in">
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        <div>
          <h1 className="text-2xl font-extrabold text-slate-900 tracking-tight">Courses & Departments</h1>
          <p className="text-xs text-slate-500 mt-1">Academic curriculum catalog, department structures, and semester courses</p>
        </div>

        {role === 'admin' && (
          <div className="flex items-center gap-2">
            <button
              onClick={() => setShowDeptModal(true)}
              className="px-3.5 py-2 rounded-xl bg-slate-800 hover:bg-slate-700 text-white font-bold text-xs shadow-sm flex items-center gap-1.5"
            >
              <Building2 className="w-4 h-4 text-amber-400" /> + Dept
            </button>
            <button
              onClick={() => setShowCourseModal(true)}
              className="px-3.5 py-2 rounded-xl bg-sky-600 hover:bg-sky-500 text-white font-bold text-xs shadow-md shadow-sky-600/20 flex items-center gap-1.5"
            >
              <Plus className="w-4 h-4" /> + Course
            </button>
          </div>
        )}
      </div>

      {successMsg && (
        <div className="p-3 rounded-xl bg-emerald-50 border border-emerald-200 text-emerald-700 text-xs font-semibold flex items-center gap-2">
          <CheckCircle2 className="w-4 h-4" /> {successMsg}
        </div>
      )}

      {/* Departments Grid */}
      <div>
        <h3 className="text-sm font-bold text-slate-500 uppercase tracking-wider mb-3">Academic Departments</h3>
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
          {departments.map((d) => (
            <div key={d.id} className="glass-panel p-4 rounded-2xl border border-slate-200 hover:border-sky-300 transition-all">
              <div className="flex items-center gap-3">
                <div className="w-10 h-10 rounded-xl bg-sky-50 text-sky-600 border border-sky-200 flex items-center justify-center font-bold text-xs">
                  {d.code}
                </div>
                <div>
                  <h4 className="font-bold text-slate-900 text-xs">{d.name}</h4>
                  <p className="text-[11px] text-slate-500 font-mono mt-0.5">Dept Code: {d.code}</p>
                </div>
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Course Catalog Filter */}
      <div>
        <div className="flex items-center justify-between mb-3">
          <h3 className="text-sm font-bold text-slate-500 uppercase tracking-wider">Course Catalog</h3>
          <div className="flex items-center gap-1.5 overflow-x-auto py-1">
            <button
              onClick={() => setSelectedSem('all')}
              className={`px-3 py-1 rounded-lg text-xs font-bold transition-all ${
                selectedSem === 'all' ? 'bg-sky-600 text-white shadow-sm' : 'bg-white text-slate-600 border border-slate-200'
              }`}
            >
              All Semesters
            </button>
            {[1, 2, 3, 4, 5, 6, 7, 8].map((s) => (
              <button
                key={s}
                onClick={() => setSelectedSem(s)}
                className={`px-2.5 py-1 rounded-lg text-xs font-bold transition-all ${
                  selectedSem === s ? 'bg-sky-600 text-white shadow-sm' : 'bg-white text-slate-600 border border-slate-200'
                }`}
              >
                Sem {s}
              </button>
            ))}
          </div>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
          {filteredCourses.map((c) => (
            <div key={c.id} className="glass-panel p-5 rounded-2xl border border-slate-200 hover:border-sky-300 transition-all flex flex-col justify-between">
              <div>
                <div className="flex items-center justify-between mb-2">
                  <span className="px-2.5 py-0.5 rounded-full text-xs font-mono font-bold bg-sky-100 text-sky-800 border border-sky-200">
                    {c.code}
                  </span>
                  <span className="text-xs font-semibold text-slate-500">Sem {c.semester}</span>
                </div>
                <h4 className="font-bold text-slate-900 text-base">{c.name}</h4>
                <p className="text-xs text-slate-500 mt-1">
                  {c.departmentName || departments.find((d) => d.id === c.departmentId)?.name || 'Engineering'}
                </p>
              </div>

              <div className="mt-4 pt-3 border-t border-slate-100 flex items-center justify-between text-xs">
                <span className="font-bold text-slate-700">{c.credits} Credit Hours</span>
                <span className="text-sky-600 font-semibold flex items-center gap-1">
                  <BookOpen className="w-3.5 h-3.5" /> Syllabus Active
                </span>
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* Modal Add Dept */}
      {showDeptModal && (
        <div className="fixed inset-0 bg-slate-900/60 backdrop-blur-sm z-50 flex items-center justify-center p-4">
          <div className="bg-white rounded-3xl p-6 w-full max-w-md shadow-2xl border border-slate-200">
            <h3 className="text-lg font-bold text-slate-900 mb-4 flex items-center gap-2">
              <Building2 className="w-5 h-5 text-sky-600" /> Create Department
            </h3>
            <form onSubmit={handleCreateDept} className="space-y-4">
              <div>
                <label className="block text-xs font-semibold text-slate-600 mb-1">Department Name</label>
                <input
                  type="text"
                  required
                  placeholder="e.g. Civil Engineering"
                  value={deptName}
                  onChange={(e) => setDeptName(e.target.value)}
                  className="w-full px-3.5 py-2 rounded-xl bg-slate-50 border border-slate-200 text-sm"
                />
              </div>
              <div>
                <label className="block text-xs font-semibold text-slate-600 mb-1">Department Code</label>
                <input
                  type="text"
                  required
                  placeholder="e.g. CE"
                  value={deptCode}
                  onChange={(e) => setDeptCode(e.target.value)}
                  className="w-full px-3.5 py-2 rounded-xl bg-slate-50 border border-slate-200 text-sm"
                />
              </div>
              <div className="flex gap-2 pt-2">
                <button
                  type="button"
                  onClick={() => setShowDeptModal(false)}
                  className="flex-1 py-2 rounded-xl bg-slate-100 hover:bg-slate-200 text-slate-700 font-bold text-xs"
                >
                  Cancel
                </button>
                <button
                  type="submit"
                  className="flex-1 py-2 rounded-xl bg-sky-600 hover:bg-sky-500 text-white font-bold text-xs shadow-md"
                >
                  Save Department
                </button>
              </div>
            </form>
          </div>
        </div>
      )}

      {/* Modal Add Course */}
      {showCourseModal && (
        <div className="fixed inset-0 bg-slate-900/60 backdrop-blur-sm z-50 flex items-center justify-center p-4">
          <div className="bg-white rounded-3xl p-6 w-full max-w-md shadow-2xl border border-slate-200">
            <h3 className="text-lg font-bold text-slate-900 mb-4 flex items-center gap-2">
              <Plus className="w-5 h-5 text-sky-600" /> Create New Course
            </h3>
            <form onSubmit={handleCreateCourse} className="space-y-3 text-xs">
              <div>
                <label className="block font-semibold text-slate-600 mb-1">Course Code</label>
                <input
                  type="text"
                  required
                  placeholder="e.g. CS301"
                  value={courseCode}
                  onChange={(e) => setCourseCode(e.target.value)}
                  className="w-full px-3 py-2 rounded-xl bg-slate-50 border border-slate-200"
                />
              </div>
              <div>
                <label className="block font-semibold text-slate-600 mb-1">Course Name</label>
                <input
                  type="text"
                  required
                  placeholder="e.g. Artificial Intelligence"
                  value={courseName}
                  onChange={(e) => setCourseName(e.target.value)}
                  className="w-full px-3 py-2 rounded-xl bg-slate-50 border border-slate-200"
                />
              </div>
              <div className="grid grid-cols-2 gap-2">
                <div>
                  <label className="block font-semibold text-slate-600 mb-1">Department</label>
                  <select
                    value={courseDeptId}
                    onChange={(e) => setCourseDeptId(Number(e.target.value))}
                    className="w-full px-3 py-2 rounded-xl bg-slate-50 border border-slate-200"
                  >
                    {departments.map((d) => (
                      <option key={d.id} value={d.id}>
                        {d.name}
                      </option>
                    ))}
                  </select>
                </div>
                <div>
                  <label className="block font-semibold text-slate-600 mb-1">Semester</label>
                  <select
                    value={courseSem}
                    onChange={(e) => setCourseSem(Number(e.target.value))}
                    className="w-full px-3 py-2 rounded-xl bg-slate-50 border border-slate-200"
                  >
                    {[1, 2, 3, 4, 5, 6, 7, 8].map((s) => (
                      <option key={s} value={s}>
                        Sem {s}
                      </option>
                    ))}
                  </select>
                </div>
              </div>
              <div>
                <label className="block font-semibold text-slate-600 mb-1">Credits (1-6)</label>
                <input
                  type="number"
                  min={1}
                  max={6}
                  value={courseCredits}
                  onChange={(e) => setCourseCredits(Number(e.target.value))}
                  className="w-full px-3 py-2 rounded-xl bg-slate-50 border border-slate-200"
                />
              </div>
              <div className="flex gap-2 pt-3">
                <button
                  type="button"
                  onClick={() => setShowCourseModal(false)}
                  className="flex-1 py-2 rounded-xl bg-slate-100 hover:bg-slate-200 text-slate-700 font-bold"
                >
                  Cancel
                </button>
                <button
                  type="submit"
                  className="flex-1 py-2 rounded-xl bg-sky-600 hover:bg-sky-500 text-white font-bold shadow-md"
                >
                  Save Course
                </button>
              </div>
            </form>
          </div>
        </div>
      )}
    </div>
  );
};
