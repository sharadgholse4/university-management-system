// EduPortal | University Management System Application Bundle
const h = React.createElement;
const { useState, useEffect, createContext, useContext } = React;

// --- API CLIENT ---
const API_BASE = '/api';

async function apiRequest(endpoint, method = 'GET', data = null, token = null) {
  const headers = { 'Content-Type': 'application/json' };
  if (token) headers['Authorization'] = `Bearer ${token}`;

  const options = { method, headers };
  if (data) options.body = JSON.stringify(data);

  try {
    const res = await fetch(`${API_BASE}${endpoint}`, options);
    const result = await res.json();
    return result;
  } catch (err) {
    console.warn(`[API] Endpoint ${endpoint} network call fallback:`, err.message);
    return { success: false, error: err.message };
  }
}

// --- MOCK DATA FOR DEMO MODE ---
const INITIAL_DEMO_USERS = {
  student: { id: 101, username: 'student_alex', role: 'student', name: 'Alex Johnson', email: 'alex@university.edu', department: 'Computer Science', rollNumber: 'CS-2024-042', semester: 4, gpa: 3.85 },
  professor: { id: 201, username: 'prof_smith', role: 'professor', name: 'Dr. Robert Smith', email: 'rsmith@university.edu', department: 'Computer Science', designation: 'Senior Professor' },
  admin: { id: 301, username: 'admin_dean', role: 'admin', name: 'Dean Sarah Connor', email: 'dean@university.edu', department: 'Administration', designation: 'Head Administrator' }
};

const INITIAL_COURSES = [
  { id: 1, code: 'CS-101', name: 'Data Structures & Algorithms', department: 'Computer Science', credits: 4, semester: 4, instructor: 'Dr. Robert Smith', enrolled: 45 },
  { id: 2, code: 'CS-202', name: 'Database Systems & SQL', department: 'Computer Science', credits: 3, semester: 4, instructor: 'Dr. Robert Smith', enrolled: 38 },
  { id: 3, code: 'EE-105', name: 'Digital Circuit Design', department: 'Electrical Engineering', credits: 4, semester: 2, instructor: 'Prof. Alan Turing', enrolled: 30 },
  { id: 4, code: 'MATH-301', name: 'Linear Algebra & Calculus', department: 'Mathematics', credits: 3, semester: 3, instructor: 'Dr. Ada Lovelace', enrolled: 52 }
];

const INITIAL_ATTENDANCE = [
  { id: 1, date: '2026-07-28', course: 'CS-101 Data Structures', status: 'Present', studentName: 'Alex Johnson' },
  { id: 2, date: '2026-07-29', course: 'CS-202 Database Systems', status: 'Present', studentName: 'Alex Johnson' },
  { id: 3, date: '2026-07-30', course: 'CS-101 Data Structures', status: 'Late', studentName: 'Alex Johnson' }
];

const INITIAL_RESULTS = [
  { id: 1, course: 'CS-101 Data Structures', exam: 'Midterm', marks: 88, maxMarks: 100, grade: 'A', points: 4.0 },
  { id: 2, course: 'CS-202 Database Systems', exam: 'Assignment 1', marks: 95, maxMarks: 100, grade: 'A+', points: 4.0 },
  { id: 3, course: 'MATH-301 Linear Algebra', exam: 'Midterm', marks: 78, maxMarks: 100, grade: 'B+', points: 3.3 }
];

const INITIAL_NOTICES = [
  { id: 1, title: 'Fall 2026 Mid-Semester Exam Schedule', date: '2026-07-29', category: 'Exam', author: 'Academic Office', content: 'Mid-semester examinations will commence from August 15th. Detailed timetable published.' },
  { id: 2, title: 'Annual University Hackathon Registration Open', date: '2026-07-28', category: 'Event', author: 'CS Department', content: 'Register your teams of 4 for the 48-hour coding hackathon. Prize pool $10,000!' }
];

// --- AUTH CONTEXT ---
const AuthContext = createContext(null);

