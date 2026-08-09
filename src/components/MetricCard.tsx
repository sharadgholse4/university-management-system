import React from 'react';

interface MetricCardProps {
  title: string;
  value: string;
  subtitle?: string;
  icon: string;
  badge?: string;
}

export const MetricCard: React.FC<MetricCardProps> = ({ title, value, subtitle, icon, badge }) => {
  return (
    <div className="bg-slate-900 p-5 rounded-2xl border border-slate-800 shadow-lg space-y-2">
      <div className="flex items-center justify-between">
        <span className="text-xs font-bold text-slate-400 uppercase tracking-wider">{title}</span>
        <span className="text-xl">{icon}</span>
      </div>
      <div className="flex items-baseline justify-between">
        <h3 className="text-2xl font-black text-white">{value}</h3>
        {badge && (
          <span className="px-2 py-0.5 rounded text-[11px] font-bold bg-emerald-500/20 text-emerald-400 border border-emerald-500/30">
            {badge}
          </span>
        )}
      </div>
      {subtitle && <p className="text-xs text-slate-400 font-medium">{subtitle}</p>}
    </div>
  );
};
