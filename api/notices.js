/**
 * Vercel Serverless API Endpoint: Bulletins & Announcements
 * Route: GET /api/notices, POST /api/notices
 */
import { verifyJwt } from './_jwt.js';

const NOTICES = [
  { id: 1, title: 'Fall 2026 Mid-Semester Examination Schedule Published', category: 'Academic', date: '2026-08-01', author: 'Academic Affairs Office', content: 'The official schedule for mid-semester examinations has been published. All students must review their course dates.' },
  { id: 2, title: 'University Research Grant Call for Proposals', category: 'Research', date: '2026-07-28', author: 'Office of Research', content: 'Faculty members are invited to submit research funding proposals for the upcoming fiscal cycle.' }
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

    if (claims && claims.role === 'student') {
      return res.status(403).json({ success: false, error: 'Forbidden: Students are not authorized to publish official university bulletins.' });
    }

    const newNotice = {
      id: Date.now(),
      title: req.body?.title || 'General Circular Announcement',
      category: req.body?.category || 'Academic',
      date: new Date().toISOString().split('T')[0],
      author: claims?.username || 'Department Office',
      content: req.body?.content || 'Notice details.'
    };
    NOTICES.unshift(newNotice);
    return res.status(201).json({ success: true, data: newNotice });
  }

  return res.status(200).json({
    success: true,
    data: NOTICES
  });
}