function AuthProvider({ children }) {
  const [user, setUser] = useState(null); // Default to Login Page
  const [token, setToken] = useState(null);

  const switchRole = (roleKey) => {
    if (INITIAL_DEMO_USERS[roleKey]) {
      setUser(INITIAL_DEMO_USERS[roleKey]);
      setToken(`demo-${roleKey}-token`);
    }
  };

  const login = async (username, password) => {
    const res = await apiRequest('/auth/login', 'POST', { username, password });
    if (res.success && res.data) {
      setUser(res.data.user);
      setToken(res.data.token);
      return { success: true };
    }
    // Fallback demo login based on input username
    const lower = (username || '').toLowerCase();
    if (lower.includes('prof') || lower.includes('smith')) switchRole('professor');
    else if (lower.includes('admin') || lower.includes('dean')) switchRole('admin');
    else switchRole('student');
    return { success: true };
  };

  const logout = () => {
    setUser(null);
    setToken(null);
  };

  return h(AuthContext.Provider, { value: { user, token, switchRole, login, logout } }, children);
}

// --- COMPONENTS ---

// 1. Backend Status Badge
function BackendStatusBadge() {
  const [status, setStatus] = useState('checking');

  useEffect(() => {
    const checkHealth = async () => {
      try {
        const res = await fetch('/api/health');
        const data = await res.json();
        if (data.status === 'ok') setStatus('online');
        else setStatus('offline');
      } catch (err) {
        setStatus('offline');
      }
    };
    checkHealth();
    const interval = setInterval(checkHealth, 5000);
    return () => clearInterval(interval);
  }, []);

  const isOnline = status === 'online';

  return h('div', { className: `flex items-center gap-2 px-3 py-1.5 rounded-full text-xs font-semibold border ${isOnline ? 'bg-emerald-500/10 border-emerald-500/30 text-emerald-600' : 'bg-amber-500/10 border-amber-500/30 text-amber-600'}` },
    h('span', { className: `w-2 h-2 rounded-full ${isOnline ? 'bg-emerald-500 animate-pulse' : 'bg-amber-500'}` }),
    `C++ Crow Backend: ${isOnline ? 'Port 8080 (Online)' : 'Connecting...'}`
  );
}

// 2. Navbar
function Navbar({ activeTab, setActiveTab }) {
  const { user, switchRole, logout } = useContext(AuthContext);

  return h('header', { className: 'bg-white border-b border-slate-200 sticky top-0 z-50 shadow-sm' },
    h('div', { className: 'max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 h-16 flex items-center justify-between' },
      // Brand Logo
      h('div', { className: 'flex items-center gap-3 cursor-pointer', onClick: () => setActiveTab('dashboard') },
        h('div', { className: 'w-10 h-10 rounded-xl bg-gradient-to-tr from-indigo-600 to-sky-500 flex items-center justify-center text-white font-bold shadow-md shadow-indigo-500/20' }, '🎓'),
        h('div', null,
          h('h1', { className: 'text-lg font-bold bg-gradient-to-r from-slate-900 via-indigo-950 to-slate-800 bg-clip-text text-transparent' }, 'EduPortal'),
          h('p', { className: 'text-[10px] text-slate-400 font-medium tracking-wide uppercase' }, 'University Management System')
        )
      ),

      // Center Status Badge
      h(BackendStatusBadge),

      // Right Controls: Role Switcher & User Profile
      h('div', { className: 'flex items-center gap-4' },
        // Quick Demo Role Switcher
        h('div', { className: 'hidden md:flex items-center gap-1 bg-slate-100 p-1 rounded-xl border border-slate-200' },
          h('span', { className: 'text-xs text-slate-500 px-2 font-medium' }, 'Demo Role:'),
          ['student', 'professor', 'admin'].map((roleKey) =>
            h('button', {
              key: roleKey,
              onClick: () => switchRole(roleKey),
              className: `px-2.5 py-1 text-xs font-semibold rounded-lg capitalize transition-all ${user?.role === roleKey ? 'bg-indigo-600 text-white shadow-sm' : 'text-slate-600 hover:bg-slate-200'}`
            }, roleKey)
          )
        ),

        // User Avatar Badge & Sign Out Button
        user && h('div', { className: 'flex items-center gap-3 pl-3 border-l border-slate-200' },
          h('div', { className: 'text-right hidden sm:block' },
            h('div', { className: 'text-xs font-bold text-slate-800' }, user.name),
            h('div', { className: 'text-[10px] font-semibold uppercase text-indigo-600 tracking-wider' }, user.role)
          ),
          h('button', { onClick: logout, title: 'Sign Out to Login Page', className: 'px-3 py-1.5 bg-rose-50 text-rose-600 hover:bg-rose-100 rounded-xl text-xs font-bold transition-colors flex items-center gap-1' },
            '🚪 Sign Out'
          )
        )
      )
    )
  );
}

