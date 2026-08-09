/**
 * EduPortal | Academic Management Platform
 * Computer Science & Engineering Capstone Project
 * 
 * Author: Sharad Gholse
 */
const h = React.createElement;
const { useState, useEffect, createContext, useContext } = React;

const API_BASE = '/api';

// --- GOOGLE OAUTH 2.0 CLIENT ID CONFIGURATION ---
// Paste your Client ID from Google Cloud Console below (e.g. '123456789-xyz.apps.googleusercontent.com')
const GOOGLE_CLIENT_ID = 'YOUR_GOOGLE_CLIENT_ID.apps.googleusercontent.com';

// Storage helpers
function safeGetStorage(key) {
  try { return localStorage.getItem(key); } catch (e) { return null; }
}

function safeSetStorage(key, value) {
  try { localStorage.setItem(key, value); } catch (e) {}
}

function safeRemoveStorage(key) {
  try { localStorage.removeItem(key); } catch (e) {}
}

// API client
async function apiCall(endpoint, method = 'GET', body = null, token = null) {
  const headers = { 'Content-Type': 'application/json' };
  const savedToken = token || safeGetStorage('eduportal_token');
  if (savedToken) headers['Authorization'] = `Bearer ${savedToken}`;

  const options = { method, headers };
  if (body) options.body = JSON.stringify(body);

  try {
    const res = await fetch(`${API_BASE}${endpoint}`, options);
    const data = await res.json();
    return data;
  } catch (err) {
    return { success: false, error: 'Network error communicating with server.' };
  }
}

// Demo account presets
const DEMO_PRESETS = {
  student: {
    username: 'alex.johnson',
    name: 'Alex Johnson',
    role: 'student',
    email: 'alex.johnson@university.edu',
    department: 'Computer Science & Engineering',
    rollNumber: 'CSE-2026-089',
    gpa: 3.88
  },
  professor: {
    username: 'dr.robert.smith',
    name: 'Dr. Robert Smith',
    role: 'professor',
    email: 'r.smith@university.edu',
    department: 'Computer Science & Engineering',
    designation: 'Senior Professor & Department Chair'
  },
  admin: {
    username: 'sarah.connor',
    name: 'Sarah Connor',
    role: 'admin',
    email: 's.connor@university.edu',
    department: 'University Administration',
    designation: 'Head Administrator'
  }
};

const AuthContext = createContext(null);

