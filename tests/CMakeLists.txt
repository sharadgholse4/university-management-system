# tests/CMakeLists.txt

# Testing is crucial for a production system. We use GoogleTest.

# Helper macro to simplify adding tests
macro(add_ums_test TEST_NAME SOURCE_FILE)
    add_executable(${TEST_NAME} ${SOURCE_FILE})
    target_include_directories(${TEST_NAME} PRIVATE 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/tests
    )
    target_link_libraries(${TEST_NAME} PRIVATE 
        ums_lib 
        GTest::gtest_main 
        GTest::gmock
    )
    
    # gtest_discover_tests automatically discovers tests in the executable and registers them with CTest.
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endmacro()

# Module 1: Auth tests
add_ums_test(test_user_model unit/auth/test_user_model.cpp)
add_ums_test(test_user_repository unit/auth/test_user_repository.cpp)
add_ums_test(test_auth_service unit/auth/test_auth_service.cpp)
add_ums_test(test_jwt_service unit/auth/test_jwt_service.cpp)
add_ums_test(test_auth_endpoints integration/test_auth_endpoints.cpp)

# Module 2: Student Profile tests
add_ums_test(test_student_model unit/student/test_student_model.cpp)
add_ums_test(test_student_repository unit/student/test_student_repository.cpp)
add_ums_test(test_student_service unit/student/test_student_service.cpp)
add_ums_test(test_student_endpoints integration/test_student_endpoints.cpp)

# Module 3: Attendance tests
add_ums_test(test_attendance_model unit/attendance/test_attendance_model.cpp)
add_ums_test(test_attendance_repository unit/attendance/test_attendance_repository.cpp)
add_ums_test(test_attendance_service unit/attendance/test_attendance_service.cpp)
add_ums_test(test_attendance_endpoints integration/test_attendance_endpoints.cpp)

# Module 4: Results / CGPA tests
add_ums_test(test_result_model unit/results/test_result_model.cpp)
add_ums_test(test_result_repository unit/results/test_result_repository.cpp)
add_ums_test(test_result_service unit/results/test_result_service.cpp)
add_ums_test(test_result_endpoints integration/test_result_endpoints.cpp)

# Module 5: Courses & Departments tests
add_ums_test(test_course_model unit/course/test_course_model.cpp)
add_ums_test(test_course_repository unit/course/test_course_repository.cpp)
add_ums_test(test_course_service unit/course/test_course_service.cpp)
add_ums_test(test_course_endpoints integration/test_course_endpoints.cpp)

# Module 6: Enrollments tests
add_ums_test(test_enrollment_model unit/enrollment/test_enrollment_model.cpp)
add_ums_test(test_enrollment_repository unit/enrollment/test_enrollment_repository.cpp)
add_ums_test(test_enrollment_service unit/enrollment/test_enrollment_service.cpp)
add_ums_test(test_enrollment_endpoints integration/test_enrollment_endpoints.cpp)

# Module 7: Analytics & Reports tests
add_ums_test(test_report_service unit/reports/test_report_service.cpp)
add_ums_test(test_report_endpoints integration/test_report_endpoints.cpp)




