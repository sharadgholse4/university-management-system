import React, { useState, useEffect, useContext } from 'react';
import { AuthContext, apiCall } from '../context/AuthContext';
import { Course } from '../types';

export const CoursesPage: React.FC = () => {
  const auth = useContext(AuthContext);
  const user = auth?.user;

  const [courses, setCourses] = useState<Course[]>([]);
  const [searchQuery, setSearchQuery] = useState('');
  const [selectedDept, setSelectedDept] = useState('All');
  const [showAddModal, setShowAddModal] = useState(false);

  const [newCode, setNewCode] = useState('');
  const [newName, setNewName] = useState('');
  const [newDept, setNewDept] = useState('Computer Science');
  const [newCredits, setNewCredits] = useState(4);
  const [newInstructor, setNewInstructor] = useState(user?.name || 'Dr. Robert Smith');

  useEffect(() => {
    const fetchCourses = async () => {
      const res = await apiCall('/courses');
      if (res && res.success && Array.isArray(res.data)) {
        setCourses(res.data);
      } else {
        const saved = localStorage.getItem('eduportal_courses');
        if (saved) {
          try { setCourses(JSON.parse(saved)); return; } catch(e){}
        }
        const initial: Course[] = [
          { id: 1, code: 'CSE-101', name: 'Data Structures & Algorithms', department: 'Computer Science', credits: 4, semester: 4, instructor: 'Dr. Robert Smith', enrolled: 48 },
          { id: 2, code: 'CSE-202', name: 'Relational Database Management Systems', department: 'Computer Science', credits: 4, semester: 4, instructor: 'Dr. Robert Smith', enrolled: 42 },
          { id: 3, code: 'EEE-105', name: 'Microprocessor Systems & Architecture', department: 'Electrical Engineering', credits: 3, semester: 2, instructor: 'Prof. Alan Turing', enrolled: 35 },
          { id: 4, code: 'MAT-301', name: 'Applied Linear Algebra & Statistics', department: 'Mathematics', credits: 3, semester: 3, instructor: 'Dr. Ada Lovelace', enrolled: 55 }
        ];
        setCourses(initial);
        localStorage.setItem('eduportal_courses', JSON.stringify(initial));
      }
    };
    fetchCourses();
  }, []);

  const handleAddCourse = (e: React.FormEvent) => {
    e.preventDefault();
    if (!newCode || !newName) return;

    const courseObj: Course = {
      id: Date.now(),
      code: newCode.toUpperCase(),
      name: newName,
      department: newDept,
      credits: parseInt(newCredits.toString(), 10),
      semester: 4,
      instructor: newInstructor,
      enrolled: 1
    };

    const updated = [courseObj, ...courses];
    setCourses(updated);
    localStorage.setItem('eduportal_courses', JSON.stringify(updated));
    apiCall('/courses', 'POST', courseObj).catch(() => {});

    setNewCode('');
    setNewName('');
    setShowAddModal(false);
  };

  const filteredCourses = courses.filter(c => {
    const matchesSearch = c.name.toLowerCase().includes(searchQuery.toLowerCase()) || c.code.toLowerCase().includes(searchQuery.toLowerCase());
    const matchesDept = selectedDept === 'All' || c.department === selectedDept;
    return matchesSearch && matchesDept;
  });

  return (
    <div className="space-y-6">
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        <div>
          <h2 className="text-2xl font-black text-white">Academic Course Catalog</h2>
          <p className="text-xs text-slate-400 font-medium">Official curriculum and course offerings across university departments</p>
        </div>
        {(user?.role === 'professor' || user?.role === 'admin') && (
          <button
            onClick={() => setShowAddModal(true)}
            className="px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors flex items-center gap-2 shrink-0"
          >
            + Add New Course
          </button>
        )}
      </div>

      <div className="flex flex-col sm:flex-row gap-3 bg-slate-900 p-3 rounded-2xl border border-slate-800">
        <input
          type="text"
          value={searchQuery}
          placeholder="🔍 Search by course name or code (e.g. CSE-101)..."
          onChange={(e) => setSearchQuery(e.target.value)}
          className="flex-1 px-4 py-2 bg-slate-800 border border-slate-700 text-white text-xs rounded-xl focus:outline-none focus:border-indigo-500"
        />
        <select
          value={selectedDept}
          onChange={(e) => setSelectedDept(e.target.value)}
          className="px-4 py-2 bg-slate-800 border border-slate-700 text-white text-xs rounded-xl focus:outline-none focus:border-indigo-500"
        >
          <option value="All">All Departments</option>
          <option value="Computer Science">Computer Science</option>
          <option value="Electrical Engineering">Electrical Engineering</option>
          <option value="Mathematics">Mathematics</option>
        </select>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        {filteredCourses.length > 0 ? (
          filteredCourses.map(course => (
            <div key={course.id} className="bg-slate-900 p-5 rounded-2xl border border-slate-800 shadow-sm space-y-3">
              <div className="flex items-center justify-between">
                <span className="px-2.5 py-1 rounded-md bg-indigo-500/20 text-indigo-300 font-bold text-xs border border-indigo-500/30">
                  {course.code}
                </span>
                <span className="text-xs font-bold text-slate-400">
                  {`${course.credits} Credits • Semester ${course.semester}`}
                </span>
              </div>
              <h3 className="text-base font-bold text-white">{course.name}</h3>
              <div className="flex items-center justify-between text-xs text-slate-400 pt-3 border-t border-slate-800 font-medium">
                <span>{`Faculty: ${course.instructor}`}</span>
                <span className="font-bold text-emerald-400">{`${course.enrolled} Enrolled`}</span>
              </div>
            </div>
          ))
        ) : (
          <div className="col-span-2 text-center p-8 bg-slate-900 rounded-2xl border border-slate-800 text-slate-400 text-xs font-medium">
            No courses found matching your query.
          </div>
        )}
      </div>

      {showAddModal && (
        <div className="fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4">
          <div className="bg-slate-900 border border-slate-800 p-6 rounded-3xl max-w-md w-full space-y-4 shadow-2xl">
            <div className="flex justify-between items-center">
              <h3 className="text-lg font-bold text-white">Create New Course Offering</h3>
              <button onClick={() => setShowAddModal(false)} className="text-slate-400 hover:text-white font-bold">
                ✕
              </button>
            </div>
            <form onSubmit={handleAddCourse} className="space-y-3 text-xs">
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Course Code</label>
                <input
                  type="text"
                  value={newCode}
                  placeholder="e.g. CSE-305"
                  onChange={(e) => setNewCode(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                />
              </div>
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Course Title</label>
                <input
                  type="text"
                  value={newName}
                  placeholder="e.g. Operating Systems & Kernel Architecture"
                  onChange={(e) => setNewName(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                />
              </div>
              <div className="grid grid-cols-2 gap-3">
                <div className="space-y-1">
                  <label className="font-bold text-slate-300">Department</label>
                  <select
                    value={newDept}
                    onChange={(e) => setNewDept(e.target.value)}
                    className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                  >
                    <option value="Computer Science">Computer Science</option>
                    <option value="Electrical Engineering">Electrical Engineering</option>
                    <option value="Mathematics">Mathematics</option>
                  </select>
                </div>
                <div className="space-y-1">
                  <label className="font-bold text-slate-300">Credit Hours</label>
                  <input
                    type="number"
                    value={newCredits}
                    min={1}
                    max={6}
                    onChange={(e) => setNewCredits(parseInt(e.target.value) || 1)}
                    className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                  />
                </div>
              </div>
              <div className="flex justify-end gap-2 pt-2">
                <button type="button" onClick={() => setShowAddModal(false)} className="px-4 py-2 bg-slate-800 text-slate-300 rounded-xl font-bold">
                  Cancel
                </button>
                <button type="submit" className="px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white rounded-xl font-bold">
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
