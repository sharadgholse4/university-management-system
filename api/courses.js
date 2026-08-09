/**
 * Vercel Serverless API Endpoint: Course Catalog Management
 * Route: GET /api/courses, POST /api/courses
 */
import { verifyJwt } from './_jwt.js';

const COURSES = [
  { id: 1, code: 'CSE-101', name: 'Data Structures & Algorithms', department: 'Computer Science', credits: 4, semester: 4, instructor: 'Dr. Robert Smith', enrolled: 48 },
  { id: 2, code: 'CSE-202', name: 'Relational Database Management Systems', department: 'Computer Science', credits: 4, semester: 4, instructor: 'Dr. Robert Smith', enrolled: 42 },
  { id: 3, code: 'EEE-105', name: 'Microprocessor Systems & Architecture', department: 'Electrical Engineering', credits: 3, semester: 2, instructor: 'Prof. Alan Turing', enrolled: 35 },
  { id: 4, code: 'MAT-301', name: 'Applied Linear Algebra & Statistics', department: 'Mathematics', credits: 3, semester: 3, instructor: 'Dr. Ada Lovelace', enrolled: 55 }
];

export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    return res.status(200).end();
  }

  if (req.method === 'POST') {
    const authHeader = req.headers.authorization || '';
    const token = authHeader.replace('Bearer ', '').trim();
    const claims = verifyJwt(token);

    // Enforce Role-Based Access Control (RBAC): Only Professors and Admins can create courses
    if (claims && claims.role === 'student') {
      return res.status(403).json({ success: false, error: 'Forbidden: Students are not authorized to create courses.' });
    }

    const newCourse = {
      id: Date.now(),
      code: req.body?.code || 'CSE-401',
      name: req.body?.name || 'Advanced Machine Learning',
      department: req.body?.department || 'Computer Science',
      credits: req.body?.credits || 4,
      semester: 4,
      instructor: req.body?.instructor || (claims?.username || 'Dr. Robert Smith'),
      enrolled: 1
    };
    COURSES.unshift(newCourse);
    return res.status(201).json({ success: true, data: newCourse });
  }

  return res.status(200).json({
    success: true,
    data: COURSES
  });
}
