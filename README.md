# 🏛️ EduPortal — University Management System

A high-performance **University Management System** featuring a **C++17 Crow REST API Backend** and an interactive **React Web Application Frontend**.

Developed by **Sharad Gholse** as a Computer Science & Engineering capstone project.

---

## 🌟 Key Features

### 🔐 1. Authentication & Role-Based Access Control
- **Sign In & Sign Up (Registration)**: Dedicated authentication flow for Students and Faculty with department and roll number enrollment.
- **Google Workspace SSO**: Google OAuth 2.0 Single Sign-On integration.
- **3-Layer Security Roles**: Enforced access control for **Students**, **Professors / Faculty**, and **System Administrators**.
- **Crypto Password Hashing & JWT**: HMAC-SHA256 JWT token verification and BCrypt password security.

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
   │                React 18 Web Application                   │
   │      Responsive UI • Tailwind CSS • Context API State     │
   └─────────────────────────────┬─────────────────────────────┘
                                 │ REST API Calls (/api)
   ┌─────────────────────────────▼─────────────────────────────┐
   │                C++17 Crow REST API Server                 │
   │    Controllers • Domain Services • Data Access Layer      │
   └─────────────────────────────┬─────────────────────────────┘
                                 │ SQLiteCpp RAII Wrapper
   ┌─────────────────────────────▼─────────────────────────────┐
   │                       SQLite Database                     │
   └───────────────────────────────────────────────────────────┘
```

- **Backend**: C++17, Crow REST Framework, SQLiteCpp, CMake
- **Frontend**: React 18, Tailwind CSS, Web Standard ES Modules
- **Database**: Embedded SQLite3 Engine
- **DevOps**: Multi-stage Dockerfile, Vercel CDN static hosting

---

## 🚀 Quick Start Guide

### 1. Build and Run the C++ Backend (Port 8080)

```bash
# Configure CMake build directory
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Compile C++ REST API server
cmake --build build --config Release --target ums_server

# Launch backend server
./build/bin/ums_server.exe
```

The C++ REST API will start listening on `http://localhost:8080/api`.

---

### 2. Launch the Web Frontend (Port 3000)

```bash
# Navigate to frontend directory
cd frontend

# Launch Node server
node server.cjs
```

Open your browser at **`http://localhost:3000`** to access EduPortal!

---

## 🧪 Automated Testing

```bash
# Build GoogleTest suites
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --config Release --target test_user_model test_jwt_service test_student_model test_attendance_model test_result_model test_course_model test_enrollment_model

# Execute tests
./build/bin/test_user_model.exe
./build/bin/test_jwt_service.exe
./build/bin/test_student_model.exe
```

---

## 📄 License
This project is open source and available under the [MIT License](LICENSE).
