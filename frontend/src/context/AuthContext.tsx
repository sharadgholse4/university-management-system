import React, { createContext, useContext, useState, useEffect } from 'react';
import { User, Student, Role, AuthState } from '../types';
import { api } from '../services/api';
import { DEMO_USERS, INITIAL_STUDENT_PROFILE } from '../data/mockData';

interface AuthContextType extends AuthState {
  login: (email: string, pass: string) => Promise<void>;
  loginAsDemo: (role: Role) => Promise<void>;
  register: (email: string, pass: string, role: Role) => Promise<void>;
  logout: () => void;
  backendOnline: boolean;
  checkBackendStatus: () => Promise<void>;
}

const AuthContext = createContext<AuthContextType | undefined>(undefined);

export const AuthProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [state, setState] = useState<AuthState>({
    token: api.getToken(),
    user: null,
    studentProfile: null,
    isAuthenticated: false,
    isLoading: true,
  });

  const [backendOnline, setBackendOnline] = useState<boolean>(false);

  const checkBackendStatus = async () => {
    const res = await api.checkHealth();
    setBackendOnline(res.status === 'ok');
  };

  useEffect(() => {
    checkBackendStatus();
    const interval = setInterval(checkBackendStatus, 10000);
    return () => clearInterval(interval);
  }, []);

  const loadUser = async () => {
    const token = api.getToken();
    if (!token) {
      setState((prev) => ({ ...prev, isLoading: false, isAuthenticated: false }));
      return;
    }

    try {
      // Attempt to load current user from C++ backend
      const user = await api.getCurrentUser();
      let studentProfile: Student | null = null;

      if (user.role === 'student') {
        try {
          studentProfile = await api.getStudentByUserId(user.id);
        } catch {
          studentProfile = INITIAL_STUDENT_PROFILE;
        }
      }

      setState({
        token,
        user,
        studentProfile,
        isAuthenticated: true,
        isLoading: false,
      });
    } catch (err) {
      // Fallback for demo token if backend is restarting
      console.warn('Backend session load failed, resetting token or loading demo state');
      api.setToken(null);
      setState({
        token: null,
        user: null,
        studentProfile: null,
        isAuthenticated: false,
        isLoading: false,
      });
    }
  };

  useEffect(() => {
    loadUser();
  }, []);

  const login = async (email: string, pass: string) => {
    setState((prev) => ({ ...prev, isLoading: true }));
    try {
      const data = await api.login(email, pass);
      let studentProfile: Student | null = null;
      if (data.user.role === 'student') {
        try {
          studentProfile = await api.getStudentByUserId(data.user.id);
        } catch {
          studentProfile = { ...INITIAL_STUDENT_PROFILE, userId: data.user.id };
        }
      }

      setState({
        token: data.token,
        user: data.user,
        studentProfile,
        isAuthenticated: true,
        isLoading: false,
      });
    } catch (err: any) {
      setState((prev) => ({ ...prev, isLoading: false }));
      throw err;
    }
  };

  const register = async (email: string, pass: string, role: Role) => {
    setState((prev) => ({ ...prev, isLoading: true }));
    try {
      await api.register(email, pass, role);
      // Auto login after registration
      await login(email, pass);
    } catch (err: any) {
      setState((prev) => ({ ...prev, isLoading: false }));
      throw err;
    }
  };

  const loginAsDemo = async (role: Role) => {
    const demo = DEMO_USERS[role];
    if (!demo) return;

    try {
      // First try real backend login
      await login(demo.email, demo.pass);
    } catch {
      // If user isn't registered on backend yet, try registering then logging in
      try {
        await register(demo.email, demo.pass, demo.role);
      } catch {
        // Fallback to local demo state if backend offline
        const fakeUser: User = {
          id: role === 'student' ? 1 : role === 'professor' ? 2 : 3,
          email: demo.email,
          role: demo.role,
        };
        api.setToken(`demo_token_${role}`);
        setState({
          token: `demo_token_${role}`,
          user: fakeUser,
          studentProfile: role === 'student' ? INITIAL_STUDENT_PROFILE : null,
          isAuthenticated: true,
          isLoading: false,
        });
      }
    }
  };

  const logout = () => {
    api.setToken(null);
    setState({
      token: null,
      user: null,
      studentProfile: null,
      isAuthenticated: false,
      isLoading: false,
    });
  };

  return (
    <AuthContext.Provider
      value={{
        ...state,
        login,
        loginAsDemo,
        register,
        logout,
        backendOnline,
        checkBackendStatus,
      }}
    >
      {children}
    </AuthContext.Provider>
  );
};

export const useAuth = () => {
  const context = useContext(AuthContext);
  if (!context) {
    throw new Error('useAuth must be used within an AuthProvider');
  }
  return context;
};
