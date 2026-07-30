# 🎓 University Management System (EduPortal v2.0)

A enterprise-grade, high-performance **University Management System** featuring a **C++17 Crow REST API backend** and a modern **React + TypeScript web frontend**.

---

## 🌟 Key Features

### 🔐 1. Authentication & Role-Based Access Control (RBAC)
- 3-Layer Role-Based Access Control (**Student**, **Professor**, **Admin**) enforced at the C++ REST API layer.
- **JWT (HMAC-SHA256)** token issuance and verification without external OpenSSL dependencies.
- **BCrypt** password hashing with salt auto-generation.

### 👤 2. Student Profile Management
- Comprehensive student records, roll numbers, department standings, and profile editing.
- Automatic student ID linkage to user authentication accounts.

### 📅 3. Attendance Management System
- Interactive attendance marking grid for Professors and Administrators.
- Real-time student attendance percentage calculations and visual status rings.

### 🎓 4. Results, Grading & CGPA System
- Exam marks recording (Midterm, Final, Assignments).
- Automated grade assignment (A+, A, B+, B, C, F) and grade point conversions.
- Cumulative GPA (CGPA) calculation engine.

### 📚 5. Course & Department Catalog
- Department structures, course credit hours, semester filters (Semester 1–8).
- Course assignment to faculty members.

### 📝 6. Self-Service Course Enrollment Portal
- 1-Click student course registration and drop portal.
- Real-time course capacity validation and student rosters.

### 📊 7. System Analytics & Academic Reports
- Department performance metrics and student grade distribution histograms.
- Printable/PDF summary reports generation.

---

## 🏗️ Architecture & Technology Stack

```
   ┌─────────────────────────────────────────────────────────┐
   │             React 18 + TypeScript Frontend              │
   │  Vite • Glassmorphism CSS • Lucide Icons • Context API  │
   └───────────────────────────┬─────────────────────────────┘
                               │ HTTP / JSON API Proxy (Port 3000 -> 8080)
   ┌───────────────────────────▼─────────────────────────────┐
   │               C++17 Crow REST API Backend               │
   │   Controllers • Domain Services • Repository Interfaces │
   └───────────────────────────┬─────────────────────────────┘
                               │ SQLiteCpp RAII Wrapper
   ┌───────────────────────────▼─────────────────────────────┐
   │                     SQLite Database                     │
   └─────────────────────────────────────────────────────────┘
```

- **Backend**: C++17, Crow REST Framework, SQLiteCpp, CMake, MinGW / GCC.
- **Frontend**: React 18, TypeScript, Vite, Custom CSS Design System, Lucide React Icons.

---

## 🛠️ Build & Installation Guide

### Prerequisites
- **C++ Compiler**: GCC 10+ / MinGW / Clang (with C++17 support).
- **CMake**: v3.14 or higher.
- **Node.js**: v18+ or v22+.

---

### 1. Build & Launch the C++ REST API Backend

```bash
# 1. Configure CMake
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# 2. Compile Backend Executable
cmake --build build --config Release --target ums_server

# 3. Launch Backend Server (Listens on port 8080)
./build/bin/ums_server.exe
```

---

### 2. Launch the React Web Frontend

```bash
# 1. Navigate to frontend directory
cd frontend

# 2. Launch Development / Proxy Server (Listens on port 3000)
node server.cjs
```

Open **`http://localhost:3000`** in your browser!

---

## 🧪 Running Automated Unit Tests

```bash
# Configure build with testing enabled
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON

# Build test targets
cmake --build build --config Release --target test_user_model test_jwt_service test_student_model test_attendance_model test_result_model test_course_model test_enrollment_model

# Run GoogleTest suite
./build/bin/test_user_model.exe
./build/bin/test_jwt_service.exe
./build/bin/test_student_model.exe
./build/bin/test_attendance_model.exe
./build/bin/test_result_model.exe
./build/bin/test_course_model.exe
./build/bin/test_enrollment_model.exe
```

---

## 🚀 Push to Your GitHub Repository

To push this project to your own GitHub repository:

```bash
# 1. Add your remote GitHub repository URL
git remote add origin https://github.com/YOUR_USERNAME/university-management-system.git

# 2. Rename branch to main
git branch -M main

# 3. Push to GitHub
git push -u origin main
```
