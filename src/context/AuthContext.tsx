import React, { createContext, useState, ReactNode } from 'react';
import { User, AuthContextType, DemoPreset, Role } from '../types';

const API_BASE = '/api';

function safeGetStorage(key: string): string | null {
  try { return localStorage.getItem(key); } catch (e) { return null; }
}

function safeSetStorage(key: string, value: string): void {
  try { localStorage.setItem(key, value); } catch (e) {}
}

function safeRemoveStorage(key: string): void {
  try { localStorage.removeItem(key); } catch (e) {}
}

export async function apiCall(endpoint: string, method = 'GET', body: any = null, token: string | null = null) {
  const headers: Record<string, string> = { 'Content-Type': 'application/json' };
  const savedToken = token || safeGetStorage('eduportal_token');
  if (savedToken) headers['Authorization'] = `Bearer ${savedToken}`;

  const options: RequestInit = { method, headers };
  if (body) options.body = JSON.stringify(body);

  try {
    const res = await fetch(`${API_BASE}${endpoint}`, options);
    const data = await res.json();
    return data;
  } catch (err) {
    return { success: false, error: 'Network error communicating with server.' };
  }
}

export const DEMO_PRESETS: Record<string, DemoPreset> = {
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

export const AuthContext = createContext<AuthContextType | null>(null);

export const AuthProvider: React.FC<{ children: ReactNode }> = ({ children }) => {
  const [user, setUser] = useState<User | null>(() => {
    const saved = safeGetStorage('eduportal_user');
    try { return saved ? JSON.parse(saved) : null; } catch (e) { return null; }
  });
  const [token, setToken] = useState<string | null>(() => safeGetStorage('eduportal_token'));
  const [loading, setLoading] = useState(false);

  const getRegisteredUsers = (): Record<string, User> => {
    try {
      const data = safeGetStorage('eduportal_registered_users');
      return data ? JSON.parse(data) : {};
    } catch (e) { return {}; }
  };

  const saveRegisteredUser = (userData: User) => {
    const users = getRegisteredUsers();
    users[userData.username.toLowerCase()] = userData;
    safeSetStorage('eduportal_registered_users', JSON.stringify(users));
  };

  const login = async (username: string, password?: string) => {
    setLoading(true);
    let userData: User | null = null;
    const lowerUser = (username || '').toLowerCase().trim();

    try {
      const res = await apiCall('/auth/login', 'POST', { username: lowerUser, password });
      if (res && res.success && res.data && res.data.token) {
        userData = res.data.user || {
          id: Date.now(),
          username: lowerUser,
          name: lowerUser.replace('.', ' ').replace('_', ' ').replace(/\b\w/g, l => l.toUpperCase()),
          role: (res.data.role || (lowerUser.includes('prof') ? 'professor' : lowerUser.includes('admin') ? 'admin' : 'student')) as Role,
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
      if (lowerUser.includes('prof') || lowerUser.includes('smith')) userData = DEMO_PRESETS.professor as User;
      else if (lowerUser.includes('admin') || lowerUser.includes('connor')) userData = DEMO_PRESETS.admin as User;
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

  const register = async (accountData: {
    name: string;
    email: string;
    username: string;
    password?: string;
    role: Role;
    department: string;
    rollNumber?: string;
    designation?: string;
  }) => {
    setLoading(true);
    const lowerUser = accountData.username.toLowerCase().trim();

    const newUser: User = {
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

  const loginWithGoogle = async (credentialOrUser: any) => {
    setLoading(true);
    let gUser: User | null = null;
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
        username: (credentialOrUser?.email || 'alex.johnson').split('@')[0],
        name: credentialOrUser?.name || 'Alex Johnson (Google SSO)',
        email: credentialOrUser?.email || 'alex.johnson@university.edu',
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

  return (
    <AuthContext.Provider value={{ user, token, loading, login, register, loginWithGoogle, logout }}>
      {children}
    </AuthContext.Provider>
  );
};
