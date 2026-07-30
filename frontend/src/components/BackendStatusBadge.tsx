import React from 'react';
import { useAuth } from '../context/AuthContext';
import { Server, RefreshCw } from 'lucide-react';

export const BackendStatusBadge: React.FC = () => {
  const { backendOnline, checkBackendStatus } = useAuth();
  const [checking, setChecking] = React.useState(false);

  const handleRefresh = async () => {
    setChecking(true);
    await checkBackendStatus();
    setTimeout(() => setChecking(false), 500);
  };

  return (
    <button
      onClick={handleRefresh}
      title="Click to re-check C++ Crow Backend connection (Port 8080)"
      className={`inline-flex items-center gap-2 px-3 py-1.5 rounded-full text-xs font-semibold transition-all border ${
        backendOnline
          ? 'bg-emerald-50 text-emerald-700 border-emerald-200 hover:bg-emerald-100'
          : 'bg-amber-50 text-amber-700 border-amber-200 hover:bg-amber-100'
      }`}
    >
      <span className="relative flex h-2 w-2">
        <span
          className={`animate-ping absolute inline-flex h-full w-full rounded-full opacity-75 ${
            backendOnline ? 'bg-emerald-400' : 'bg-amber-400'
          }`}
        ></span>
        <span
          className={`relative inline-flex rounded-full h-2 w-2 ${
            backendOnline ? 'bg-emerald-500' : 'bg-amber-500'
          }`}
        ></span>
      </span>
      <Server className="w-3.5 h-3.5" />
      <span>{backendOnline ? 'C++ Backend Live (8080)' : 'Backend Syncing...'}</span>
      <RefreshCw className={`w-3 h-3 ml-1 ${checking ? 'animate-spin' : ''}`} />
    </button>
  );
};