// 3. Sidebar Navigation
function Sidebar({ activeTab, setActiveTab }) {
  const { user } = useContext(AuthContext);

  const navItems = [
    { id: 'dashboard', label: 'Dashboard', icon: '📊', roles: ['student', 'professor', 'admin'] },
    { id: 'profile', label: 'My Profile', icon: '👤', roles: ['student', 'professor', 'admin'] },
    { id: 'courses', label: 'Course Catalog', icon: '📚', roles: ['student', 'professor', 'admin'] },
    { id: 'enrollment', label: 'Course Enrollment', icon: '📝', roles: ['student', 'admin'] },
    { id: 'attendance', label: 'Attendance Registry', icon: '📅', roles: ['student', 'professor', 'admin'] },
    { id: 'results', label: 'Exam Results & Transcripts', icon: '🎓', roles: ['student', 'professor', 'admin'] },
    { id: 'reports', label: 'Analytics & Reports', icon: '📈', roles: ['professor', 'admin'] },
    { id: 'notices', label: 'Notice Board', icon: '📢', roles: ['student', 'professor', 'admin'] }
  ];

  const filteredNav = navItems.filter(item => item.roles.includes(user?.role || 'student'));

  return h('aside', { className: 'w-64 bg-white border-r border-slate-200 min-h-[calc(100vh-4rem)] p-4 flex flex-col justify-between' },
    h('nav', { className: 'space-y-1' },
      h('div', { className: 'text-[11px] font-bold text-slate-400 tracking-wider uppercase px-3 mb-2' }, 'Main Menu'),
      filteredNav.map(item =>
        h('button', {
          key: item.id,
          onClick: () => setActiveTab(item.id),
          className: `w-full flex items-center gap-3 px-3 py-2.5 rounded-xl text-sm font-semibold transition-all ${activeTab === item.id ? 'bg-indigo-600 text-white shadow-md shadow-indigo-500/20' : 'text-slate-600 hover:bg-slate-100 hover:text-slate-900'}`
        },
          h('span', { className: 'text-base' }, item.icon),
          item.label
        )
      )
    ),

    h('div', { className: 'p-3 bg-slate-50 rounded-2xl border border-slate-200/80 text-xs text-slate-500' },
      h('div', { className: 'font-bold text-slate-700 mb-1' }, '⚙️ System Info'),
      h('div', null, 'API Port: 8080 (Crow C++)'),
      h('div', null, 'Web Port: 3000 (Node)')
    )
  );
}

// 4. Stat Card Component
function StatCard({ title, value, subtitle, icon, color = 'indigo' }) {
  const colors = {
    indigo: 'bg-indigo-500/10 text-indigo-600 border-indigo-200',
    emerald: 'bg-emerald-500/10 text-emerald-600 border-emerald-200',
    amber: 'bg-amber-500/10 text-amber-600 border-amber-200',
    rose: 'bg-rose-500/10 text-rose-600 border-rose-200'
  };

  return h('div', { className: 'bg-white p-5 rounded-2xl border border-slate-200 shadow-sm hover:shadow-md transition-all' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('p', { className: 'text-xs font-semibold text-slate-400 uppercase tracking-wide' }, title),
        h('h3', { className: 'text-2xl font-bold text-slate-800 mt-1' }, value),
        subtitle && h('p', { className: 'text-xs text-slate-500 mt-1' }, subtitle)
      ),
      h('div', { className: `w-12 h-12 rounded-xl border flex items-center justify-center text-xl font-bold ${colors[color]}` }, icon)
    )
  );
}

// --- PAGES ---

