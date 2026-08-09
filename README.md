# 🏛️ EduPortal — Enterprise Academic Management Platform

A high-performance **University Management System** featuring a **Vite + React 18 + TypeScript Single Page Application (SPA)** with **Vercel Serverless JWT & RBAC API Endpoints** and an optional **C++17 Crow REST API Backend**.

---

## 🌟 Key Features

### 🔐 1. Authentication & Role-Based Access Control (RBAC)
- **Sign In & Sign Up (Registration)**: Dedicated registration for Students and Faculty with department and roll number enrollment.
- **Google Workspace SSO**: Google OAuth 2.0 Single Sign-On integration.
- **Crypto Password Hashing & HMAC-SHA256 JWT**: Real JWT token generation & server-side verification.
- **Server-Side RBAC Enforcement**: Role-based access validation preventing unauthorized actions (e.g. course creation, attendance recording, bulletin publication).

### 👤 2. Academic Profiles & Student Records
- Student identity records, roll numbers, GPA scores, and department affiliations.
- Faculty designations, department chair badges, and course assignments.

### 📅 3. Attendance Registry
- Course-wise attendance logging and compliance metrics.
- Visual status indicators for class participation.

### 🎓 4. Examinations & Academic Transcripts
- Score calculation engine across mid-term exams, finals, and lab assessments.
- Automated letter grade assignment (A+, A, B, C, F) and CGPA calculation.

### 📚 5. Course Catalog & Self-Service Registration
- University-wide curriculum listings filtered by department and credit hours.
- Self-service enrollment and registration management.

### 📢 6. Bulletins & System Analytics
- University circulars and academic notices.
- Executive summary metrics and printable PDF reports.

---

## 🏗️ Architecture & Tech Stack

```
   ┌───────────────────────────────────────────────────────────┐
   │             Vite + React 18 + TypeScript SPA              │
   │      Modular Components • Tailwind CSS • Context API      │
   └─────────────────────────────┬─────────────────────────────┘
                                 │ REST API Calls (/api)
   ┌─────────────────────────────▼─────────────────────────────┐
   │            Vercel Serverless Node.js APIs                 │
   │   HMAC-SHA256 JWT Verification • RBAC Authorization       │
   └───────────────────────────────────────────────────────────┘
```

- **Frontend**: Vite 5, React 18, TypeScript, Tailwind CSS
- **Serverless API**: Node.js, HMAC-SHA256 JWT Engine, RBAC Policy
- **Alternative Backend**: C++17, Crow REST Framework, SQLiteCpp, CMake
- **Hosting & CI/CD**: Vercel Edge Network

---

## 🚀 Quick Start Guide

### Launch Development Server

```bash
# Install dependencies
npm install

# Start Vite HMR Dev Server (Port 3000)
npm run dev
```

Open your browser at **`http://localhost:3000`** to access EduPortal!

---

### Production Build

```bash
# Compile TypeScript & bundle application to dist/
npm run build
```

---

## 📄 License
This project is open source and available under the [MIT License](LICENSE).
