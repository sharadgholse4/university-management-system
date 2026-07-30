import React from 'react';
import { useAuth } from '../context/AuthContext';
import { BackendStatusBadge } from './BackendStatusBadge';
import { GraduationCap, UserCheck, Shield, LogOut, Sparkles } from 'lucide-react';
import { Role } from '../types';

interface NavbarProps {
  activeTab: string;
  setActiveTab: (tab: string) => void;
}

export const Navbar: React.FC<NavbarProps> = ({ activeTab, setActiveTab }) => {
  const { user, studentProfile, isAuthenticated, loginAsDemo, logout } = useAuth();

  const getRoleBadge = (role?: Role) => {
    switch (role) {
      case 'admin':
        return (
          <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-bold bg-amber-100 text-amber-800 border border-amber-300">
            <Shield className="w-3 h-3" /> Admin
          </span>
        );
      case 'professor':
        return (
          <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-bold bg-purple-100 text-purple-800 border border-purple-300">
            <UserCheck className="w-3 h-3" /> Professor
          </span>
        );
      default:
        return (
          <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-bold bg-sky-100 text-sky-800 border border-sky-300">
            <GraduationCap className="w-3 h-3" /> Student
          </span>
        );
    }
  };

  return (
    <header className="sticky top-0 z-40 w-full glass-panel border-b border-slate-200 shadow-sm">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex items-center justify-between h-16">
          {/* Brand Logo */}
          <div className="flex items-center gap-3 cursor-pointer" onClick={() => setActiveTab('dashboard')}>
            <div className="w-10 h-10 rounded-xl bg-gradient-to-tr from-sky-600 via-indigo-600 to-purple-600 flex items-center justify-center text-white shadow-md shadow-sky-500/20">
              <GraduationCap className="w-6 h-6" />
            </div>
            <div>
              <span className="font-serif-heading text-xl font-bold text-slate-900 tracking-tight">
                EduPortal
              </span>
              <span className="hidden sm:inline-block ml-2 text-xs font-semibold px-2 py-0.5 rounded bg-slate-100 text-slate-600 border border-slate-200">
                v2.0 C++ REST
              </span>
            </div>
          </div>

          {/* Center Actions / Demo Switcher */}
          <div className="hidden md:flex items-center gap-2 bg-slate-100 p-1.5 rounded-xl border border-slate-200">
            <span className="text-xs font-semibold text-slate-500 px-2 flex items-center gap-1">
              <Sparkles className="w-3 h-3 text-amber-500" /> Switch Role:
            </span>
            <button
              onClick={() => loginAsDemo('student')}
              className={`px-3 py-1 rounded-lg text-xs font-semibold transition-all ${
                user?.role === 'student'
                  ? 'bg-white text-sky-700 shadow-sm font-bold border border-sky-200'
                  : 'text-slate-600 hover:text-slate-900 hover:bg-slate-200/60'
              }`}
            >
              Student
            </button>
            <button
              onClick={() => loginAsDemo('professor')}
              className={`px-3 py-1 rounded-lg text-xs font-semibold transition-all ${
                user?.role === 'professor'
                  ? 'bg-white text-purple-700 shadow-sm font-bold border border-purple-200'
                  : 'text-slate-600 hover:text-slate-900 hover:bg-slate-200/60'
              }`}
            >
              Professor
            </button>
            <button
              onClick={() => loginAsDemo('admin')}
              className={`px-3 py-1 rounded-lg text-xs font-semibold transition-all ${
                user?.role === 'admin'
                  ? 'bg-white text-amber-700 shadow-sm font-bold border border-amber-200'
                  : 'text-slate-600 hover:text-slate-900 hover:bg-slate-200/60'
              }`}
            >
              Admin
            </button>
          </div>

          {/* Right Side: Backend Badge & User Profile */}
          <div className="flex items-center gap-3">
            <BackendStatusBadge />

            {isAuthenticated && user && (
              <div className="flex items-center gap-3 pl-3 border-l border-slate-200">
                <div className="hidden lg:block text-right">
                  <div className="text-sm font-bold text-slate-800">
                    {studentProfile
                      ? `${studentProfile.firstName} ${studentProfile.lastName}`
                      : user.email.split('@')[0]}
                  </div>
                  <div className="text-xs text-slate-500">{user.email}</div>
                </div>
                {getRoleBadge(user.role)}
                <button
                  onClick={logout}
                  title="Logout"
                  className="p-2 rounded-lg text-slate-500 hover:text-rose-600 hover:bg-rose-50 transition-colors"
                >
                  <LogOut className="w-4 h-4" />
                </button>
              </div>
            )}
          </div>
        </div>
      </div>
    </header>
  );
};
