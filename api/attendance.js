/**
 * Vercel Serverless API Endpoint: Attendance Registry
 * Route: GET /api/attendance, POST /api/attendance
 */

const ATTENDANCE = [
  { id: 1, date: '2026-08-01', course: 'CSE-101 Data Structures & Algorithms', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' },
  { id: 2, date: '2026-07-29', course: 'CSE-202 Database Management Systems', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' },
  { id: 3, date: '2026-07-28', course: 'MAT-301 Applied Linear Algebra', student: 'Alex Johnson (CSE-2026-104)', status: 'Present' }
];

export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    return res.status(200).end();
  }

  if (req.method === 'POST') {
    const entry = {
      id: Date.now(),
      date: req.body?.date || new Date().toISOString().split('T')[0],
      course: req.body?.course || 'CSE-101 Data Structures',
      student: req.body?.student || 'Alex Johnson',
      status: req.body?.status || 'Present'
    };
    ATTENDANCE.unshift(entry);
    return res.status(201).json({ success: true, data: entry });
  }

  return res.status(200).json({
    success: true,
    data: ATTENDANCE
  });
}
