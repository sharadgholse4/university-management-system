import React, { useState } from 'react';

interface EnrollmentCourse {
  id: number;
  code: string;
  name: string;
  credits: number;
  instructor: string;
}

export const EnrollmentPage: React.FC = () => {
  const [enrolled, setEnrolled] = useState<number[]>([1, 2]);

  const toggleEnroll = (id: number) => {
    if (enrolled.includes(id)) setEnrolled(enrolled.filter(i => i !== id));
    else setEnrolled([...enrolled, id]);
  };

  const courses: EnrollmentCourse[] = [
    { id: 1, code: 'CSE-101', name: 'Data Structures & Algorithms', credits: 4, instructor: 'Dr. Robert Smith' },
    { id: 2, code: 'CSE-202', name: 'Relational Database Systems', credits: 4, instructor: 'Dr. Robert Smith' },
    { id: 3, code: 'EEE-105', name: 'Microprocessor Architecture', credits: 3, instructor: 'Prof. Alan Turing' },
    { id: 4, code: 'MAT-301', name: 'Applied Linear Algebra', credits: 3, instructor: 'Dr. Ada Lovelace' }
  ];

  return (
    <div className="space-y-6">
      <div>
        <h2 className="text-2xl font-black text-white">Course Enrollment Portal</h2>
        <p className="text-xs text-slate-400 font-medium">Official course registration portal for Fall Semester 2026</p>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        {courses.map(course => {
          const isEnrolled = enrolled.includes(course.id);
          return (
            <div
              key={course.id}
              className={`bg-slate-900 p-5 rounded-2xl border transition-all ${
                isEnrolled ? 'border-indigo-500 shadow-lg shadow-indigo-500/10' : 'border-slate-800'
              }`}
            >
              <div className="flex items-center justify-between mb-3">
                <span className="px-2.5 py-1 rounded bg-slate-800 text-slate-200 font-bold text-xs">{course.code}</span>
                <span className="text-xs font-bold text-slate-400">{`${course.credits} Credits`}</span>
              </div>
              <h3 className="font-bold text-white text-base mb-1">{course.name}</h3>
              <p className="text-xs text-slate-400 mb-4">{`Faculty: ${course.instructor}`}</p>
              <button
                onClick={() => toggleEnroll(course.id)}
                className={`w-full py-2.5 rounded-xl font-bold text-xs transition-all uppercase tracking-wider ${
                  isEnrolled
                    ? 'bg-rose-500/20 text-rose-300 hover:bg-rose-500/30 border border-rose-500/30'
                    : 'bg-indigo-600 text-white hover:bg-indigo-500 shadow-md'
                }`}
              >
                {isEnrolled ? 'Drop Registration' : 'Register Course'}
              </button>
            </div>
          );
        })}
      </div>
    </div>
  );
};
