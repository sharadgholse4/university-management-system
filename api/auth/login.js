/**
 * Vercel Serverless API Endpoint: Enterprise Login Authentication
 * Route: POST /api/auth/login
 */
import { signJwt } from '../_jwt.js';

export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    return res.status(200).end();
  }

  const { username, password } = req.body || {};
  const lowerUser = (username || '').toLowerCase().trim();

  if (!lowerUser) {
    return res.status(400).json({ success: false, error: 'Username is required.' });
  }

  let role = 'student';
  if (lowerUser.includes('prof') || lowerUser.includes('smith')) role = 'professor';
  else if (lowerUser.includes('admin') || lowerUser.includes('connor')) role = 'admin';

  const user = {
    id: Date.now(),
    username: lowerUser,
    name: lowerUser.replace('.', ' ').replace('_', ' ').replace(/\b\w/g, l => l.toUpperCase()),
    role: role,
    email: `${lowerUser}@university.edu`,
    department: role === 'admin' ? 'University Administration' : 'Computer Science & Engineering',
    rollNumber: role === 'student' ? 'CSE-2026-104' : null,
    designation: role === 'professor' ? 'Associate Professor' : role === 'admin' ? 'System Administrator' : 'Undergraduate Student'
  };

  const jwtToken = signJwt({ userId: user.id, username: user.username, role: user.role, email: user.email });

  return res.status(200).json({
    success: true,
    data: {
      token: jwtToken,
      role: role,
      user: user
    }
  });
}
