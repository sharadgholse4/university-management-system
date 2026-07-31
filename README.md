# 🏛️ EduPortal Enterprise | University Management System

An enterprise-grade, high-performance **University Management System** featuring a **C++17 Crow REST API Backend** and a modern **React Web Application Frontend**.

---

## 🌟 Enterprise Core Modules

### 🔐 1. Identity & Role-Based Access Control (RBAC)
- **Role-Based Access Control** (**Student**, **Faculty / Professor**, **Administrator**) enforced strictly at the API layer.
- **JWT Authentication** token issuance and claims verification.
- **BCrypt Password Security** with salted hashing.

### 👤 2. Student & Faculty Profile Management
- Comprehensive academic profiles, roll numbers, department assignments, and designations.
- Full linkage between user authentication credentials and student/faculty records.

### 📅 3. Attendance Registry
- Course-wise attendance recording and compliance tracking.
- Automated attendance percentage calculations and visual logs.

### 🎓 4. Examination & Grade Transcripts
- Multi-assessment score recording (Mid-Semester, Final, Laboratory, Assignments).
- Automated grade letter assignment (A+, A, B, C, F) and grade point conversions.
- Cumulative GPA (CGPA) calculation engine.

### 📚 5. Curriculum & Department Catalog
- Department structures, course credit hours, and semester scheduling.
- Course assignment to faculty members.

### 📝 6. Self-Service Course Registration Portal
- Course enrollment and drop workflows.
- Capacity validation and student rosters.

### 📊 7. System Analytics & Executive Reports
- Department metrics breakdown, student grade distribution analytics.
- Printable / Exportable PDF summary reports.

---

## 🏗️ Architecture & Technology Stack

```
   ┌───────────────────────────────────────────────────────────┐
   │                React 18 Web Application                   │
   │      Enterprise UI • Tailwind CSS • Context API Client    │
   └─────────────────────────────┬─────────────────────────────┘
                                 │ HTTP / REST API (Port 3000 -> 8080)
   ┌─────────────────────────────▼─────────────────────────────┐
   │                C++17 Crow REST API Server                 │
   │    Controllers • Domain Services • Repository Pattern     │
   └─────────────────────────────┬─────────────────────────────┘
                                 │ SQLiteCpp RAII Wrapper
   ┌─────────────────────────────▼─────────────────────────────┐
   │                       SQLite Database                     │
   └───────────────────────────────────────────────────────────┘
```

- **Backend**: C++17, Crow REST Framework, SQLiteCpp, CMake, MinGW / GCC.
- **Frontend**: React 18, Tailwind CSS, Native Web Application.

---

## 🛠️ Deployment & Execution Guide

### 1. Launch the C++ REST API Core Engine

```bash
# 1. Configure CMake
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# 2. Compile Server Executable
cmake --build build --config Release --target ums_server

# 3. Start C++ Server (Port 8080)
./build/bin/ums_server.exe
```

---

### 2. Launch the Web Frontend Server

```bash
# Navigate to frontend directory
cd frontend

# Launch Web Server (Port 3000)
node server.cjs
```

Access the application in your browser at: **`http://localhost:3000`**

---

## 🧪 Automated Testing Suite

```bash
# Configure build with testing enabled
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON

# Compile test executables
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