// 0. LOGIN PAGE COMPONENT
function LoginPage() {
  const { login, switchRole } = useContext(AuthContext);
  const [username, setUsername] = useState('student_alex');
  const [password, setPassword] = useState('password123');
  const [loading, setLoading] = useState(false);
  const [isRegister, setIsRegister] = useState(false);

  const handleSubmit = async (e) => {
    e.preventDefault();
    setLoading(true);
    await login(username, password);
    setLoading(false);
  };

  return h('div', { className: 'min-h-screen bg-gradient-to-br from-slate-900 via-indigo-950 to-slate-900 flex items-center justify-center p-4 font-sans' },
    h('div', { className: 'w-full max-w-md bg-white/95 backdrop-blur-xl p-8 rounded-3xl shadow-2xl border border-white/20 space-y-6' },
      // Header Logo
      h('div', { className: 'text-center space-y-2' },
        h('div', { className: 'w-16 h-16 rounded-2xl bg-gradient-to-tr from-indigo-600 to-sky-500 mx-auto flex items-center justify-center text-3xl text-white font-bold shadow-lg shadow-indigo-500/30' }, '🎓'),
        h('h2', { className: 'text-2xl font-bold text-slate-900' }, isRegister ? 'Create EduPortal Account' : 'Welcome to EduPortal'),
        h('p', { className: 'text-xs text-slate-500' }, 'Sign in to access University Management Portal & C++ Backend')
      ),

      // 1-Click Quick Demo Login Buttons
      h('div', { className: 'p-4 bg-indigo-50/80 rounded-2xl border border-indigo-100 space-y-2' },
        h('div', { className: 'text-xs font-bold text-indigo-900 uppercase tracking-wider text-center' }, '⚡ 1-Click Quick Demo Sign In'),
        h('div', { className: 'grid grid-cols-3 gap-2 pt-1' },
          h('button', {
            type: 'button',
            onClick: () => switchRole('student'),
            className: 'py-2 px-1 bg-white hover:bg-indigo-600 hover:text-white text-indigo-950 text-xs font-bold rounded-xl shadow-sm border border-indigo-200/60 transition-all text-center'
          }, '👨‍🎓 Student'),
          h('button', {
            type: 'button',
            onClick: () => switchRole('professor'),
            className: 'py-2 px-1 bg-white hover:bg-indigo-600 hover:text-white text-indigo-950 text-xs font-bold rounded-xl shadow-sm border border-indigo-200/60 transition-all text-center'
          }, '👨‍🏫 Professor'),
          h('button', {
            type: 'button',
            onClick: () => switchRole('admin'),
            className: 'py-2 px-1 bg-white hover:bg-indigo-600 hover:text-white text-indigo-950 text-xs font-bold rounded-xl shadow-sm border border-indigo-200/60 transition-all text-center'
          }, '👑 Admin')
        )
      ),

      // Login / Registration Form
      h('form', { onSubmit: handleSubmit, className: 'space-y-4' },
        h('div', { className: 'space-y-1' },
          h('label', { className: 'text-xs font-bold text-slate-700 uppercase tracking-wider' }, 'Username / Email'),
          h('input', {
            type: 'text',
            value: username,
            onChange: (e) => setUsername(e.target.value),
            required: true,
            className: 'w-full px-4 py-3 rounded-xl border border-slate-200 focus:outline-none focus:ring-2 focus:ring-indigo-600 text-sm font-medium'
          })
        ),

        h('div', { className: 'space-y-1' },
          h('label', { className: 'text-xs font-bold text-slate-700 uppercase tracking-wider' }, 'Password'),
          h('input', {
            type: 'password',
            value: password,
            onChange: (e) => setPassword(e.target.value),
            required: true,
            className: 'w-full px-4 py-3 rounded-xl border border-slate-200 focus:outline-none focus:ring-2 focus:ring-indigo-600 text-sm font-medium'
          })
        ),

        h('button', {
          type: 'submit',
          disabled: loading,
          className: 'w-full py-3.5 bg-gradient-to-r from-indigo-600 to-sky-600 text-white font-bold text-sm rounded-xl shadow-lg shadow-indigo-600/30 hover:opacity-95 transition-all'
        }, loading ? 'Authenticating with C++ REST API...' : (isRegister ? 'Register Account' : 'Sign In to Portal')),

        h('div', { className: 'text-center pt-2' },
          h('button', {
            type: 'button',
            onClick: () => setIsRegister(!isRegister),
            className: 'text-xs font-semibold text-indigo-600 hover:underline'
          }, isRegister ? 'Already have an account? Sign In' : 'Need an account? Register here')
        )
      )
    )
  );
}

