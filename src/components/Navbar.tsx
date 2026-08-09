import React, { useState, useEffect, useContext } from 'react';
import { AuthContext } from '../context/AuthContext';

function getDynamicGreeting(userName?: string): string {
  const hour = new Date().getHours();
  let timeOfDay = 'Good morning';
  let emoji = '🌅';

  if (hour >= 12 && hour < 17) {
    timeOfDay = 'Good afternoon';
    emoji = '☀️';
  } else if (hour >= 17 && hour < 22) {
    timeOfDay = 'Good evening';
    emoji = '🌆';
  } else if (hour >= 22 || hour < 5) {
    timeOfDay = 'Good night';
    emoji = '🌙';
  }

  const firstName = (userName || '').split(' ')[0] || 'User';
  return `${emoji} ${timeOfDay}, ${firstName}!`;
}

export const DynamicGreetingBadge: React.FC = () => {
  const auth = useContext(AuthContext);
  const user = auth?.user;
  const [greeting, setGreeting] = useState(() => getDynamicGreeting(user?.name));

  useEffect(() => {
    const timer = setInterval(() => {
      setGreeting(getDynamicGreeting(user?.name));
    }, 60000);
    return () => clearInterval(timer);
  }, [user]);

  if (!user) return null;

  return (
    <div className="hidden md:flex items-center gap-2 px-3.5 py-1.5 rounded-xl bg-indigo-500/10 border border-indigo-500/30 text-indigo-300 text-xs font-bold shadow-sm">
      <span>{greeting}</span>
    </div>
  );
};

interface NavbarProps {
  activeTab: string;
  setActiveTab: (tab: string) => void;
  mobileOpen: boolean;
  setMobileOpen: (open: boolean) => void;
}

export const Navbar: React.FC<NavbarProps> = ({ setActiveTab, mobileOpen, setMobileOpen }) => {
  const auth = useContext(AuthContext);
  const user = auth?.user;
  const logout = auth?.logout;

  return (
    <header className="bg-slate-900/90 border-b border-slate-800 text-white backdrop-blur-md sticky top-0 z-50 shadow-md">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 h-16 flex items-center justify-between gap-2">
        <div className="flex items-center gap-3">
          <button
            onClick={() => setMobileOpen(!mobileOpen)}
            className="lg:hidden p-2 rounded-xl border border-slate-700 bg-slate-800 text-slate-300 text-sm font-bold"
          >
            {mobileOpen ? '✕' : '☰'}
          </button>
          <div className="flex items-center gap-2.5 cursor-pointer" onClick={() => setActiveTab('dashboard')}>
            <div className="w-9 h-9 rounded-xl bg-gradient-to-tr from-indigo-500 to-sky-400 flex items-center justify-center text-white font-black text-lg shadow-lg shadow-indigo-500/20">
              🏛️
            </div>
            <div>
              <h1 className="text-base sm:text-lg font-extrabold tracking-tight text-white">EDUPORTAL</h1>
              <p className="text-[9px] sm:text-[10px] text-slate-400 font-bold tracking-widest uppercase hidden xs:block">
                Academic System
              </p>
            </div>
          </div>
        </div>

        <DynamicGreetingBadge />

        {user && (
          <div className="flex items-center gap-3">
            <div className="text-right hidden sm:block">
              <div className="text-xs font-bold text-slate-100">{user.name}</div>
              <div className="text-[10px] font-bold uppercase tracking-wider text-indigo-400">
                {`${user.role} • ${user.department || 'Academic'}`}
              </div>
            </div>
            <button
              onClick={logout}
              className="px-3 py-1.5 bg-slate-800 hover:bg-rose-950 hover:text-rose-300 text-slate-300 rounded-xl text-xs font-bold border border-slate-700 transition-colors flex items-center gap-1.5"
            >
              <span>🔒</span>
              <span className="hidden sm:inline">Sign Out</span>
            </button>
          </div>
        )}
      </div>
    </header>
  );
};
