import React, { useContext } from 'react';
import { AuthContext } from '../context/AuthContext';

export const ProfilePage: React.FC = () => {
  const auth = useContext(AuthContext);
  const user = auth?.user;

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      <div className="bg-slate-900 rounded-3xl border border-slate-800 shadow-lg overflow-hidden">
        <div className="h-32 bg-slate-950" />
        <div className="px-6 pb-6 relative">
          <div className="flex flex-col sm:flex-row items-start sm:items-end justify-between -mt-12 mb-4 gap-4">
            <div className="flex items-end gap-4">
              <div className="w-24 h-24 rounded-2xl bg-indigo-600 border-4 border-slate-900 shadow-xl flex items-center justify-center text-4xl text-white font-black">
                {user?.name?.[0] || 'U'}
              </div>
              <div>
                <h2 className="text-2xl font-black text-white">{user?.name}</h2>
                <p className="text-xs text-indigo-400 font-bold uppercase tracking-wider">
                  {`${user?.role} • ${user?.department || 'Academic'}`}
                </p>
              </div>
            </div>
            <span className="px-3 py-1 rounded-full bg-emerald-500/20 text-emerald-400 border border-emerald-500/30 text-xs font-bold">
              Status: Active Institutional Account
            </span>
          </div>

          <div className="grid grid-cols-1 sm:grid-cols-2 gap-4 mt-6 pt-6 border-t border-slate-800 text-xs">
            <div className="space-y-1">
              <p className="text-[11px] text-slate-400 font-bold uppercase">Institutional Email</p>
              <p className="font-semibold text-white text-sm">{user?.email}</p>
            </div>
            {user?.rollNumber && (
              <div className="space-y-1">
                <p className="text-[11px] text-slate-400 font-bold uppercase">Roll Number</p>
                <p className="font-semibold text-white text-sm">{user?.rollNumber}</p>
              </div>
            )}
            <div className="space-y-1">
              <p className="text-[11px] text-slate-400 font-bold uppercase">Academic Department</p>
              <p className="font-semibold text-white text-sm">{user?.department || 'Computer Science & Engineering'}</p>
            </div>
            {user?.designation && (
              <div className="space-y-1">
                <p className="text-[11px] text-slate-400 font-bold uppercase">Designation</p>
                <p className="font-semibold text-white text-sm">{user?.designation}</p>
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
};