// 1. Dashboard Page
function DashboardPage({ setActiveTab }) {
  const { user } = useContext(AuthContext);

  return h('div', { className: 'space-y-6' },
    // Welcome Banner
    h('div', { className: 'bg-gradient-to-r from-indigo-900 via-indigo-800 to-sky-900 text-white p-6 rounded-3xl shadow-xl flex flex-col md:flex-row items-center justify-between gap-6' },
      h('div', { className: 'space-y-2' },
        h('span', { className: 'px-3 py-1 bg-white/10 text-sky-200 rounded-full text-xs font-semibold uppercase tracking-wider backdrop-blur-md' }, `Welcome Back, ${user?.role}`),
        h('h2', { className: 'text-3xl font-bold' }, `Hello, ${user?.name}!`),
        h('p', { className: 'text-indigo-200 text-sm max-w-xl' }, 'Welcome to EduPortal v2.0. Manage academic records, course enrollments, attendance, and transcripts powered by C++ Crow REST API.')
      ),
      h('div', { className: 'flex gap-3' },
        h('button', { onClick: () => setActiveTab('courses'), className: 'px-4 py-2.5 bg-white text-indigo-900 font-bold rounded-xl text-sm shadow-md hover:bg-slate-100 transition-colors' }, 'Browse Courses'),
        h('button', { onClick: () => setActiveTab('results'), className: 'px-4 py-2.5 bg-indigo-700/60 hover:bg-indigo-700 text-white font-semibold rounded-xl text-sm transition-colors border border-indigo-400/30' }, 'View Transcripts')
      )
    ),

    // Quick Metrics Grid
    h('div', { className: 'grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4' },
      h(StatCard, { title: 'Current CGPA', value: '3.85 / 4.00', subtitle: 'Top 5% of Class', icon: '🎓', color: 'emerald' }),
      h(StatCard, { title: 'Attendance Rate', value: '94.2%', subtitle: '28 / 30 Classes Attended', icon: '📅', color: 'indigo' }),
      h(StatCard, { title: 'Enrolled Courses', value: '4 Courses', subtitle: '14 Credit Hours', icon: 'amber' }),
      h(StatCard, { title: 'Pending Tasks', value: '2 Exams', subtitle: 'Midterms next week', icon: '⏳', color: 'rose' })
    ),

    // Notice Board Section
    h('div', { className: 'bg-white p-6 rounded-2xl border border-slate-200 shadow-sm space-y-4' },
      h('div', { className: 'flex items-center justify-between' },
        h('h3', { className: 'text-lg font-bold text-slate-800' }, '📢 Official Announcements'),
        h('button', { onClick: () => setActiveTab('notices'), className: 'text-xs font-bold text-indigo-600 hover:text-indigo-800' }, 'View All Notices →')
      ),
      h('div', { className: 'grid grid-cols-1 md:grid-cols-2 gap-4' },
        INITIAL_NOTICES.map(notice =>
          h('div', { key: notice.id, className: 'p-4 rounded-xl bg-slate-50 border border-slate-200 space-y-2' },
            h('div', { className: 'flex items-center justify-between text-xs' },
              h('span', { className: 'px-2 py-0.5 rounded-md font-bold bg-indigo-100 text-indigo-700' }, notice.category),
              h('span', { className: 'text-slate-400 font-medium' }, notice.date)
            ),
            h('h4', { className: 'font-bold text-slate-800 text-sm' }, notice.title),
            h('p', { className: 'text-xs text-slate-600 line-clamp-2' }, notice.content)
          )
        )
      )
    )
  );
}

