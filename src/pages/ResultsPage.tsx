import React from 'react';
import { TranscriptResult } from '../types';

export const ResultsPage: React.FC = () => {
  const results: TranscriptResult[] = [
    { id: 1, course: 'CSE-101 Data Structures & Algorithms', exam: 'Mid-Semester Exam', score: '92 / 100', grade: 'A', points: 4.0 },
    { id: 2, course: 'CSE-202 Relational Database Systems', exam: 'Laboratory Assessment', score: '98 / 100', grade: 'A+', points: 4.0 },
    { id: 3, course: 'MAT-301 Applied Linear Algebra', exam: 'Mid-Semester Exam', score: '85 / 100', grade: 'A-', points: 3.7 }
  ];

  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h2 className="text-2xl font-black text-white">Academic Transcripts & Grades</h2>
          <p className="text-xs text-slate-400 font-medium">Official examination scores and cumulative GPA metrics</p>
        </div>
        <div className="px-4 py-2 bg-emerald-500/20 border border-emerald-500/30 text-emerald-400 font-bold rounded-xl text-xs">
          Cumulative GPA: 3.88 / 4.00
        </div>
      </div>

      <div className="bg-slate-900 rounded-2xl border border-slate-800 shadow-sm overflow-hidden">
        <table className="w-full text-left border-collapse text-xs">
          <thead className="bg-slate-950 text-slate-300 font-bold uppercase">
            <tr>
              <th className="p-4">Course</th>
              <th className="p-4">Assessment Type</th>
              <th className="p-4">Score</th>
              <th className="p-4">Letter Grade</th>
              <th className="p-4">Grade Points</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-slate-800 font-medium text-slate-300">
            {results.map(res => (
              <tr key={res.id} className="hover:bg-slate-800/50 transition-colors">
                <td className="p-4 font-bold text-white">{res.course}</td>
                <td className="p-4 text-slate-400">{res.exam}</td>
                <td className="p-4 font-bold text-slate-200">{res.score}</td>
                <td className="p-4">
                  <span className="px-2.5 py-1 rounded bg-indigo-500/20 text-indigo-300 border border-indigo-500/30 font-bold text-[11px]">
                    {res.grade}
                  </span>
                </td>
                <td className="p-4 font-bold text-emerald-400">{res.points}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
};
