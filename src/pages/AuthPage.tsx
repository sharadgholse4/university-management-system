import React, { useState, useContext } from 'react';
import { AuthContext, DEMO_PRESETS } from '../context/AuthContext';
import { BackgroundVideo } from '../components/BackgroundVideo';
import { Role } from '../types';

const GOOGLE_CLIENT_ID = '445838676324-impfkq5c9utvu6ff3inh6iv8s67vvuph.apps.googleusercontent.com';

declare global {
  interface Window {
    google?: any;
  }
}

export const AuthPage: React.FC = () => {
  const auth = useContext(AuthContext);
  const login = auth?.login;
  const register = auth?.register;
  const loginWithGoogle = auth?.loginWithGoogle;
  const loading = auth?.loading;

  const [mode, setMode] = useState<'login' | 'register'>('login');
  
  const [loginUsername, setLoginUsername] = useState('');
  const [loginPassword, setLoginPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [rememberMe, setRememberMe] = useState(true);

  const [regName, setRegName] = useState('');
  const [regEmail, setRegEmail] = useState('');
  const [regUsername, setRegUsername] = useState('');
  const [regPassword, setRegPassword] = useState('');
  const [regConfirmPassword, setRegConfirmPassword] = useState('');
  const [regRole, setRegRole] = useState<Role>('student');
  const [regDepartment, setRegDepartment] = useState('Computer Science & Engineering');
  const [regIdNumber, setRegIdNumber] = useState('');

  const [error, setError] = useState<string | null>(null);
  const [successMsg, setSuccessMsg] = useState<string | null>(null);

  const handleLoginSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError(null);
    setSuccessMsg(null);
    if (!loginUsername.trim() || !loginPassword.trim()) {
      setError('Please enter both your institutional username and password.');
      return;
    }
    if (login) await login(loginUsername.trim(), loginPassword);
  };

  const fillQuickPreset = (roleKey: string) => {
    const preset = DEMO_PRESETS[roleKey];
    if (preset && login) {
      setLoginUsername(preset.username);
      setLoginPassword('Password123!');
      login(preset.username, 'Password123!');
    }
  };

  const handleRegisterSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError(null);
    setSuccessMsg(null);

    if (!regName.trim() || !regEmail.trim() || !regUsername.trim() || !regPassword) {
      setError('All required registration fields must be completed.');
      return;
    }
    if (!regEmail.includes('@') || !regEmail.includes('.')) {
      setError('Please enter a valid institutional email address.');
      return;
    }
    if (regPassword.length < 6) {
      setError('Password must be at least 6 characters long.');
      return;
    }
    if (regPassword !== regConfirmPassword) {
      setError('Passwords do not match. Please re-enter your password.');
      return;
    }

    if (register) {
      await register({
        name: regName.trim(),
        email: regEmail.trim(),
        username: regUsername.trim(),
        password: regPassword,
        role: regRole,
        department: regDepartment,
        rollNumber: regIdNumber.trim()
      });
    }

    setSuccessMsg('Account created successfully! Redirecting to portal...');
  };

  const handleGoogleSignIn = () => {
    const cleanClientId = (GOOGLE_CLIENT_ID || '').trim();

    if (cleanClientId && window.google?.accounts?.oauth2 && loginWithGoogle) {
      try {
        const client = window.google.accounts.oauth2.initTokenClient({
          client_id: cleanClientId,
          scope: 'email profile',
          callback: async (tokenResponse: any) => {
            if (tokenResponse && tokenResponse.access_token) {
              try {
                const userInfo = await fetch('https://www.googleapis.com/oauth2/v3/userinfo', {
                  headers: { Authorization: `Bearer ${tokenResponse.access_token}` }
                }).then(res => res.json());

                if (userInfo && userInfo.email) {
                  loginWithGoogle({
                    name: userInfo.name || 'Google User',
                    email: userInfo.email,
                    picture: userInfo.picture || ''
                  });
                  return;
                }
              } catch (e) {
                console.error('[Google OAuth] Failed to fetch Google userinfo:', e);
              }
            }
          }
        });
        client.requestAccessToken();
        return;
      } catch (err) {
        console.warn('[Google OAuth] Token client failed, falling back to demo mode:', err);
      }
    }

    if (loginWithGoogle) {
      loginWithGoogle({
        name: 'Alex Johnson (Google Workspace SSO)',
        email: 'alex.johnson@university.edu',
        picture: ''
      });
    }
  };

  return (
    <div className="min-h-screen bg-slate-950 flex items-center justify-center p-4 font-sans relative overflow-hidden">
      <BackgroundVideo />
      <div className="absolute -top-40 -left-40 w-96 h-96 bg-indigo-600/30 rounded-full blur-3xl pointer-events-none" />
      <div className="absolute -bottom-40 -right-40 w-96 h-96 bg-sky-500/30 rounded-full blur-3xl pointer-events-none" />

      <div className="w-full max-w-lg bg-slate-900/90 border border-slate-800 backdrop-blur-2xl p-6 sm:p-8 rounded-3xl space-y-6 relative z-10 shadow-2xl">
        <div className="text-center space-y-2">
          <div className="w-14 h-14 rounded-2xl bg-gradient-to-tr from-indigo-500 to-sky-400 mx-auto flex items-center justify-center text-white font-black text-2xl shadow-xl shadow-indigo-500/30">
            🏛️
          </div>
          <h2 className="text-xl sm:text-2xl font-black text-white tracking-tight">EDUPORTAL ACADEMIC SYSTEM</h2>
          <p className="text-xs text-indigo-400 font-bold">Official Student & Faculty Portal Access</p>
        </div>

        <div className="flex p-1.5 bg-slate-950 rounded-2xl border border-slate-800 text-xs font-bold">
          <button
            onClick={() => { setMode('login'); setError(null); }}
            className={`flex-1 py-2 rounded-xl transition-all ${mode === 'login' ? 'bg-indigo-600 text-white shadow-md' : 'text-slate-400 hover:text-white'}`}
          >
            🔑 Sign In
          </button>
          <button
            onClick={() => { setMode('register'); setError(null); }}
            className={`flex-1 py-2 rounded-xl transition-all ${mode === 'register' ? 'bg-indigo-600 text-white shadow-md' : 'text-slate-400 hover:text-white'}`}
          >
            📝 Create Account (Sign Up)
          </button>
        </div>

        {error && <div className="p-3 bg-rose-500/10 border border-rose-500/20 rounded-xl text-rose-400 text-xs font-bold text-center">{error}</div>}
        {successMsg && <div className="p-3 bg-emerald-500/10 border border-emerald-500/20 rounded-xl text-emerald-400 text-xs font-bold text-center">{successMsg}</div>}

        {mode === 'login' && (
          <div className="space-y-4">
            <button
              type="button"
              onClick={handleGoogleSignIn}
              className="w-full py-2.5 bg-white hover:bg-slate-100 text-slate-900 font-bold text-xs rounded-xl flex items-center justify-center gap-3 transition-colors shadow-md"
            >
              <svg className="w-4 h-4" viewBox="0 0 24 24">
                <path fill="#4285F4" d="M22.56 12.25c0-.78-.07-1.53-.2-2.25H12v4.26h5.92c-.26 1.37-1.04 2.53-2.21 3.31v2.77h3.57c2.08-1.92 3.28-4.74 3.28-8.09z" />
                <path fill="#34A853" d="M12 23c2.97 0 5.46-.98 7.28-2.66l-3.57-2.77c-.98.66-2.23 1.06-3.71 1.06-2.86 0-5.29-1.93-6.16-4.53H2.18v2.84C3.99 20.53 7.7 23 12 23z" />
                <path fill="#FBBC05" d="M5.84 14.09c-.22-.66-.35-1.36-.35-2.09s.13-1.43.35-2.09V7.06H2.18C1.43 8.55 1 10.22 1 12s.43 3.45 1.18 4.94l2.85-2.22.81-.63z" />
                <path fill="#EA4335" d="M12 5.38c1.62 0 3.06.56 4.21 1.64l3.15-3.15C17.45 2.09 14.97 1 12 1 7.7 1 3.99 3.47 2.18 7.06l3.66 2.84c.87-2.6 3.3-4.52 6.16-4.52z" />
              </svg>
              Continue with Google Workspace
            </button>

            <div className="flex items-center gap-3 my-2">
              <div className="flex-1 h-px bg-slate-800" />
              <span className="text-[10px] text-slate-500 font-bold uppercase tracking-wider">Or Sign In with Credentials</span>
              <div className="flex-1 h-px bg-slate-800" />
            </div>

            <form onSubmit={handleLoginSubmit} className="space-y-3 text-xs">
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Username or Institutional Email</label>
                <input
                  type="text"
                  value={loginUsername}
                  placeholder="alex.johnson or alex@university.edu"
                  onChange={(e) => setLoginUsername(e.target.value)}
                  className="w-full px-3.5 py-2.5 bg-slate-950 border border-slate-800 text-white rounded-xl focus:outline-none focus:border-indigo-500"
                />
              </div>

              <div className="space-y-1">
                <div className="flex justify-between items-center">
                  <label className="font-bold text-slate-300">Password</label>
                  <button
                    type="button"
                    onClick={() => setShowPassword(!showPassword)}
                    className="text-[11px] text-indigo-400 font-bold hover:underline"
                  >
                    {showPassword ? 'Hide' : 'Show'}
                  </button>
                </div>
                <input
                  type={showPassword ? 'text' : 'password'}
                  value={loginPassword}
                  placeholder="••••••••"
                  onChange={(e) => setLoginPassword(e.target.value)}
                  className="w-full px-3.5 py-2.5 bg-slate-950 border border-slate-800 text-white rounded-xl focus:outline-none focus:border-indigo-500"
                />
              </div>

              <div className="flex items-center justify-between text-xs">
                <label className="flex items-center gap-2 cursor-pointer">
                  <input
                    type="checkbox"
                    checked={rememberMe}
                    onChange={(e) => setRememberMe(e.target.checked)}
                    className="rounded bg-slate-800 border-slate-700 text-indigo-600 focus:ring-0"
                  />
                  <span className="text-slate-400 font-medium">Keep me signed in</span>
                </label>
                <span className="text-indigo-400 font-bold cursor-pointer hover:underline">Need help?</span>
              </div>

              <button
                type="submit"
                disabled={loading}
                className="w-full py-3 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl shadow-lg shadow-indigo-600/30 transition-all uppercase tracking-wider"
              >
                {loading ? 'Authenticating Credentials...' : 'Sign In to Portal'}
              </button>
            </form>

            <div className="pt-4 border-t border-slate-800/80 space-y-2">
              <div className="text-[10px] font-bold text-slate-400 uppercase tracking-wider text-center">Quick Demo Profile Test Sign-In</div>
              <div className="grid grid-cols-3 gap-2">
                <button
                  type="button"
                  onClick={() => fillQuickPreset('student')}
                  className="py-2 bg-slate-800 hover:bg-slate-700 text-slate-300 font-bold text-[11px] rounded-xl border border-slate-700 transition-colors"
                >
                  🎓 Student
                </button>
                <button
                  type="button"
                  onClick={() => fillQuickPreset('professor')}
                  className="py-2 bg-slate-800 hover:bg-slate-700 text-slate-300 font-bold text-[11px] rounded-xl border border-slate-700 transition-colors"
                >
                  👨‍🏫 Professor
                </button>
                <button
                  type="button"
                  onClick={() => fillQuickPreset('admin')}
                  className="py-2 bg-slate-800 hover:bg-slate-700 text-slate-300 font-bold text-[11px] rounded-xl border border-slate-700 transition-colors"
                >
                  🏛️ Admin
                </button>
              </div>
            </div>
          </div>
        )}

        {mode === 'register' && (
          <form onSubmit={handleRegisterSubmit} className="space-y-3 text-xs">
            <div className="grid grid-cols-1 sm:grid-cols-2 gap-3">
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Full Name *</label>
                <input
                  type="text"
                  value={regName}
                  placeholder="Alex Johnson"
                  onChange={(e) => setRegName(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 text-white rounded-xl"
                />
              </div>
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Username *</label>
                <input
                  type="text"
                  value={regUsername}
                  placeholder="alex.j"
                  onChange={(e) => setRegUsername(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 text-white rounded-xl"
                />
              </div>
            </div>

            <div className="space-y-1">
              <label className="font-bold text-slate-300">Institutional Email *</label>
              <input
                type="email"
                value={regEmail}
                placeholder="alex.johnson@university.edu"
                onChange={(e) => setRegEmail(e.target.value)}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 text-white rounded-xl"
              />
            </div>

            <div className="grid grid-cols-1 sm:grid-cols-2 gap-3">
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Account Role</label>
                <select
                  value={regRole}
                  onChange={(e) => setRegRole(e.target.value as Role)}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 text-white rounded-xl"
                >
                  <option value="student">Student</option>
                  <option value="professor">Faculty / Professor</option>
                </select>
              </div>
              <div className="space-y-1">
                <label className="font-bold text-slate-300">{regRole === 'student' ? 'Roll Number' : 'Employee ID'}</label>
                <input
                  type="text"
                  value={regIdNumber}
                  placeholder={regRole === 'student' ? 'CSE-2026-105' : 'EMP-9021'}
                  onChange={(e) => setRegIdNumber(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 text-white rounded-xl"
                />
              </div>
            </div>

            <div className="space-y-1">
              <label className="font-bold text-slate-300">Department</label>
              <select
                value={regDepartment}
                onChange={(e) => setRegDepartment(e.target.value)}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 text-white rounded-xl"
              >
                <option value="Computer Science & Engineering">Computer Science & Engineering</option>
                <option value="Electrical & Electronics Engineering">Electrical & Electronics Engineering</option>
                <option value="Mechanical Engineering">Mechanical Engineering</option>
                <option value="Mathematics & Data Science">Mathematics & Data Science</option>
                <option value="School of Business Administration">School of Business Administration</option>
              </select>
            </div>

            <div className="grid grid-cols-1 sm:grid-cols-2 gap-3">
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Password *</label>
                <input
                  type="password"
                  value={regPassword}
                  placeholder="••••••••"
                  onChange={(e) => setRegPassword(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 text-white rounded-xl"
                />
              </div>
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Confirm Password *</label>
                <input
                  type="password"
                  value={regConfirmPassword}
                  placeholder="••••••••"
                  onChange={(e) => setRegConfirmPassword(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 text-white rounded-xl"
                />
              </div>
            </div>

            <button
              type="submit"
              disabled={loading}
              className="w-full py-3 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl shadow-lg shadow-indigo-600/30 transition-all uppercase tracking-wider mt-2"
            >
              {loading ? 'Registering Account...' : 'Complete Registration & Sign In'}
            </button>
          </form>
        )}
      </div>
    </div>
  );
};
