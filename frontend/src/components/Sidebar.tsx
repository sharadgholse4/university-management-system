import React from 'react';
import { useAuth } from '../context/AuthContext';
import {
  LayoutDashboard,
  User,
  CalendarCheck,
  Award,
  BookOpen,
  UserPlus,
  BarChart3,
  Megaphone,
  ChevronRight,
} from 'lucide-react';

interface SidebarProps {
  activeTab: string;
  setActiveTab: (tab: string) => void;
}

export const Sidebar: React.FC<SidebarProps> = ({ activeTab, setActiveTab }) => {
  const { user } = useAuth();
  const role = user?.role || 'student';

  const navItems = [
    { id: 'dashboard', label: 'Dashboard', icon: LayoutDashboard, roles: ['student', 'professor', 'admin'] },
    { id: 'profile', label: 'Student Profile', icon: User, roles: ['student', 'professor', 'admin'] },
    { id: 'attendance', label: 'Attendance', icon: CalendarCheck, roles: ['student', 'professor', 'admin'] },
    { id: 'results', label: 'Results & CGPA', icon: Award, roles: ['student', 'professor', 'admin'] },
    { id: 'courses', label: 'Courses & Depts', icon: BookOpen, roles: ['student', 'professor', 'admin'] },
    { id: 'enrollments', label: 'Course Registration', icon: UserPlus, roles: ['student', 'professor', 'admin'] },
    { id: 'reports', label: 'Analytics & Reports', icon: BarChart3, roles: ['professor', 'admin'] },
    { id: 'announcements', label: 'Announcements', icon: Megaphone, roles: ['student', 'professor', 'admin'] },
  ];

  const filteredItems = navItems.filter((item) => item.roles.includes(role));

  return (
    <aside className="w-64 glass-panel border-r border-slate-200 shrink-0 hidden md:block min-h-[calc(100vh-4rem)] p-4">
      <div className="space-y-1">
        <div className="px-3 py-2 text-xs font-bold uppercase tracking-wider text-slate-400">
          Navigation
        </div>
        {filteredItems.map((item) => {
          const Icon = item.icon;
          const isActive = activeTab === item.id;
          return (
            <button
              key={item.id}
              onClick={() => setActiveTab(item.id)}
              className={`w-full flex items-center justify-between px-3 py-2.5 rounded-xl font-medium text-sm transition-all ${
                isActive
                  ? 'bg-sky-600 text-white shadow-md shadow-sky-600/20 font-semibold'
                  : 'text-slate-600 hover:text-slate-900 hover:bg-slate-100/80'
              }`}
            >
              <div className="flex items-center gap-3">
                <Icon className={`w-5 h-5 ${isActive ? 'text-white' : 'text-slate-400'}`} />
                <span>{item.label}</span>
              </div>
              {isActive && <ChevronRight className="w-4 h-4 text-white/80" />}
            </button>
          );
        })}
      </div>
    </aside>
  );
};
