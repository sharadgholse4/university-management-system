# src/CMakeLists.txt

add_compile_definitions(_WIN32_WINNT=0x0A00)
add_compile_definitions(ASIO_STANDALONE)

# Create a STATIC library containing all business logic and infrastructure code.
# This design choice allows both the main executable and the test executables to link 
# against the exact same compiled code, ensuring tests reflect production behavior exactly.
add_library(ums_lib STATIC
    # We include all source files here. Header files don't need to be listed.
    # Module 0: Infrastructure
    database/database_manager.cpp
    # Module 1: Auth
    auth/repositories/sqlite_user_repository.cpp
    auth/services/password_service.cpp
    auth/services/jwt_service.cpp
    auth/services/auth_service.cpp
    auth/controllers/auth_controller.cpp
    # Module 2: Student Profile
    student/repositories/sqlite_student_repository.cpp
    student/services/student_service.cpp
    student/controllers/student_controller.cpp
    # Module 3: Attendance
    attendance/repositories/sqlite_attendance_repository.cpp
    attendance/services/attendance_service.cpp
    attendance/controllers/attendance_controller.cpp
    # Module 4: Results / CGPA
    results/repositories/sqlite_result_repository.cpp
    results/services/result_service.cpp
    results/controllers/result_controller.cpp
    # Module 5: Courses & Departments
    course/repositories/sqlite_department_repository.cpp
    course/repositories/sqlite_course_repository.cpp
    course/services/course_service.cpp
    course/controllers/course_controller.cpp
    # Module 6: Enrollments
    enrollment/repositories/sqlite_enrollment_repository.cpp
    enrollment/services/enrollment_service.cpp
    enrollment/controllers/enrollment_controller.cpp
    # Module 7: Analytics & Reports
    reports/services/report_service.cpp
    reports/controllers/report_controller.cpp
)

# Set include directories
target_include_directories(ums_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ASIO_INCLUDE_DIR}
)

# Link dependencies
target_link_libraries(ums_lib PUBLIC
    Crow::Crow
    SQLiteCpp
    bcrypt_lib
    bcrypt
    ws2_32
    mswsock
)

# Create the main executable
add_executable(ums_server main.cpp)

# Link the executable against our static library
target_link_libraries(ums_server PRIVATE ums_lib)