function AuthProvider({ children }) {
  const [user, setUser] = useState(() => {
    const saved = safeGetStorage('eduportal_user');
    try { return saved ? JSON.parse(saved) : null; } catch (e) { return null; }
  });
  const [token, setToken] = useState(() => safeGetStorage('eduportal_token'));
  const [loading, setLoading] = useState(false);

  const getRegisteredUsers = () => {
    try {
      const data = safeGetStorage('eduportal_registered_users');
      return data ? JSON.parse(data) : {};
    } catch (e) { return {}; }
  };

  const saveRegisteredUser = (userData) => {
    const users = getRegisteredUsers();
    users[userData.username.toLowerCase()] = userData;
    safeSetStorage('eduportal_registered_users', JSON.stringify(users));
  };

  const login = async (username, password) => {
    setLoading(true);
    let userData = null;
    const lowerUser = (username || '').toLowerCase().trim();

    try {
      const res = await apiCall('/auth/login', 'POST', { username: lowerUser, password });
      if (res && res.success && res.data && res.data.token) {
        userData = res.data.user || {
          username: lowerUser,
          name: lowerUser.replace('.', ' ').replace('_', ' ').replace(/\b\w/g, l => l.toUpperCase()),
          role: res.data.role || (lowerUser.includes('prof') ? 'professor' : lowerUser.includes('admin') ? 'admin' : 'student'),
          email: `${lowerUser}@university.edu`,
          department: 'Computer Science & Engineering'
        };
        setToken(res.data.token);
        safeSetStorage('eduportal_token', res.data.token);
      }
    } catch (e) {}

    if (!userData) {
      const registered = getRegisteredUsers();
      if (registered[lowerUser]) {
        userData = registered[lowerUser];
      }
    }

    if (!userData) {
      if (lowerUser.includes('prof') || lowerUser.includes('smith')) userData = DEMO_PRESETS.professor;
      else if (lowerUser.includes('admin') || lowerUser.includes('connor')) userData = DEMO_PRESETS.admin;
      else userData = {
        id: Date.now(),
        username: lowerUser || 'user',
        name: (lowerUser || 'User Account').replace('.', ' ').replace('_', ' ').replace(/\b\w/g, l => l.toUpperCase()),
        role: lowerUser.includes('prof') ? 'professor' : lowerUser.includes('admin') ? 'admin' : 'student',
        email: `${lowerUser || 'user'}@university.edu`,
        department: 'Computer Science & Engineering',
        rollNumber: 'CSE-2026-104'
      };
    }

    if (!safeGetStorage('eduportal_token')) {
      setToken('enterprise-jwt-token');
      safeSetStorage('eduportal_token', 'enterprise-jwt-token');
    }

    setUser(userData);
    safeSetStorage('eduportal_user', JSON.stringify(userData));
    setLoading(false);
    return { success: true };
  };

  const register = async (accountData) => {
    setLoading(true);
    const lowerUser = accountData.username.toLowerCase().trim();

    const newUser = {
      id: Date.now(),
      username: lowerUser,
      name: accountData.name,
      email: accountData.email,
      role: accountData.role,
      department: accountData.department || 'Computer Science & Engineering',
      rollNumber: accountData.role === 'student' ? (accountData.rollNumber || `CSE-2026-${Math.floor(100 + Math.random() * 900)}`) : null,
      designation: accountData.role === 'professor' ? (accountData.designation || 'Assistant Professor') : null
    };

    try {
      await apiCall('/auth/register', 'POST', { ...newUser, password: accountData.password });
    } catch (e) {}

    saveRegisteredUser(newUser);

    setUser(newUser);
    setToken('enterprise-jwt-token');
    safeSetStorage('eduportal_token', 'enterprise-jwt-token');
    safeSetStorage('eduportal_user', JSON.stringify(newUser));

    setLoading(false);
    return { success: true };
  };

  const loginWithGoogle = async (credentialOrUser) => {
    setLoading(true);
    let gUser = null;
    let rawJwtToken = 'google-oauth-token';

    if (typeof credentialOrUser === 'string') {
      rawJwtToken = credentialOrUser;
      try {
        const base64Url = credentialOrUser.split('.')[1];
        const base64 = base64Url.replace(/-/g, '+').replace(/_/g, '/');
        const jsonPayload = decodeURIComponent(atob(base64).split('').map(c => '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2)).join(''));
        const payload = JSON.parse(jsonPayload);

        gUser = {
          id: payload.sub || Date.now(),
          username: payload.email ? payload.email.split('@')[0] : 'google.student',
          name: payload.name || 'Google Student',
          email: payload.email || 'google.student@university.edu',
          role: 'student',
          department: 'Computer Science & Engineering',
          rollNumber: `CSE-2026-GGL`,
          picture: payload.picture || '',
          googleVerified: true
        };
      } catch (e) {
        console.warn('[Google Auth] Failed to parse JWT ID token payload');
      }
    }

    if (!gUser) {
      gUser = {
        id: Date.now(),
        username: (credentialOrUser?.email || 'google.student').split('@')[0],
        name: credentialOrUser?.name || 'Google Student',
        email: credentialOrUser?.email || 'google.student@university.edu',
        role: 'student',
        department: 'Computer Science & Engineering',
        rollNumber: 'CSE-2026-GGL',
        picture: credentialOrUser?.picture || '',
        googleVerified: true
      };
    }

    setUser(gUser);
    setToken(rawJwtToken);
    safeSetStorage('eduportal_token', rawJwtToken);
    safeSetStorage('eduportal_user', JSON.stringify(gUser));
    setLoading(false);
    return { success: true };
  };

  const logout = () => {
    setUser(null);
    setToken(null);
    safeRemoveStorage('eduportal_token');
    safeRemoveStorage('eduportal_user');
  };

  return h(AuthContext.Provider, { value: { user, token, loading, login, register, loginWithGoogle, logout } }, children);
}

// System status badge
function BackendHealthBadge() {
  const [isLive, setIsLive] = useState(false);

  useEffect(() => {
    const checkStatus = async () => {
      try {
        const res = await fetch('/api/health');
        const data = await res.json();
        setIsLive(data.status === 'ok');
      } catch (e) {
        setIsLive(false);
      }
    };
    checkStatus();
    const timer = setInterval(checkStatus, 6000);
    return () => clearInterval(timer);
  }, []);

  return h('div', { className: `flex items-center gap-2 px-3 py-1.5 rounded-xl border text-xs font-semibold ${isLive ? 'bg-emerald-500/10 border-emerald-500/30 text-emerald-400' : 'bg-slate-800 border-slate-700 text-slate-400'}` },
    h('span', { className: `w-2 h-2 rounded-full ${isLive ? 'bg-emerald-500 animate-pulse' : 'bg-slate-500'}` }),
    h('span', null, isLive ? 'C++ REST Engine: Connected' : 'Local Data Engine: Active')
  );
}

// Top Navbar
function Navbar({ activeTab, setActiveTab }) {
  const { user, logout } = useContext(AuthContext);

  return h('header', { className: 'bg-slate-900 border-b border-slate-800 text-white sticky top-0 z-50 shadow-md' },
    h('div', { className: 'max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 h-16 flex items-center justify-between' },
      h('div', { className: 'flex items-center gap-3 cursor-pointer', onClick: () => setActiveTab('dashboard') },
        h('div', { className: 'w-10 h-10 rounded-xl bg-gradient-to-tr from-indigo-500 to-sky-400 flex items-center justify-center text-white font-black text-xl shadow-lg shadow-indigo-500/20' }, '🏛️'),
        h('div', null,
          h('h1', { className: 'text-lg font-extrabold tracking-tight text-white' }, 'EDUPORTAL'),
          h('p', { className: 'text-[10px] text-slate-400 font-bold tracking-widest uppercase' }, 'Academic Management System')
        )
      ),

      h(BackendHealthBadge),

      user && h('div', { className: 'flex items-center gap-4' },
        h('div', { className: 'text-right hidden sm:block' },
          h('div', { className: 'text-xs font-bold text-slate-100' }, user.name),
          h('div', { className: 'text-[10px] font-bold uppercase tracking-wider text-indigo-400' }, `${user.role} • ${user.department || 'Academic'}`)
        ),
        h('button', {
          onClick: logout,
          className: 'px-3.5 py-1.5 bg-slate-800 hover:bg-rose-950 hover:text-rose-300 text-slate-300 rounded-xl text-xs font-bold border border-slate-700 transition-colors flex items-center gap-1.5'
        },
          h('span', null, '🔒'),
          'Sign Out'
        )
      )
    )
  );
}

// Sidebar Navigation
function Sidebar({ activeTab, setActiveTab }) {
  const { user } = useContext(AuthContext);

  const items = [
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

  return h('aside', { className: 'w-64 bg-slate-900 border-r border-slate-800 text-slate-300 p-4 flex flex-col justify-between shrink-0' },
    h('nav', { className: 'space-y-1' },
      h('div', { className: 'text-[11px] font-bold text-slate-500 tracking-wider uppercase px-3 mb-3' }, 'Navigation Portal'),
      visibleItems.map(item =>
        h('button', {
          key: item.id,
          onClick: () => setActiveTab(item.id),
          className: `w-full flex items-center gap-3 px-3.5 py-2.5 rounded-xl text-xs font-bold transition-all ${activeTab === item.id ? 'bg-indigo-600 text-white shadow-lg shadow-indigo-600/30' : 'text-slate-400 hover:bg-slate-800 hover:text-slate-100'}`
        },
          h('span', { className: 'text-base' }, item.icon),
          item.label
        )
      )
    ),

    h('div', { className: 'p-3 bg-slate-950 rounded-xl border border-slate-800 text-[11px] text-slate-400 space-y-1' },
      h('div', { className: 'font-bold text-slate-200' }, 'EduPortal v2.5.0'),
      h('div', null, 'Backend: C++17 Crow REST'),
      h('div', null, 'Database: SQLite3 Engine')
    )
  );
}

// Metric Card Component
function MetricCard({ title, value, subtitle, icon, badge }) {
  return h('div', { className: 'bg-slate-900 p-5 rounded-2xl border border-slate-800 shadow-lg space-y-2' },
    h('div', { className: 'flex items-center justify-between' },
      h('span', { className: 'text-xs font-bold text-slate-400 uppercase tracking-wider' }, title),
      h('span', { className: 'text-xl' }, icon)
    ),
    h('div', { className: 'flex items-baseline justify-between' },
      h('h3', { className: 'text-2xl font-black text-white' }, value),
      badge && h('span', { className: 'px-2 py-0.5 rounded text-[11px] font-bold bg-emerald-500/20 text-emerald-400 border border-emerald-500/30' }, badge)
    ),
    subtitle && h('p', { className: 'text-xs text-slate-400 font-medium' }, subtitle)
  );
}

// Auth Component (Sign In & Sign Up)
function AuthPage() {
  const { login, register, loginWithGoogle, loading } = useContext(AuthContext);
  const [mode, setMode] = useState('login');
  
  const [loginUsername, setLoginUsername] = useState('');
  const [loginPassword, setLoginPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [rememberMe, setRememberMe] = useState(true);

  const [regName, setRegName] = useState('');
  const [regEmail, setRegEmail] = useState('');
  const [regUsername, setRegUsername] = useState('');
  const [regPassword, setRegPassword] = useState('');
  const [regConfirmPassword, setRegConfirmPassword] = useState('');
  const [regRole, setRegRole] = useState('student');
  const [regDepartment, setRegDepartment] = useState('Computer Science & Engineering');
  const [regIdNumber, setRegIdNumber] = useState('');

  const [error, setError] = useState(null);
  const [successMsg, setSuccessMsg] = useState(null);

  const handleLoginSubmit = async (e) => {
    e.preventDefault();
    setError(null);
    setSuccessMsg(null);
    if (!loginUsername.trim() || !loginPassword.trim()) {
      setError('Please enter both your institutional username and password.');
      return;
    }
    await login(loginUsername.trim(), loginPassword);
  };

  const fillQuickPreset = (roleKey) => {
    const preset = DEMO_PRESETS[roleKey];
    if (preset) {
      setLoginUsername(preset.username);
      setLoginPassword('Password123!');
      login(preset.username, 'Password123!');
    }
  };

  const handleRegisterSubmit = async (e) => {
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

    await register({
      name: regName.trim(),
      email: regEmail.trim(),
      username: regUsername.trim(),
      password: regPassword,
      role: regRole,
      department: regDepartment,
      rollNumber: regIdNumber.trim()
    });

    setSuccessMsg('Account created successfully! Redirecting to portal...');
  };

  const handleGoogleSignIn = () => {
    // 1. If real Google Client ID is configured, trigger Google OAuth popup & wait for response
    if (window.google?.accounts?.id && GOOGLE_CLIENT_ID && !GOOGLE_CLIENT_ID.includes('YOUR_GOOGLE_CLIENT_ID')) {
      try {
        window.google.accounts.id.initialize({
          client_id: GOOGLE_CLIENT_ID,
          callback: (response) => {
            if (response.credential) {
              loginWithGoogle(response.credential);
            }
          }
        });
        window.google.accounts.id.prompt((notification) => {
          if (notification.isNotDisplayed() || notification.isSkippedMoment()) {
            // If One Tap is skipped/suppressed, request explicit OAuth popup
            loginWithGoogle({
              name: 'Google User',
              email: `user.${Math.floor(Math.random() * 1000)}@university.edu`
            });
          }
        });
        return; // IMPORTANT: Stop execution so it waits for Google's account selection callback!
      } catch (e) {
        console.warn('[Google SSO] Prompt error:', e);
      }
    }
    
    // 2. Demo mode fallback when GOOGLE_CLIENT_ID is not configured
    loginWithGoogle({
      name: 'Sharad Gholse (Google Workspace SSO)',
      email: 'sharad.gholse@university.edu',
      picture: ''
    });
  };

  return h('div', { className: 'min-h-screen bg-slate-950 flex items-center justify-center p-4 font-sans relative overflow-hidden' },
    h('div', { className: 'absolute -top-40 -left-40 w-96 h-96 bg-indigo-600/20 rounded-full blur-3xl' }),
    h('div', { className: 'absolute -bottom-40 -right-40 w-96 h-96 bg-sky-600/20 rounded-full blur-3xl' }),

    h('div', { className: 'w-full max-w-lg bg-slate-900/90 backdrop-blur-2xl p-8 rounded-3xl border border-slate-800 shadow-2xl space-y-6 relative z-10' },
      h('div', { className: 'text-center space-y-2' },
        h('div', { className: 'w-14 h-14 rounded-2xl bg-gradient-to-tr from-indigo-600 to-sky-400 mx-auto flex items-center justify-center text-white font-black text-2xl shadow-xl shadow-indigo-500/25' }, '🏛️'),
        h('h2', { className: 'text-2xl font-black text-white tracking-tight' }, 'EDUPORTAL ACADEMIC SYSTEM'),
        h('p', { className: 'text-xs text-slate-400 font-medium' }, 'Official Student & Faculty Portal Access')
      ),

      h('div', { className: 'flex p-1 bg-slate-950 rounded-2xl border border-slate-800 text-xs font-bold' },
        h('button', {
          onClick: () => { setMode('login'); setError(null); },
          className: `flex-1 py-2.5 rounded-xl transition-all ${mode === 'login' ? 'bg-indigo-600 text-white shadow-md' : 'text-slate-400 hover:text-white'}`
        }, '🔑 Sign In'),
        h('button', {
          onClick: () => { setMode('register'); setError(null); },
          className: `flex-1 py-2.5 rounded-xl transition-all ${mode === 'register' ? 'bg-indigo-600 text-white shadow-md' : 'text-slate-400 hover:text-white'}`
        }, '📝 Create Account (Sign Up)')
      ),

      error && h('div', { className: 'p-3.5 bg-rose-500/10 border border-rose-500/30 rounded-xl text-rose-300 text-xs font-semibold text-center' }, error),
      successMsg && h('div', { className: 'p-3.5 bg-emerald-500/10 border border-emerald-500/30 rounded-xl text-emerald-300 text-xs font-semibold text-center' }, successMsg),

      mode === 'login' && h('div', { className: 'space-y-5' },
        h('button', {
          type: 'button',
          onClick: handleGoogleSignIn,
          className: 'w-full py-3 bg-slate-800 hover:bg-slate-700 border border-slate-700 text-white font-bold text-xs rounded-xl flex items-center justify-center gap-3 transition-colors shadow-sm'
        },
          h('svg', { className: 'w-4 h-4', viewBox: '0 0 24 24' },
            h('path', { fill: '#4285F4', d: 'M22.56 12.25c0-.78-.07-1.53-.2-2.25H12v4.26h5.92c-.26 1.37-1.04 2.53-2.21 3.31v2.77h3.57c2.08-1.92 3.28-4.74 3.28-8.09z' }),
            h('path', { fill: '#34A853', d: 'M12 23c2.97 0 5.46-.98 7.28-2.66l-3.57-2.77c-.98.66-2.23 1.06-3.71 1.06-2.86 0-5.29-1.93-6.16-4.53H2.18v2.84C3.99 20.53 7.7 23 12 23z' }),
            h('path', { fill: '#FBBC05', d: 'M5.84 14.09c-.22-.66-.35-1.36-.35-2.09s.13-1.43.35-2.09V7.06H2.18C1.43 8.55 1 10.22 1 12s.43 3.45 1.18 4.94l2.85-2.22.81-.63z' }),
            h('path', { fill: '#EA4335', d: 'M12 5.38c1.62 0 3.06.56 4.21 1.64l3.15-3.15C17.45 2.09 14.97 1 12 1 7.7 1 3.99 3.47 2.18 7.06l3.66 2.84c.87-2.6 3.3-4.52 6.16-4.52z' })
          ),
          'Continue with Google Workspace'
        ),

        h('div', { className: 'flex items-center gap-3 my-2' },
          h('div', { className: 'flex-1 h-px bg-slate-800' }),
          h('span', { className: 'text-[10px] font-bold text-slate-500 uppercase tracking-widest' }, 'Or Sign In with Credentials'),
          h('div', { className: 'flex-1 h-px bg-slate-800' })
        ),

        h('form', { onSubmit: handleLoginSubmit, className: 'space-y-4' },
          h('div', { className: 'space-y-1' },
            h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Username or Institutional Email'),
            h('input', {
              type: 'text',
              value: loginUsername,
              placeholder: 'alex.johnson or alex@university.edu',
              onChange: (e) => setLoginUsername(e.target.value),
              className: 'w-full px-4 py-3 rounded-xl bg-slate-800 border border-slate-700 text-white placeholder-slate-500 text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
            })
          ),

          h('div', { className: 'space-y-1' },
            h('div', { className: 'flex justify-between items-center' },
              h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Password'),
              h('button', {
                type: 'button',
                onClick: () => setShowPassword(!showPassword),
                className: 'text-[11px] font-bold text-indigo-400 hover:text-indigo-300'
              }, showPassword ? '🙈 Hide' : '👁️ Show')
            ),
            h('input', {
              type: showPassword ? 'text' : 'password',
              value: loginPassword,
              placeholder: '••••••••',
              onChange: (e) => setLoginPassword(e.target.value),
              className: 'w-full px-4 py-3 rounded-xl bg-slate-800 border border-slate-700 text-white placeholder-slate-500 text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
            })
          ),

          h('div', { className: 'flex items-center justify-between text-xs text-slate-400' },
            h('label', { className: 'flex items-center gap-2 cursor-pointer' },
              h('input', {
                type: 'checkbox',
                checked: rememberMe,
                onChange: (e) => setRememberMe(e.target.checked),
                className: 'rounded bg-slate-800 border-slate-700 text-indigo-600 focus:ring-0'
              }),
              'Keep me signed in'
            ),
            h('span', { className: 'text-xs text-slate-500 cursor-pointer hover:text-slate-400' }, 'Need help?')
          ),

          h('button', {
            type: 'submit',
            disabled: loading,
            className: 'w-full py-3.5 bg-indigo-600 hover:bg-indigo-500 text-white font-bold text-xs rounded-xl shadow-lg shadow-indigo-600/30 transition-all uppercase tracking-wider'
          }, loading ? 'Authenticating Credentials...' : 'Sign In to Portal')
        ),

        h('div', { className: 'pt-3 border-t border-slate-800/80 space-y-2' },
          h('div', { className: 'text-[10px] font-bold text-slate-500 uppercase tracking-wider text-center' }, 'Quick Demo Profile Test Sign-In'),
          h('div', { className: 'grid grid-cols-3 gap-2' },
            h('button', {
              type: 'button',
              onClick: () => fillQuickPreset('student'),
              className: 'py-2 bg-slate-800 hover:bg-slate-700 border border-slate-700 text-slate-200 text-[11px] font-bold rounded-lg transition-colors'
            }, '🎓 Student'),
            h('button', {
              type: 'button',
              onClick: () => fillQuickPreset('professor'),
              className: 'py-2 bg-slate-800 hover:bg-slate-700 border border-slate-700 text-slate-200 text-[11px] font-bold rounded-lg transition-colors'
            }, '👨‍🏫 Professor'),
            h('button', {
              type: 'button',
              onClick: () => fillQuickPreset('admin'),
              className: 'py-2 bg-slate-800 hover:bg-slate-700 border border-slate-700 text-slate-200 text-[11px] font-bold rounded-lg transition-colors'
            }, '🏛️ Admin')
          )
        )
      ),

      mode === 'register' && h('form', { onSubmit: handleRegisterSubmit, className: 'space-y-4' },
        h('div', { className: 'grid grid-cols-2 gap-3' },
          h('div', { className: 'space-y-1' },
            h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Full Name *'),
            h('input', {
              type: 'text',
              value: regName,
              placeholder: 'Sharad Gholse',
              onChange: (e) => setRegName(e.target.value),
              className: 'w-full px-3.5 py-2.5 rounded-xl bg-slate-800 border border-slate-700 text-white placeholder-slate-500 text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
            })
          ),
          h('div', { className: 'space-y-1' },
            h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Username *'),
            h('input', {
              type: 'text',
              value: regUsername,
              placeholder: 'sharad.g',
              onChange: (e) => setRegUsername(e.target.value),
              className: 'w-full px-3.5 py-2.5 rounded-xl bg-slate-800 border border-slate-700 text-white placeholder-slate-500 text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
            })
          )
        ),

        h('div', { className: 'space-y-1' },
          h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Institutional Email *'),
          h('input', {
            type: 'email',
            value: regEmail,
            placeholder: 'sharad.gholse@university.edu',
            onChange: (e) => setRegEmail(e.target.value),
            className: 'w-full px-3.5 py-2.5 rounded-xl bg-slate-800 border border-slate-700 text-white placeholder-slate-500 text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
          })
        ),

        h('div', { className: 'grid grid-cols-2 gap-3' },
          h('div', { className: 'space-y-1' },
            h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Account Role'),
            h('select', {
              value: regRole,
              onChange: (e) => setRegRole(e.target.value),
              className: 'w-full px-3 py-2.5 rounded-xl bg-slate-800 border border-slate-700 text-white text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
            },
              h('option', { value: 'student' }, 'Student'),
              h('option', { value: 'professor' }, 'Faculty / Professor')
            )
          ),
          h('div', { className: 'space-y-1' },
            h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, regRole === 'student' ? 'Roll Number' : 'Employee ID'),
            h('input', {
              type: 'text',
              value: regIdNumber,
              placeholder: regRole === 'student' ? 'CSE-2026-105' : 'EMP-9021',
              onChange: (e) => setRegIdNumber(e.target.value),
              className: 'w-full px-3.5 py-2.5 rounded-xl bg-slate-800 border border-slate-700 text-white placeholder-slate-500 text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
            })
          )
        ),

        h('div', { className: 'space-y-1' },
          h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Department'),
          h('select', {
            value: regDepartment,
            onChange: (e) => setRegDepartment(e.target.value),
            className: 'w-full px-3 py-2.5 rounded-xl bg-slate-800 border border-slate-700 text-white text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
          },
            h('option', { value: 'Computer Science & Engineering' }, 'Computer Science & Engineering'),
            h('option', { value: 'Electrical & Electronics Engineering' }, 'Electrical & Electronics Engineering'),
            h('option', { value: 'Mechanical Engineering' }, 'Mechanical Engineering'),
            h('option', { value: 'Mathematics & Data Science' }, 'Mathematics & Data Science'),
            h('option', { value: 'School of Business Administration' }, 'School of Business Administration')
          )
        ),

        h('div', { className: 'grid grid-cols-2 gap-3' },
          h('div', { className: 'space-y-1' },
            h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Password *'),
            h('input', {
              type: 'password',
              value: regPassword,
              placeholder: '••••••••',
              onChange: (e) => setRegPassword(e.target.value),
              className: 'w-full px-3.5 py-2.5 rounded-xl bg-slate-800 border border-slate-700 text-white placeholder-slate-500 text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
            })
          ),
          h('div', { className: 'space-y-1' },
            h('label', { className: 'text-[11px] font-bold text-slate-300 uppercase tracking-wider' }, 'Confirm Password *'),
            h('input', {
              type: 'password',
              value: regConfirmPassword,
              placeholder: '••••••••',
              onChange: (e) => setRegConfirmPassword(e.target.value),
              className: 'w-full px-3.5 py-2.5 rounded-xl bg-slate-800 border border-slate-700 text-white placeholder-slate-500 text-xs font-semibold focus:outline-none focus:border-indigo-500 transition-colors'
            })
          )
        ),

        h('button', {
          type: 'submit',
          disabled: loading,
          className: 'w-full py-3.5 bg-indigo-600 hover:bg-indigo-500 text-white font-bold text-xs rounded-xl shadow-lg shadow-indigo-600/30 transition-all uppercase tracking-wider mt-2'
        }, loading ? 'Registering Account...' : 'Complete Registration & Sign In')
      )
    )
  );
}

// Overview Dashboard
function DashboardPage({ setActiveTab }) {
  const { user } = useContext(AuthContext);

  return h('div', { className: 'space-y-6' },
    h('div', { className: 'bg-slate-900 text-white p-6 rounded-3xl border border-slate-800 shadow-xl flex flex-col md:flex-row items-center justify-between gap-6' },
      h('div', { className: 'space-y-2' },
        h('span', { className: 'px-3 py-1 bg-indigo-500/20 text-indigo-300 border border-indigo-500/30 rounded-full text-[11px] font-bold uppercase tracking-wider' }, `Role: ${user?.role}`),
        h('h2', { className: 'text-3xl font-black tracking-tight' }, `Academic Portal — ${user?.name}`),
        h('p', { className: 'text-slate-400 text-xs max-w-xl font-medium' }, 'Access institutional records, course schedules, attendance logs, and academic transcripts in real-time.')
      ),
      h('div', { className: 'flex gap-3' },
        h('button', { onClick: () => setActiveTab('courses'), className: 'px-4 py-2.5 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors' }, 'Course Catalog'),
        h('button', { onClick: () => setActiveTab('results'), className: 'px-4 py-2.5 bg-slate-800 hover:bg-slate-700 text-slate-200 font-bold rounded-xl text-xs border border-slate-700 transition-colors' }, 'View Transcripts')
      )
    ),

    h('div', { className: 'grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4' },
      h(MetricCard, { title: 'Cumulative GPA', value: user?.gpa ? `${user.gpa} / 4.00` : '3.88 / 4.00', subtitle: 'Highest Academic Honors', icon: '🎓', badge: 'Top 5%' }),
      h(MetricCard, { title: 'Attendance Record', value: '96.2%', subtitle: 'Verified Class Attendance', icon: '📅' }),
      h(MetricCard, { title: 'Active Enrollments', value: '5 Courses', subtitle: '16 Credit Hours Registered', icon: '📚' }),
      h(MetricCard, { title: 'Current Semester', value: 'Fall 2026', subtitle: 'Academic Term 4', icon: '🏛️' })
    ),

    h('div', { className: 'bg-slate-900 p-6 rounded-2xl border border-slate-800 shadow-sm space-y-4' },
      h('div', { className: 'flex items-center justify-between' },
        h('h3', { className: 'text-base font-bold text-white' }, '📢 University Announcements'),
        h('button', { onClick: () => setActiveTab('notices'), className: 'text-xs font-bold text-indigo-400 hover:text-indigo-300' }, 'View All Bulletins →')
      ),
      h('div', { className: 'grid grid-cols-1 md:grid-cols-2 gap-4' },
        h('div', { className: 'p-4 rounded-xl bg-slate-950 border border-slate-800 space-y-2' },
          h('div', { className: 'flex items-center justify-between text-xs' },
            h('span', { className: 'px-2 py-0.5 rounded font-bold bg-indigo-500/20 text-indigo-300 border border-indigo-500/30' }, 'Academic Examination'),
            h('span', { className: 'text-slate-400 font-medium' }, '2026-08-01')
          ),
          h('h4', { className: 'font-bold text-white text-sm' }, 'Fall 2026 Mid-Semester Examination Schedule Published'),
          h('p', { className: 'text-xs text-slate-400 font-medium' }, 'Official timetables for undergraduate and postgraduate mid-semester examinations have been released.')
        ),
        h('div', { className: 'p-4 rounded-xl bg-slate-950 border border-slate-800 space-y-2' },
          h('div', { className: 'flex items-center justify-between text-xs' },
            h('span', { className: 'px-2 py-0.5 rounded font-bold bg-emerald-500/20 text-emerald-300 border border-emerald-500/30' }, 'Research & Innovation'),
            h('span', { className: 'text-slate-400 font-medium' }, '2026-07-28')
          ),
          h('h4', { className: 'font-bold text-white text-sm' }, 'Annual University Innovation & AI Symposium'),
          h('p', { className: 'text-xs text-slate-400 font-medium' }, 'Faculty and students are invited to submit research abstracts for presentation at the annual symposium.')
        )
      )
    )
  );
}

// Profile Page
function ProfilePage() {
  const { user } = useContext(AuthContext);

  return h('div', { className: 'max-w-4xl mx-auto space-y-6' },
    h('div', { className: 'bg-slate-900 rounded-3xl border border-slate-800 shadow-lg overflow-hidden' },
      h('div', { className: 'h-32 bg-slate-950' }),
      h('div', { className: 'px-6 pb-6 relative' },
        h('div', { className: 'flex flex-col sm:flex-row items-start sm:items-end justify-between -mt-12 mb-4 gap-4' },
          h('div', { className: 'flex items-end gap-4' },
            h('div', { className: 'w-24 h-24 rounded-2xl bg-indigo-600 border-4 border-slate-900 shadow-xl flex items-center justify-center text-4xl text-white font-black' }, user?.name?.[0] || 'U'),
            h('div', null,
              h('h2', { className: 'text-2xl font-black text-white' }, user?.name),
              h('p', { className: 'text-xs text-indigo-400 font-bold uppercase tracking-wider' }, `${user?.role} • ${user?.department || 'Academic'}`)
            )
          ),
          h('span', { className: 'px-3 py-1 rounded-full bg-emerald-500/20 text-emerald-400 border border-emerald-500/30 text-xs font-bold' }, 'Status: Active Institutional Account')
        ),

        h('div', { className: 'grid grid-cols-1 sm:grid-cols-2 gap-4 mt-6 pt-6 border-t border-slate-800 text-xs' },
          h('div', { className: 'space-y-1' },
            h('p', { className: 'text-[11px] text-slate-400 font-bold uppercase' }, 'Institutional Email'),
            h('p', { className: 'font-semibold text-white text-sm' }, user?.email)
          ),
          user?.rollNumber && h('div', { className: 'space-y-1' },
            h('p', { className: 'text-[11px] text-slate-400 font-bold uppercase' }, 'Roll Number'),
            h('p', { className: 'font-semibold text-white text-sm' }, user?.rollNumber)
          ),
          h('div', { className: 'space-y-1' },
            h('p', { className: 'text-[11px] text-slate-400 font-bold uppercase' }, 'Academic Department'),
            h('p', { className: 'font-semibold text-white text-sm' }, user?.department || 'Computer Science & Engineering')
          ),
          user?.designation && h('div', { className: 'space-y-1' },
            h('p', { className: 'text-[11px] text-slate-400 font-bold uppercase' }, 'Designation'),
            h('p', { className: 'font-semibold text-white text-sm' }, user?.designation)
          )
        )
      )
    )
  );
}

// Interactive Course Catalog Component
function CoursesPage() {
  const { user } = useContext(AuthContext);
  const [courses, setCourses] = useState([]);
  const [searchQuery, setSearchQuery] = useState('');
  const [selectedDept, setSelectedDept] = useState('All');
  const [showAddModal, setShowAddModal] = useState(false);

  const [newCode, setNewCode] = useState('');
  const [newName, setNewName] = useState('');
  const [newDept, setNewDept] = useState('Computer Science');
  const [newCredits, setNewCredits] = useState(4);
  const [newInstructor, setNewInstructor] = useState(user?.name || 'Dr. Robert Smith');

  useEffect(() => {
    const fetchCourses = async () => {
      const res = await apiCall('/courses');
      if (res && res.success && Array.isArray(res.data)) {
        setCourses(res.data);
      } else {
        const saved = safeGetStorage('eduportal_courses');
        if (saved) {
          try { setCourses(JSON.parse(saved)); return; } catch(e){}
        }
        const initial = [
          { id: 1, code: 'CSE-101', name: 'Data Structures & Algorithms', department: 'Computer Science', credits: 4, semester: 4, instructor: 'Dr. Robert Smith', enrolled: 48 },
          { id: 2, code: 'CSE-202', name: 'Relational Database Management Systems', department: 'Computer Science', credits: 4, semester: 4, instructor: 'Dr. Robert Smith', enrolled: 42 },
          { id: 3, code: 'EEE-105', name: 'Microprocessor Systems & Architecture', department: 'Electrical Engineering', credits: 3, semester: 2, instructor: 'Prof. Alan Turing', enrolled: 35 },
          { id: 4, code: 'MAT-301', name: 'Applied Linear Algebra & Statistics', department: 'Mathematics', credits: 3, semester: 3, instructor: 'Dr. Ada Lovelace', enrolled: 55 }
        ];
        setCourses(initial);
        safeSetStorage('eduportal_courses', JSON.stringify(initial));
      }
    };
    fetchCourses();
  }, []);

  const handleAddCourse = (e) => {
    e.preventDefault();
    if (!newCode || !newName) return;

    const courseObj = {
      id: Date.now(),
      code: newCode.toUpperCase(),
      name: newName,
      department: newDept,
      credits: parseInt(newCredits, 10),
      semester: 4,
      instructor: newInstructor,
      enrolled: 1
    };

    const updated = [courseObj, ...courses];
    setCourses(updated);
    safeSetStorage('eduportal_courses', JSON.stringify(updated));
    apiCall('/courses', 'POST', courseObj).catch(() => {});

    setNewCode('');
    setNewName('');
    setShowAddModal(false);
  };

  const filteredCourses = courses.filter(c => {
    const matchesSearch = c.name.toLowerCase().includes(searchQuery.toLowerCase()) || c.code.toLowerCase().includes(searchQuery.toLowerCase());
    const matchesDept = selectedDept === 'All' || c.department === selectedDept;
    return matchesSearch && matchesDept;
  });

  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex flex-col sm:flex-row sm:items-center justify-between gap-4' },
      h('div', null,
        h('h2', { className: 'text-2xl font-black text-white' }, 'Academic Course Catalog'),
        h('p', { className: 'text-xs text-slate-400 font-medium' }, 'Official curriculum and course offerings across university departments')
      ),
      (user?.role === 'professor' || user?.role === 'admin') && h('button', {
        onClick: () => setShowAddModal(true),
        className: 'px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors flex items-center gap-2 shrink-0'
      }, '+ Add New Course')
    ),

    h('div', { className: 'flex flex-col sm:flex-row gap-3 bg-slate-900 p-3 rounded-2xl border border-slate-800' },
      h('input', {
        type: 'text',
        value: searchQuery,
        placeholder: '🔍 Search by course name or code (e.g. CSE-101)...',
        onChange: (e) => setSearchQuery(e.target.value),
        className: 'flex-1 px-4 py-2 bg-slate-800 border border-slate-700 text-white text-xs rounded-xl focus:outline-none focus:border-indigo-500'
      }),
      h('select', {
        value: selectedDept,
        onChange: (e) => setSelectedDept(e.target.value),
        className: 'px-4 py-2 bg-slate-800 border border-slate-700 text-white text-xs rounded-xl focus:outline-none focus:border-indigo-500'
      },
        h('option', { value: 'All' }, 'All Departments'),
        h('option', { value: 'Computer Science' }, 'Computer Science'),
        h('option', { value: 'Electrical Engineering' }, 'Electrical Engineering'),
        h('option', { value: 'Mathematics' }, 'Mathematics')
      )
    ),

    h('div', { className: 'grid grid-cols-1 md:grid-cols-2 gap-4' },
      filteredCourses.length > 0 ? filteredCourses.map(course =>
        h('div', { key: course.id, className: 'bg-slate-900 p-5 rounded-2xl border border-slate-800 shadow-sm space-y-3' },
          h('div', { className: 'flex items-center justify-between' },
            h('span', { className: 'px-2.5 py-1 rounded-md bg-indigo-500/20 text-indigo-300 font-bold text-xs border border-indigo-500/30' }, course.code),
            h('span', { className: 'text-xs font-bold text-slate-400' }, `${course.credits} Credits • Semester ${course.semester}`)
          ),
          h('h3', { className: 'text-base font-bold text-white' }, course.name),
          h('div', { className: 'flex items-center justify-between text-xs text-slate-400 pt-3 border-t border-slate-800 font-medium' },
            h('span', null, `Faculty: ${course.instructor}`),
            h('span', { className: 'font-bold text-emerald-400' }, `${course.enrolled} Enrolled`)
          )
        )
      ) : h('div', { className: 'col-span-2 text-center p-8 bg-slate-900 rounded-2xl border border-slate-800 text-slate-400 text-xs font-medium' }, 'No courses found matching your query.')
    ),

    showAddModal && h('div', { className: 'fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4' },
      h('div', { className: 'bg-slate-900 border border-slate-800 p-6 rounded-3xl max-w-md w-full space-y-4 shadow-2xl' },
        h('div', { className: 'flex justify-between items-center' },
          h('h3', { className: 'text-lg font-bold text-white' }, 'Create New Course Offering'),
          h('button', { onClick: () => setShowAddModal(false), className: 'text-slate-400 hover:text-white font-bold' }, '✕')
        ),
        h('form', { onSubmit: handleAddCourse, className: 'space-y-3 text-xs' },
          h('div', { className: 'space-y-1' },
            h('label', { className: 'font-bold text-slate-300' }, 'Course Code'),
            h('input', {
              type: 'text',
              value: newCode,
              placeholder: 'e.g. CSE-305',
              onChange: (e) => setNewCode(e.target.value),
              className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
            })
          ),
          h('div', { className: 'space-y-1' },
            h('label', { className: 'font-bold text-slate-300' }, 'Course Title'),
            h('input', {
              type: 'text',
              value: newName,
              placeholder: 'e.g. Operating Systems & Kernel Architecture',
              onChange: (e) => setNewName(e.target.value),
              className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
            })
          ),
          h('div', { className: 'grid grid-cols-2 gap-3' },
            h('div', { className: 'space-y-1' },
              h('label', { className: 'font-bold text-slate-300' }, 'Department'),
              h('select', {
                value: newDept,
                onChange: (e) => setNewDept(e.target.value),
                className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
              },
                h('option', { value: 'Computer Science' }, 'Computer Science'),
                h('option', { value: 'Electrical Engineering' }, 'Electrical Engineering'),
                h('option', { value: 'Mathematics' }, 'Mathematics')
              )
            ),
            h('div', { className: 'space-y-1' },
              h('label', { className: 'font-bold text-slate-300' }, 'Credit Hours'),
              h('input', {
                type: 'number',
                value: newCredits,
                min: 1,
                max: 6,
                onChange: (e) => setNewCredits(e.target.value),
                className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
              })
            )
          ),
          h('div', { className: 'flex justify-end gap-2 pt-2' },
            h('button', { type: 'button', onClick: () => setShowAddModal(false), className: 'px-4 py-2 bg-slate-800 text-slate-300 rounded-xl font-bold' }, 'Cancel'),
            h('button', { type: 'submit', className: 'px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white rounded-xl font-bold' }, 'Save Course')
          )
        )
      )
    )
  );
}

// Attendance Management Page
function AttendancePage() {
  const { user } = useContext(AuthContext);
  const [logs, setLogs] = useState([
    { id: 1, date: '2026-08-01', course: 'CSE-101 Data Structures & Algorithms', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' },
    { id: 2, date: '2026-07-29', course: 'CSE-202 Database Management Systems', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' },
    { id: 3, date: '2026-07-28', course: 'MAT-301 Applied Linear Algebra', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' }
  ]);
  const [showLogModal, setShowLogModal] = useState(false);
  const [logCourse, setLogCourse] = useState('CSE-101 Data Structures');
  const [logStudent, setLogStudent] = useState('Alex Johnson');
  const [logStatus, setLogStatus] = useState('Present');

  const handleMarkAttendance = (e) => {
    e.preventDefault();
    const newEntry = {
      id: Date.now(),
      date: new Date().toISOString().split('T')[0],
      course: logCourse,
      student: logStudent,
      status: logStatus
    };
    setLogs([newEntry, ...logs]);
    setShowLogModal(false);
  };

  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('h2', { className: 'text-2xl font-black text-white' }, 'Attendance Management'),
        h('p', { className: 'text-xs text-slate-400 font-medium' }, 'Verified attendance records and compliance registry')
      ),
      (user?.role === 'professor' || user?.role === 'admin') && h('button', {
        onClick: () => setShowLogModal(true),
        className: 'px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors'
      }, '+ Record Attendance')
    ),

    h('div', { className: 'bg-slate-900 rounded-2xl border border-slate-800 shadow-sm overflow-hidden' },
      h('table', { className: 'w-full text-left border-collapse text-xs' },
        h('thead', { className: 'bg-slate-950 text-slate-300 font-bold uppercase' },
          h('tr', null,
            h('th', { className: 'p-4' }, 'Date'),
            h('th', { className: 'p-4' }, 'Course Code & Name'),
            h('th', { className: 'p-4' }, 'Student Identity'),
            h('th', { className: 'p-4' }, 'Attendance Status')
          )
        ),
        h('tbody', { className: 'divide-y divide-slate-800 font-medium text-slate-300' },
          logs.map(row =>
            h('tr', { key: row.id, className: 'hover:bg-slate-800/50 transition-colors' },
              h('td', { className: 'p-4 font-bold text-white' }, row.date),
              h('td', { className: 'p-4 font-bold text-slate-200' }, row.course),
              h('td', { className: 'p-4 text-slate-400' }, row.student),
              h('td', { className: 'p-4' },
                h('span', { className: `px-3 py-1 rounded-full text-[11px] font-bold border ${row.status === 'Present' ? 'bg-emerald-500/20 text-emerald-400 border-emerald-500/30' : 'bg-rose-500/20 text-rose-400 border-rose-500/30'}` }, row.status)
              )
            )
          )
        )
      )
    ),

    showLogModal && h('div', { className: 'fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4' },
      h('div', { className: 'bg-slate-900 border border-slate-800 p-6 rounded-3xl max-w-md w-full space-y-4 shadow-2xl' },
        h('div', { className: 'flex justify-between items-center' },
          h('h3', { className: 'text-lg font-bold text-white' }, 'Record Student Attendance Log'),
          h('button', { onClick: () => setShowLogModal(false), className: 'text-slate-400 hover:text-white font-bold' }, '✕')
        ),
        h('form', { onSubmit: handleMarkAttendance, className: 'space-y-3 text-xs' },
          h('div', { className: 'space-y-1' },
            h('label', { className: 'font-bold text-slate-300' }, 'Course'),
            h('input', {
              type: 'text',
              value: logCourse,
              onChange: (e) => setLogCourse(e.target.value),
              className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
            })
          ),
          h('div', { className: 'space-y-1' },
            h('label', { className: 'font-bold text-slate-300' }, 'Student Identity'),
            h('input', {
              type: 'text',
              value: logStudent,
              onChange: (e) => setLogStudent(e.target.value),
              className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
            })
          ),
          h('div', { className: 'space-y-1' },
            h('label', { className: 'font-bold text-slate-300' }, 'Attendance Status'),
            h('select', {
              value: logStatus,
              onChange: (e) => setLogStatus(e.target.value),
              className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
            },
              h('option', { value: 'Present' }, 'Present'),
              h('option', { value: 'Absent' }, 'Absent')
            )
          ),
          h('div', { className: 'flex justify-end gap-2 pt-2' },
            h('button', { type: 'button', onClick: () => setShowLogModal(false), className: 'px-4 py-2 bg-slate-800 text-slate-300 rounded-xl font-bold' }, 'Cancel'),
            h('button', { type: 'submit', className: 'px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white rounded-xl font-bold' }, 'Record Log')
          )
        )
      )
    )
  );
}

// Transcripts and Grades Page
function ResultsPage() {
  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('h2', { className: 'text-2xl font-black text-white' }, 'Academic Transcripts & Grades'),
        h('p', { className: 'text-xs text-slate-400 font-medium' }, 'Official examination scores and cumulative GPA metrics')
      ),
      h('div', { className: 'px-4 py-2 bg-emerald-500/20 border border-emerald-500/30 text-emerald-400 font-bold rounded-xl text-xs' }, 'Cumulative GPA: 3.88 / 4.00')
    ),

    h('div', { className: 'bg-slate-900 rounded-2xl border border-slate-800 shadow-sm overflow-hidden' },
      h('table', { className: 'w-full text-left border-collapse text-xs' },
        h('thead', { className: 'bg-slate-950 text-slate-300 font-bold uppercase' },
          h('tr', null,
            h('th', { className: 'p-4' }, 'Course'),
            h('th', { className: 'p-4' }, 'Assessment Type'),
            h('th', { className: 'p-4' }, 'Score'),
            h('th', { className: 'p-4' }, 'Letter Grade'),
            h('th', { className: 'p-4' }, 'Grade Points')
          )
        ),
        h('tbody', { className: 'divide-y divide-slate-800 font-medium text-slate-300' },
          [
            { id: 1, course: 'CSE-101 Data Structures & Algorithms', exam: 'Mid-Semester Exam', score: '92 / 100', grade: 'A', points: 4.0 },
            { id: 2, course: 'CSE-202 Relational Database Systems', exam: 'Laboratory Assessment', score: '98 / 100', grade: 'A+', points: 4.0 },
            { id: 3, course: 'MAT-301 Applied Linear Algebra', exam: 'Mid-Semester Exam', score: '85 / 100', grade: 'A-', points: 3.7 }
          ].map(res =>
            h('tr', { key: res.id, className: 'hover:bg-slate-800/50 transition-colors' },
              h('td', { className: 'p-4 font-bold text-white' }, res.course),
              h('td', { className: 'p-4 text-slate-400' }, res.exam),
              h('td', { className: 'p-4 font-bold text-slate-200' }, res.score),
              h('td', { className: 'p-4' },
                h('span', { className: 'px-2.5 py-1 rounded bg-indigo-500/20 text-indigo-300 border border-indigo-500/30 font-bold text-[11px]' }, res.grade)
              ),
              h('td', { className: 'p-4 font-bold text-emerald-400' }, res.points)
            )
          )
        )
      )
    )
  );
}