// 2. Profile Page
function ProfilePage() {
  const { user } = useContext(AuthContext);

  return h('div', { className: 'max-w-4xl mx-auto space-y-6' },
    h('div', { className: 'bg-white rounded-3xl border border-slate-200 shadow-sm overflow-hidden' },
      h('div', { className: 'h-32 bg-gradient-to-r from-indigo-600 to-sky-500' }),
      h('div', { className: 'px-6 pb-6 relative' },
        h('div', { className: 'flex flex-col sm:flex-row items-start sm:items-end justify-between -mt-12 mb-4 gap-4' },
          h('div', { className: 'flex items-end gap-4' },
            h('div', { className: 'w-24 h-24 rounded-2xl bg-slate-900 border-4 border-white shadow-lg flex items-center justify-center text-4xl text-white font-bold' }, user?.name?.[0] || 'U'),
            h('div', null,
              h('h2', { className: 'text-2xl font-bold text-slate-800' }, user?.name),
              h('p', { className: 'text-sm text-indigo-600 font-semibold uppercase tracking-wider' }, `${user?.role} • ${user?.department}`)
            )
          ),
          h('span', { className: 'px-4 py-1.5 rounded-full bg-emerald-50 text-emerald-700 border border-emerald-200 text-xs font-bold' }, 'Status: Active Account')
        ),

        h('div', { className: 'grid grid-cols-1 sm:grid-cols-2 gap-4 mt-6 pt-6 border-t border-slate-100 text-sm' },
          h('div', { className: 'space-y-1' },
            h('p', { className: 'text-xs text-slate-400 font-semibold uppercase' }, 'Email Address'),
            h('p', { className: 'font-semibold text-slate-800' }, user?.email)
          ),
          user?.rollNumber && h('div', { className: 'space-y-1' },
            h('p', { className: 'text-xs text-slate-400 font-semibold uppercase' }, 'Roll Number'),
            h('p', { className: 'font-semibold text-slate-800' }, user?.rollNumber)
          ),
          h('div', { className: 'space-y-1' },
            h('p', { className: 'text-xs text-slate-400 font-semibold uppercase' }, 'Academic Department'),
            h('p', { className: 'font-semibold text-slate-800' }, user?.department)
          ),
          user?.gpa && h('div', { className: 'space-y-1' },
            h('p', { className: 'text-xs text-slate-400 font-semibold uppercase' }, 'Cumulative GPA'),
            h('p', { className: 'font-bold text-emerald-600' }, `${user?.gpa} / 4.00`)
          )
        )
      )
    )
  );
}

// 3. Courses Page
function CoursesPage() {
  const [courses] = useState(INITIAL_COURSES);

  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('h2', { className: 'text-2xl font-bold text-slate-800' }, 'Academic Course Catalog'),
        h('p', { className: 'text-xs text-slate-500' }, 'Browse courses offered by University Departments')
      )
    ),

    h('div', { className: 'grid grid-cols-1 md:grid-cols-2 gap-4' },
      courses.map(course =>
        h('div', { key: course.id, className: 'bg-white p-5 rounded-2xl border border-slate-200 shadow-sm hover:border-indigo-300 transition-all space-y-3' },
          h('div', { className: 'flex items-center justify-between' },
            h('span', { className: 'px-2.5 py-1 rounded-lg bg-indigo-50 text-indigo-700 font-bold text-xs' }, course.code),
            h('span', { className: 'text-xs text-slate-500 font-semibold' }, `${course.credits} Credits • Semester ${course.semester}`)
          ),
          h('h3', { className: 'text-lg font-bold text-slate-800' }, course.name),
          h('div', { className: 'flex items-center justify-between text-xs text-slate-500 pt-3 border-t border-slate-100' },
            h('span', null, `Instructor: ${course.instructor}`),
            h('span', { className: 'font-bold text-emerald-600' }, `${course.enrolled} Students Enrolled`)
          )
        )
      )
    )
  );
}

// 4. Attendance Page
function AttendancePage() {
  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('h2', { className: 'text-2xl font-bold text-slate-800' }, 'Attendance Registry'),
        h('p', { className: 'text-xs text-slate-500' }, 'Track course-wise attendance logs and percentages')
      )
    ),

    h('div', { className: 'bg-white rounded-2xl border border-slate-200 shadow-sm overflow-hidden' },
      h('table', { className: 'w-full text-left border-collapse text-sm' },
        h('thead', { className: 'bg-slate-50 border-b border-slate-200 text-xs font-bold text-slate-500 uppercase' },
          h('tr', null,
            h('th', { className: 'p-4' }, 'Date'),
            h('th', { className: 'p-4' }, 'Course'),
            h('th', { className: 'p-4' }, 'Student Name'),
            h('th', { className: 'p-4' }, 'Status')
          )
        ),
        h('tbody', { className: 'divide-y divide-slate-100' },
          INITIAL_ATTENDANCE.map(row =>
            h('tr', { key: row.id, className: 'hover:bg-slate-50/80 transition-colors' },
              h('td', { className: 'p-4 font-semibold text-slate-700' }, row.date),
              h('td', { className: 'p-4 text-slate-800 font-bold' }, row.course),
              h('td', { className: 'p-4 text-slate-600' }, row.studentName),
              h('td', { className: 'p-4' },
                h('span', { className: `px-3 py-1 rounded-full text-xs font-bold ${row.status === 'Present' ? 'bg-emerald-100 text-emerald-700' : 'bg-amber-100 text-amber-700'}` }, row.status)
              )
            )
          )
        )
      )
    )
  );
}

