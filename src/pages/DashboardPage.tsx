import React, { useContext } from 'react';
import { AuthContext } from '../context/AuthContext';
import { MetricCard } from '../components/MetricCard';

interface DashboardPageProps {
  setActiveTab: (tab: string) => void;
}

export const DashboardPage: React.FC<DashboardPageProps> = ({ setActiveTab }) => {
  const auth = useContext(AuthContext);
  const user = auth?.user;

  return (
    <div className="space-y-6">
      <div className="bg-slate-900 text-white p-6 rounded-3xl border border-slate-800 shadow-xl flex flex-col md:flex-row items-start md:items-center justify-between gap-6">
        <div className="space-y-2">
          <span className="px-3 py-1 bg-indigo-500/20 text-indigo-300 border border-indigo-500/30 rounded-full text-[11px] font-bold uppercase tracking-wider">
            {`Role: ${user?.role}`}
          </span>
          <h2 className="text-2xl sm:text-3xl font-black tracking-tight">{`Academic Portal — ${user?.name}`}</h2>
          <p className="text-slate-400 text-xs max-w-xl font-medium">
            Access institutional records, course schedules, attendance logs, and academic transcripts in real-time.
          </p>
        </div>
        <div className="flex flex-wrap gap-3 w-full md:w-auto">
          <button
            onClick={() => setActiveTab('courses')}
            className="flex-1 md:flex-initial px-4 py-2.5 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors"
          >
            Course Catalog
          </button>
          <button
            onClick={() => setActiveTab('results')}
            className="flex-1 md:flex-initial px-4 py-2.5 bg-slate-800 hover:bg-slate-700 text-slate-200 font-bold rounded-xl text-xs border border-slate-700 transition-colors"
          >
            View Transcripts
          </button>
        </div>
      </div>

      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
        <MetricCard
          title="Cumulative GPA"
          value={user?.gpa ? `${user.gpa} / 4.00` : '3.88 / 4.00'}
          subtitle="Highest Academic Honors"
          icon="🎓"
          badge="Top 5%"
        />
        <MetricCard title="Attendance Record" value="96.2%" subtitle="Verified Class Attendance" icon="📅" />
        <MetricCard title="Active Enrollments" value="5 Courses" subtitle="16 Credit Hours Registered" icon="📚" />
        <MetricCard title="Current Semester" value="Fall 2026" subtitle="Academic Term 4" icon="🏛️" />
      </div>

      <div className="bg-slate-900 p-6 rounded-2xl border border-slate-800 shadow-sm space-y-4">
        <div className="flex items-center justify-between">
          <h3 className="text-base font-bold text-white">📢 University Announcements</h3>
          <button onClick={() => setActiveTab('notices')} className="text-xs font-bold text-indigo-400 hover:text-indigo-300">
            View All Bulletins →
          </button>
        </div>
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          <div className="p-4 rounded-xl bg-slate-950 border border-slate-800 space-y-2">
            <div className="flex items-center justify-between text-xs">
              <span className="px-2 py-0.5 rounded font-bold bg-indigo-500/20 text-indigo-300 border border-indigo-500/30">
                Academic Examination
              </span>
              <span className="text-slate-400 font-medium">2026-08-01</span>
            </div>
            <h4 className="font-bold text-white text-sm">Fall 2026 Mid-Semester Examination Schedule Published</h4>
            <p className="text-xs text-slate-400 font-medium">
              Official timetables for undergraduate and postgraduate mid-semester examinations have been released.
            </p>
          </div>
          <div className="p-4 rounded-xl bg-slate-950 border border-slate-800 space-y-2">
            <div className="flex items-center justify-between text-xs">
              <span className="px-2 py-0.5 rounded font-bold bg-emerald-500/20 text-emerald-300 border border-emerald-500/30">
                Research & Innovation
              </span>
              <span className="text-slate-400 font-medium">2026-07-28</span>
            </div>
            <h4 className="font-bold text-white text-sm">Annual University Innovation & AI Symposium</h4>
            <p className="text-xs text-slate-400 font-medium">
              Faculty and students are invited to submit research abstracts for presentation at the annual symposium.
            </p>
          </div>
        </div>
      </div>
    </div>
  );
};