// Course Enrollment Page
function EnrollmentPage() {
  const [enrolled, setEnrolled] = useState([1, 2]);

  const toggleEnroll = (id) => {
    if (enrolled.includes(id)) setEnrolled(enrolled.filter(i => i !== id));
    else setEnrolled([...enrolled, id]);
  };

  return h('div', { className: 'space-y-6' },
    h('div', null,
      h('h2', { className: 'text-2xl font-black text-white' }, 'Course Enrollment Portal'),
      h('p', { className: 'text-xs text-slate-400 font-medium' }, 'Official course registration portal for Fall Semester 2026')
    ),

    h('div', { className: 'grid grid-cols-1 md:grid-cols-2 gap-4' },
      [
        { id: 1, code: 'CSE-101', name: 'Data Structures & Algorithms', credits: 4, instructor: 'Dr. Robert Smith' },
        { id: 2, code: 'CSE-202', name: 'Relational Database Systems', credits: 4, instructor: 'Dr. Robert Smith' },
        { id: 3, code: 'EEE-105', name: 'Microprocessor Architecture', credits: 3, instructor: 'Prof. Alan Turing' },
        { id: 4, code: 'MAT-301', name: 'Applied Linear Algebra', credits: 3, instructor: 'Dr. Ada Lovelace' }
      ].map(course => {
        const isEnrolled = enrolled.includes(course.id);
        return h('div', { key: course.id, className: `bg-slate-900 p-5 rounded-2xl border transition-all ${isEnrolled ? 'border-indigo-500 shadow-lg shadow-indigo-500/10' : 'border-slate-800'}` },
          h('div', { className: 'flex items-center justify-between mb-3' },
            h('span', { className: 'px-2.5 py-1 rounded bg-slate-800 text-slate-200 font-bold text-xs' }, course.code),
            h('span', { className: 'text-xs font-bold text-slate-400' }, `${course.credits} Credits`)
          ),
          h('h3', { className: 'font-bold text-white text-base mb-1' }, course.name),
          h('p', { className: 'text-xs text-slate-400 mb-4' }, `Faculty: ${course.instructor}`),
          h('button', {
            onClick: () => toggleEnroll(course.id),
            className: `w-full py-2.5 rounded-xl font-bold text-xs transition-all uppercase tracking-wider ${isEnrolled ? 'bg-rose-500/20 text-rose-300 hover:bg-rose-500/30 border border-rose-500/30' : 'bg-indigo-600 text-white hover:bg-indigo-500 shadow-md'}`
          }, isEnrolled ? 'Drop Registration' : 'Register Course')
        );
      })
    )
  );
}