// 5. Results Page
function ResultsPage() {
  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('h2', { className: 'text-2xl font-bold text-slate-800' }, 'Academic Transcripts & Grades'),
        h('p', { className: 'text-xs text-slate-500' }, 'Exam scores, grade letters, and credit point breakdown')
      ),
      h('div', { className: 'px-4 py-2 bg-emerald-500/10 border border-emerald-500/30 text-emerald-700 font-bold rounded-xl text-sm' }, 'CGPA: 3.85 / 4.00')
    ),

    h('div', { className: 'bg-white rounded-2xl border border-slate-200 shadow-sm overflow-hidden' },
      h('table', { className: 'w-full text-left border-collapse text-sm' },
        h('thead', { className: 'bg-slate-50 border-b border-slate-200 text-xs font-bold text-slate-500 uppercase' },
          h('tr', null,
            h('th', { className: 'p-4' }, 'Course'),
            h('th', { className: 'p-4' }, 'Exam Type'),
            h('th', { className: 'p-4' }, 'Marks'),
            h('th', { className: 'p-4' }, 'Grade'),
            h('th', { className: 'p-4' }, 'Grade Points')
          )
        ),
        h('tbody', { className: 'divide-y divide-slate-100' },
          INITIAL_RESULTS.map(res =>
            h('tr', { key: res.id, className: 'hover:bg-slate-50/80 transition-colors' },
              h('td', { className: 'p-4 font-bold text-slate-800' }, res.course),
              h('td', { className: 'p-4 text-slate-600' }, res.exam),
              h('td', { className: 'p-4 font-semibold text-slate-700' }, `${res.marks} / ${res.maxMarks}`),
              h('td', { className: 'p-4' },
                h('span', { className: 'px-3 py-1 rounded-lg bg-indigo-100 text-indigo-700 font-bold text-xs' }, res.grade)
              ),
              h('td', { className: 'p-4 font-bold text-emerald-600' }, res.points)
            )
          )
        )
      )
    )
  );
}

// 6. Enrollment Page
function EnrollmentPage() {
  const [enrolled, setEnrolled] = useState([1, 2]);

  const toggleEnroll = (id) => {
    if (enrolled.includes(id)) setEnrolled(enrolled.filter(item => item !== id));
    else setEnrolled([...enrolled, id]);
  };

  return h('div', { className: 'space-y-6' },
    h('div', null,
      h('h2', { className: 'text-2xl font-bold text-slate-800' }, 'Course Enrollment Portal'),
      h('p', { className: 'text-xs text-slate-500' }, 'Register or drop courses for Fall Semester 2026')
    ),

    h('div', { className: 'grid grid-cols-1 md:grid-cols-2 gap-4' },
      INITIAL_COURSES.map(course => {
        const isEnrolled = enrolled.includes(course.id);
        return h('div', { key: course.id, className: `bg-white p-5 rounded-2xl border transition-all ${isEnrolled ? 'border-indigo-400 bg-indigo-50/10' : 'border-slate-200'}` },
          h('div', { className: 'flex items-center justify-between mb-3' },
            h('span', { className: 'px-2.5 py-1 rounded-lg bg-slate-100 text-slate-700 font-bold text-xs' }, course.code),
            h('span', { className: 'text-xs font-semibold text-slate-500' }, `${course.credits} Credits`)
          ),
          h('h3', { className: 'font-bold text-slate-800 text-base mb-1' }, course.name),
          h('p', { className: 'text-xs text-slate-500 mb-4' }, `Instructor: ${course.instructor}`),
          h('button', {
            onClick: () => toggleEnroll(course.id),
            className: `w-full py-2.5 rounded-xl font-bold text-xs transition-all ${isEnrolled ? 'bg-rose-50 hover:bg-rose-100 text-rose-600 border border-rose-200' : 'bg-indigo-600 hover:bg-indigo-700 text-white shadow-md'}`
          }, isEnrolled ? 'Drop Course' : '1-Click Enroll')
        );
      })
    )
  );
}

