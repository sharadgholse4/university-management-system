const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url');

const PORT = 3000;
const BACKEND_URL = 'http://127.0.0.1:8080';

const MIME_TYPES = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'application/javascript; charset=utf-8',
  '.ts': 'application/javascript; charset=utf-8',
  '.tsx': 'application/javascript; charset=utf-8',
  '.css': 'text/css; charset=utf-8',
  '.json': 'application/json',
  '.png': 'image/png',
  '.jpg': 'image/jpeg',
  '.svg': 'image/svg+xml',
  '.ico': 'image/x-icon',
};

const server = http.createServer((req, res) => {
  const parsedUrl = url.parse(req.url, true);

  // 1. Proxy /api requests to C++ Crow Backend at http://127.0.0.1:8080
  if (parsedUrl.pathname.startsWith('/api')) {
    const proxyReq = http.request(
      `${BACKEND_URL}${req.url}`,
      {
        method: req.method,
        headers: {
          ...req.headers,
          host: '127.0.0.1:8080',
        },
      },
      (proxyRes) => {
        res.writeHead(proxyRes.statusCode, proxyRes.headers);
        proxyRes.pipe(res);
      }
    );

    proxyReq.on('error', (err) => {
      // If C++ backend is offline, return fallback JSON status
      if (parsedUrl.pathname === '/api/health') {
        res.writeHead(200, { 'Content-Type': 'application/json' });
        return res.end(JSON.stringify({ status: 'offline', message: 'C++ backend starting up...' }));
      }

      res.writeHead(502, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify({ error: 'C++ Backend Offline', details: err.message }));
    });

    req.pipe(proxyReq);
    return;
  }

  // 2. Serve static frontend files
  let filePath = path.join(__dirname, parsedUrl.pathname);
  if (parsedUrl.pathname === '/' || !fs.existsSync(filePath) || fs.statSync(filePath).isDirectory()) {
    filePath = path.join(__dirname, 'index.html');
  }

  const ext = path.extname(filePath).toLowerCase();
  const contentType = MIME_TYPES[ext] || 'text/plain';

  fs.readFile(filePath, (err, content) => {
    if (err) {
      res.writeHead(500);
      res.end('Server Error');
      return;
    }
    res.writeHead(200, { 'Content-Type': contentType });
    res.end(content, 'utf-8');
  });
});

server.listen(PORT, '0.0.0.0', () => {
  console.log(`=======================================================`);
  console.log(`🎓 EduPortal Web App Frontend running on: http://localhost:${PORT}`);
  console.log(`⚙️ Proxying /api to C++ Crow Backend at: ${BACKEND_URL}`);
  console.log(`=======================================================`);
});
