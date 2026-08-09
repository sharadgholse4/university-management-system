/**
 * Vercel Serverless API Endpoint: University Bulletins
 * Route: GET /api/notices, POST /api/notices
 */

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
    const notice = {
      id: Date.now(),
      title: req.body?.title || 'Announcement',
      category: req.body?.category || 'Academic',
      date: new Date().toISOString().split('T')[0],
      author: req.body?.author || 'Department Office',
      content: req.body?.content || 'Notice details.'
    };
    NOTICES.unshift(notice);
    return res.status(201).json({ success: true, data: notice });
  }

  return res.status(200).json({
    success: true,
    data: NOTICES
  });
}