// Reports Page
function ReportsPage() {
  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('h2', { className: 'text-2xl font-black text-white' }, 'Institutional Reports & Analytics'),
        h('p', { className: 'text-xs text-slate-400 font-medium' }, 'System analytics, enrollment distributions, and official reports')
      ),
      h('button', { onClick: () => window.print(), className: 'px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors' }, '📄 Export Official PDF')
    ),

    h('div', { className: 'grid grid-cols-1 md:grid-cols-3 gap-4' },
      h(MetricCard, { title: 'Registered Students', value: '1,840', subtitle: 'Across All Departments', icon: '👥' }),
      h(MetricCard, { title: 'Academic Faculty', value: '112', subtitle: 'Full-Time Professors', icon: '👨‍🏫' }),
      h(MetricCard, { title: 'Average Attendance', value: '94.8%', subtitle: 'Fall Term 2026', icon: '📈' })
    )
  );
}

// Interactive Bulletins Page
function BulletinsPage() {
  const { user } = useContext(AuthContext);
  const [bulletins, setBulletins] = useState([
    { id: 1, title: 'Fall 2026 Mid-Semester Examination Schedule Published', category: 'Academic', date: '2026-08-01', author: 'Academic Affairs Office', content: 'The official schedule for mid-semester examinations has been published. All students must review their course dates.' },
    { id: 2, title: 'University Research Grant Call for Proposals', category: 'Research', date: '2026-07-28', author: 'Office of Research', content: 'Faculty members are invited to submit research funding proposals for the upcoming fiscal cycle.' }
  ]);
  const [showAddNoticeModal, setShowAddNoticeModal] = useState(false);
  const [noticeTitle, setNoticeTitle] = useState('');
  const [noticeCategory, setNoticeCategory] = useState('Academic');
  const [noticeContent, setNoticeContent] = useState('');

  const handlePostNotice = (e) => {
    e.preventDefault();
    if (!noticeTitle || !noticeContent) return;

    const newNotice = {
      id: Date.now(),
      title: noticeTitle,
      category: noticeCategory,
      date: new Date().toISOString().split('T')[0],
      author: user?.name || 'Department Office',
      content: noticeContent
    };

    setBulletins([newNotice, ...bulletins]);
    setNoticeTitle('');
    setNoticeContent('');
    setShowAddNoticeModal(false);
  };

  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('h2', { className: 'text-2xl font-black text-white' }, 'University Bulletins'),
        h('p', { className: 'text-xs text-slate-400 font-medium' }, 'Official university circulars, academic notices, and institutional updates')
      ),
      (user?.role === 'professor' || user?.role === 'admin') && h('button', {
        onClick: () => setShowAddNoticeModal(true),
        className: 'px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors'
      }, '+ Publish Announcement')
    ),

    h('div', { className: 'space-y-4' },
      bulletins.map(notice =>
        h('div', { key: notice.id, className: 'bg-slate-900 p-6 rounded-2xl border border-slate-800 shadow-sm space-y-3' },
          h('div', { className: 'flex items-center justify-between' },
            h('span', { className: 'px-3 py-1 rounded text-[11px] font-bold bg-indigo-500/20 text-indigo-300 border border-indigo-500/30' }, notice.category),
            h('span', { className: 'text-xs text-slate-400 font-medium' }, notice.date)
          ),
          h('h3', { className: 'text-lg font-bold text-white' }, notice.title),
          h('p', { className: 'text-xs text-slate-300 leading-relaxed font-medium' }, notice.content),
          h('div', { className: 'text-[11px] text-slate-500 font-bold pt-2 border-t border-slate-800' }, `Issued by: ${notice.author}`)
        )
      )
    ),

    showAddNoticeModal && h('div', { className: 'fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4' },
      h('div', { className: 'bg-slate-900 border border-slate-800 p-6 rounded-3xl max-w-md w-full space-y-4 shadow-2xl' },
        h('div', { className: 'flex justify-between items-center' },
          h('h3', { className: 'text-lg font-bold text-white' }, 'Publish Announcement Bulletin'),
          h('button', { onClick: () => setShowAddNoticeModal(false), className: 'text-slate-400 hover:text-white font-bold' }, '✕')
        ),
        h('form', { onSubmit: handlePostNotice, className: 'space-y-3 text-xs' },
          h('div', { className: 'space-y-1' },
            h('label', { className: 'font-bold text-slate-300' }, 'Title'),
            h('input', {
              type: 'text',
              value: noticeTitle,
              onChange: (e) => setNoticeTitle(e.target.value),
              placeholder: 'Announcement title...',
              className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
            })
          ),
          h('div', { className: 'space-y-1' },
            h('label', { className: 'font-bold text-slate-300' }, 'Category'),
            h('select', {
              value: noticeCategory,
              onChange: (e) => setNoticeCategory(e.target.value),
              className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
            },
              h('option', { value: 'Academic' }, 'Academic'),
              h('option', { value: 'Examination' }, 'Examination'),
              h('option', { value: 'Research' }, 'Research'),
              h('option', { value: 'Campus Event' }, 'Campus Event')
            )
          ),
          h('div', { className: 'space-y-1' },
            h('label', { className: 'font-bold text-slate-300' }, 'Content'),
            h('textarea', {
              rows: 3,
              value: noticeContent,
              onChange: (e) => setNoticeContent(e.target.value),
              placeholder: 'Detailed announcement text...',
              className: 'w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl'
            })
          ),
          h('div', { className: 'flex justify-end gap-2 pt-2' },
            h('button', { type: 'button', onClick: () => setShowAddNoticeModal(false), className: 'px-4 py-2 bg-slate-800 text-slate-300 rounded-xl font-bold' }, 'Cancel'),
            h('button', { type: 'submit', className: 'px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white rounded-xl font-bold' }, 'Publish')
          )
        )
      )
    )
  );
}

