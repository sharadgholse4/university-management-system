/**
 * Vercel Serverless API Endpoint: Account Registration
 * Route: POST /api/auth/register
 */
import { signJwt } from '../_jwt.js';

export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    return res.status(200).end();
  }

  if (req.method !== 'POST') {
    return res.status(405).json({ success: false, error: 'Method not allowed.' });
  }

  const { name, email, username, role, department, rollNumber, designation } = req.body || {};

  if (!username || !email || !name) {
    return res.status(400).json({ success: false, error: 'Missing required registration parameters.' });
  }

  const newUser = {
    id: Date.now(),
    username: username.toLowerCase().trim(),
    name: name.trim(),
    email: email.trim(),
    role: role || 'student',
    department: department || 'Computer Science & Engineering',
    rollNumber: role === 'student' ? (rollNumber || `CSE-2026-${Math.floor(100 + Math.random() * 900)}`) : null,
    designation: role === 'professor' ? (designation || 'Assistant Professor') : null
  };

  const jwtToken = signJwt({ userId: newUser.id, username: newUser.username, role: newUser.role, email: newUser.email });

  return res.status(201).json({
    success: true,
    data: {
      user: newUser,
      token: jwtToken
    }
  });
}