// 7. Reports Page
function ReportsPage() {
  return h('div', { className: 'space-y-6' },
    h('div', { className: 'flex items-center justify-between' },
      h('div', null,
        h('h2', { className: 'text-2xl font-bold text-slate-800' }, 'Analytics & Reports'),
        h('p', { className: 'text-xs text-slate-500' }, 'System overview metrics and academic reports')
      ),
      h('button', { onClick: () => window.print(), className: 'px-4 py-2 bg-indigo-600 text-white font-bold rounded-xl text-xs shadow-md' }, '🖨️ Export PDF / Print')
    ),

    h('div', { className: 'grid grid-cols-1 md:grid-cols-3 gap-4' },
      h(StatCard, { title: 'Total Active Students', value: '1,420', subtitle: 'Across 6 Departments', icon: '👥', color: 'indigo' }),
      h(StatCard, { title: 'Total Faculty Members', value: '85', subtitle: 'Full-time Professors', icon: '👨‍🏫', color: 'emerald' }),
      h(StatCard, { title: 'Average Attendance', value: '92.4%', subtitle: 'Fall 2026 Semester', icon: '📊', color: 'amber' })
    )
  );
}

// 8. Announcements Page
function AnnouncementsPage() {
  return h('div', { className: 'space-y-6' },
    h('div', null,
      h('h2', { className: 'text-2xl font-bold text-slate-800' }, 'Notice Board'),
      h('p', { className: 'text-xs text-slate-500' }, 'Official university circulars and news')
    ),

    h('div', { className: 'space-y-4' },
      INITIAL_NOTICES.map(notice =>
        h('div', { key: notice.id, className: 'bg-white p-6 rounded-2xl border border-slate-200 shadow-sm space-y-3' },
          h('div', { className: 'flex items-center justify-between' },
            h('span', { className: 'px-3 py-1 rounded-full text-xs font-bold bg-indigo-100 text-indigo-700' }, notice.category),
            h('span', { className: 'text-xs text-slate-400 font-semibold' }, notice.date)
          ),
          h('h3', { className: 'text-lg font-bold text-slate-800' }, notice.title),
          h('p', { className: 'text-sm text-slate-600 leading-relaxed' }, notice.content),
          h('div', { className: 'text-xs text-slate-400 font-medium pt-2 border-t border-slate-100' }, `Posted by: ${notice.author}`)
        )
      )
    )
  );
}

// --- MAIN APP CONTROLLER ---
function App() {
  const [activeTab, setActiveTab] = useState('dashboard');
  const { user } = useContext(AuthContext);

  // If user is not logged in, render the Login Page!
  if (!user) {
    return h(LoginPage);
  }

  const renderContent = () => {
    switch (activeTab) {
      case 'dashboard': return h(DashboardPage, { setActiveTab });
      case 'profile': return h(ProfilePage);
      case 'courses': return h(CoursesPage);
      case 'attendance': return h(AttendancePage);
      case 'results': return h(ResultsPage);
      case 'enrollment': return h(EnrollmentPage);
      case 'reports': return h(ReportsPage);
      case 'notices': return h(AnnouncementsPage);
      default: return h(DashboardPage, { setActiveTab });
    }
  };

  return h('div', { className: 'min-h-screen bg-slate-50 flex flex-col font-sans' },
    h(Navbar, { activeTab, setActiveTab }),
    h('div', { className: 'flex-1 flex max-w-7xl w-full mx-auto' },
      h(Sidebar, { activeTab, setActiveTab }),
      h('main', { className: 'flex-1 p-6 md:p-8 overflow-y-auto' }, renderContent())
    )
  );
}

// --- MOUNT REACT APP TO DOM ---
document.addEventListener('DOMContentLoaded', () => {
  const rootElement = document.getElementById('root');
  if (rootElement) {
    const root = ReactDOM.createRoot(rootElement);
    root.render(
      h(AuthProvider, null, h(App))
    );
  }
});
