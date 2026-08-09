/**
 * Vercel Serverless API Endpoint: Account Registration
 * Route: POST /api/auth/register
 */
export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    return res.status(200).end();
  }

  const { name, email, username, role, department, rollNumber } = req.body || {};

  const user = {
    id: Date.now(),
    username: (username || 'user').toLowerCase().trim(),
    name: name || 'Registered User',
    email: email || 'user@university.edu',
    role: role || 'student',
    department: department || 'Computer Science & Engineering',
    rollNumber: rollNumber || 'CSE-2026-REG'
  };

  return res.status(201).json({
    success: true,
    message: 'User account registered successfully.',
    data: {
      token: `vercel-jwt-${Date.now()}`,
      user: user
    }
  });
}
