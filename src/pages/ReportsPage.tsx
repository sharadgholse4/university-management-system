import React from 'react';
import { MetricCard } from '../components/MetricCard';

export const ReportsPage: React.FC = () => {
  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h2 className="text-2xl font-black text-white">Institutional Reports & Analytics</h2>
          <p className="text-xs text-slate-400 font-medium">System analytics, enrollment distributions, and official reports</p>
        </div>
        <button
          onClick={() => window.print()}
          className="px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors"
        >
          📄 Export Official PDF
        </button>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        <MetricCard title="Registered Students" value="1,840" subtitle="Across All Departments" icon="👥" />
        <MetricCard title="Academic Faculty" value="112" subtitle="Full-Time Professors" icon="👨‍🏫" />
        <MetricCard title="Average Attendance" value="94.8%" subtitle="Fall Term 2026" icon="📈" />
      </div>
    </div>
  );
};
