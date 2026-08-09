/**
 * Vercel Serverless API Endpoint: Health Check
 * Route: GET /api/health
 */
export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    return res.status(200).end();
  }

  return res.status(200).json({
    service: 'University Management System Serverless Engine',
    status: 'ok',
    timestamp: new Date().toISOString()
  });
}
