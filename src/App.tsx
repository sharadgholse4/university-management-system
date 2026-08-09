import React, { useState, useContext } from 'react';
import { AuthContext, AuthProvider } from './context/AuthContext';
import { Navbar } from './components/Navbar';
import { Sidebar } from './components/Sidebar';
import { BackgroundVideo } from './components/BackgroundVideo';

import { AuthPage } from './pages/AuthPage';
import { DashboardPage } from './pages/DashboardPage';
import { ProfilePage } from './pages/ProfilePage';
import { CoursesPage } from './pages/CoursesPage';
import { AttendancePage } from './pages/AttendancePage';
import { ResultsPage } from './pages/ResultsPage';
import { EnrollmentPage } from './pages/EnrollmentPage';
import { ReportsPage } from './pages/ReportsPage';
import { BulletinsPage } from './pages/BulletinsPage';

const MainLayout: React.FC = () => {
  const auth = useContext(AuthContext);
  const user = auth?.user;

  const [activeTab, setActiveTab] = useState('dashboard');
  const [mobileOpen, setMobileOpen] = useState(false);

  if (!user) {
    return <AuthPage />;
  }

  const renderTab = () => {
    switch (activeTab) {
      case 'dashboard': return <DashboardPage setActiveTab={setActiveTab} />;
      case 'profile': return <ProfilePage />;
      case 'courses': return <CoursesPage />;
      case 'attendance': return <AttendancePage />;
      case 'results': return <ResultsPage />;
      case 'enrollment': return <EnrollmentPage />;
      case 'reports': return <ReportsPage />;
      case 'notices': return <BulletinsPage />;
      default: return <DashboardPage setActiveTab={setActiveTab} />;
    }
  };

  return (
    <div className="min-h-screen bg-slate-950 text-slate-100 flex flex-col font-sans relative">
      <BackgroundVideo />
      <Navbar activeTab={activeTab} setActiveTab={setActiveTab} mobileOpen={mobileOpen} setMobileOpen={setMobileOpen} />
      <div className="flex-1 flex w-full relative z-10 overflow-hidden">
        <Sidebar activeTab={activeTab} setActiveTab={setActiveTab} mobileOpen={mobileOpen} setMobileOpen={setMobileOpen} />
        <main className="flex-1 p-4 sm:p-6 md:p-8 overflow-y-auto max-w-full">
          {renderTab()}
        </main>
      </div>
    </div>
  );
};

export const App: React.FC = () => {
  return (
    <AuthProvider>
      <MainLayout />
    </AuthProvider>
  );
};

export default App;
