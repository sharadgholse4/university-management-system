import React, { useContext } from 'react';
import { AuthContext } from '../context/AuthContext';
import { Role } from '../types';

interface SidebarProps {
  activeTab: string;
  setActiveTab: (tab: string) => void;
  mobileOpen: boolean;
  setMobileOpen?: (open: boolean) => void;
}

interface NavItem {
  id: string;
  label: string;
  icon: string;
  roles: Role[];
}

export const Sidebar: React.FC<SidebarProps> = ({ activeTab, setActiveTab, mobileOpen, setMobileOpen }) => {
  const auth = useContext(AuthContext);
  const user = auth?.user;

  const items: NavItem[] = [
    { id: 'dashboard', label: 'Overview Dashboard', icon: '📊', roles: ['student', 'professor', 'admin'] },
    { id: 'profile', label: 'Academic Profile', icon: '👤', roles: ['student', 'professor', 'admin'] },
    { id: 'courses', label: 'Course Catalog', icon: '📚', roles: ['student', 'professor', 'admin'] },
    { id: 'enrollment', label: 'Course Registrations', icon: '📝', roles: ['student', 'admin'] },
    { id: 'attendance', label: 'Attendance Registry', icon: '📅', roles: ['student', 'professor', 'admin'] },
    { id: 'results', label: 'Examinations & Transcripts', icon: '🎓', roles: ['student', 'professor', 'admin'] },
    { id: 'reports', label: 'Analytics & Reports', icon: '📈', roles: ['professor', 'admin'] },
    { id: 'notices', label: 'University Bulletins', icon: '📢', roles: ['student', 'professor', 'admin'] }
  ];

  const visibleItems = items.filter(item => item.roles.includes(user?.role || 'student'));

  const handleSelect = (id: string) => {
    setActiveTab(id);
    if (setMobileOpen) setMobileOpen(false);
  };

  const navContent = (
    <div className="flex flex-col justify-between h-full space-y-4">
      <nav className="space-y-1">
        <div className="text-[11px] font-bold text-slate-500 tracking-wider uppercase px-3 mb-3 flex items-center justify-between">
          <span>Navigation Portal</span>
          {setMobileOpen && (
            <button onClick={() => setMobileOpen(false)} className="lg:hidden text-slate-400 hover:text-white font-bold text-sm">
              ✕
            </button>
          )}
        </div>
        {visibleItems.map(item => (
          <button
            key={item.id}
            onClick={() => handleSelect(item.id)}
            className={`w-full flex items-center gap-3 px-3.5 py-2.5 rounded-xl text-xs font-bold transition-all ${
              activeTab === item.id ? 'bg-indigo-600 text-white shadow-lg shadow-indigo-600/30' : 'text-slate-400 hover:bg-slate-800 hover:text-slate-100'
            }`}
          >
            <span className="text-base">{item.icon}</span>
            {item.label}
          </button>
        ))}
      </nav>

      <div className="p-3 bg-slate-950 rounded-xl border border-slate-800 text-[11px] text-slate-400 space-y-1 text-center font-medium">
        <div className="font-bold text-slate-300">© 2026 EduPortal</div>
        <div className="flex items-center justify-center gap-1 text-slate-400">
          Made with <span className="text-rose-500 text-xs">❤️</span>
        </div>
      </div>
    </div>
  );

  return (
    <>
      <aside className="hidden lg:flex w-64 bg-slate-900 border-r border-slate-800 text-slate-300 p-4 flex-col justify-between shrink-0 min-h-[calc(100vh-4rem)]">
        {navContent}
      </aside>

      {mobileOpen && (
        <div className="lg:hidden fixed inset-0 z-50 flex">
          <div className="fixed inset-0 bg-slate-950/80 backdrop-blur-sm" onClick={() => setMobileOpen?.(false)} />
          <aside className="relative w-64 bg-slate-900 border-r border-slate-800 text-slate-300 p-4 flex flex-col justify-between shrink-0 h-full shadow-2xl z-10">
            {navContent}
          </aside>
        </div>
      )}
    </>
  );
};
