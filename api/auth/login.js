/**
 * Vercel Serverless API Endpoint: Login Authentication
 * Route: POST /api/auth/login
 */
export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    return res.status(200).end();
  }

  const { username, password } = req.body || {};
  const lowerUser = (username || '').toLowerCase().trim();

  let role = 'student';
  if (lowerUser.includes('prof') || lowerUser.includes('smith')) role = 'professor';
  else if (lowerUser.includes('admin') || lowerUser.includes('connor')) role = 'admin';

  const user = {
    id: Date.now(),
    username: lowerUser || 'student.user',
    name: lowerUser ? lowerUser.replace('.', ' ').replace('_', ' ').replace(/\b\w/g, l => l.toUpperCase()) : 'Student User',
    role: role,
    email: `${lowerUser || 'student'}@university.edu`,
    department: role === 'admin' ? 'University Administration' : 'Computer Science & Engineering',
    rollNumber: role === 'student' ? 'CSE-2026-104' : null,
    designation: role === 'professor' ? 'Associate Professor' : role === 'admin' ? 'System Administrator' : 'Undergraduate Student'
  };

  return res.status(200).json({
    success: true,
    data: {
      token: `vercel-jwt-${Date.now()}`,
      role: role,
      user: user
    }
  });
}