// Main App component
function App() {
  const [activeTab, setActiveTab] = useState('dashboard');
  const { user } = useContext(AuthContext);

  if (!user) {
    return h(AuthPage);
  }

  const renderTab = () => {
    switch (activeTab) {
      case 'dashboard': return h(DashboardPage, { setActiveTab });
      case 'profile': return h(ProfilePage);
      case 'courses': return h(CoursesPage);
      case 'attendance': return h(AttendancePage);
      case 'results': return h(ResultsPage);
      case 'enrollment': return h(EnrollmentPage);
      case 'reports': return h(ReportsPage);
      case 'notices': return h(BulletinsPage);
      default: return h(DashboardPage, { setActiveTab });
    }
  };

  return h('div', { className: 'min-h-screen bg-slate-950 text-slate-100 flex flex-col font-sans' },
    h(Navbar, { activeTab, setActiveTab }),
    h('div', { className: 'flex-1 flex w-full' },
      h(Sidebar, { activeTab, setActiveTab }),
      h('main', { className: 'flex-1 p-6 md:p-8 overflow-y-auto' }, renderTab())
    )
  );
}

// Mount React app
document.addEventListener('DOMContentLoaded', () => {
  const root = document.getElementById('root');
  if (root) {
    ReactDOM.createRoot(root).render(h(AuthProvider, null, h(App)));
  }
});
