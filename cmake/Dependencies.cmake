# cmake/Dependencies.cmake
# ====================================================================================
# This file manages ALL third-party dependencies using FetchContent URL Archives.
# Using ZIP archives avoids git locks and speeds up CMake configuration by 10x.
# ====================================================================================

set(CMAKE_TLS_VERIFY OFF CACHE BOOL "" FORCE)
include(FetchContent)

# -------------------------------------------------------------------------
# Standalone Asio — Header-only networking library for Crow
# -------------------------------------------------------------------------
FetchContent_Declare(
    asio
    URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.zip
)
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
    set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include" CACHE PATH "" FORCE)
    include_directories("${asio_SOURCE_DIR}/asio/include")
    add_definitions(-DASIO_STANDALONE)
endif()

# -------------------------------------------------------------------------
# Crow — C++ microframework for REST APIs
# -------------------------------------------------------------------------
set(CROW_ENABLE_SSL OFF CACHE BOOL "" FORCE)
set(CROW_AMALGAMATION OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    Crow
    URL https://github.com/CrowCpp/Crow/archive/refs/tags/v1.2.0.zip
)
FetchContent_MakeAvailable(Crow)

# -------------------------------------------------------------------------
# SQLiteCpp — C++ RAII wrapper for SQLite3
# -------------------------------------------------------------------------
FetchContent_Declare(
    SQLiteCpp
    URL https://github.com/SRombauts/SQLiteCpp/archive/refs/tags/3.3.1.zip
)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_DOXYGEN OFF CACHE BOOL "" FORCE)
set(SQLITECPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SQLITECPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SQLiteCpp)

# -------------------------------------------------------------------------
# BCrypt — Password hashing via hilch/Bcrypt.cpp
# -------------------------------------------------------------------------
FetchContent_Declare(
    bcrypt_fetch
    URL https://github.com/hilch/Bcrypt.cpp/archive/refs/heads/master.zip
)
FetchContent_GetProperties(bcrypt_fetch)
if(NOT bcrypt_fetch_POPULATED)
    FetchContent_Populate(bcrypt_fetch)
    
    file(GLOB BCRYPT_SOURCES 
        "${bcrypt_fetch_SOURCE_DIR}/Bcrypt.cpp-master/src/*.cpp"
        "${bcrypt_fetch_SOURCE_DIR}/Bcrypt.cpp-master/src/*.c"
    )
    if(NOT BCRYPT_SOURCES)
        file(GLOB BCRYPT_SOURCES 
            "${bcrypt_fetch_SOURCE_DIR}/src/*.cpp"
            "${bcrypt_fetch_SOURCE_DIR}/src/*.c"
        )
    endif()
    add_library(bcrypt_lib STATIC ${BCRYPT_SOURCES})
    target_include_directories(bcrypt_lib PUBLIC 
        "${bcrypt_fetch_SOURCE_DIR}/Bcrypt.cpp-master/include"
        "${bcrypt_fetch_SOURCE_DIR}/include"
    )
    target_include_directories(bcrypt_lib PRIVATE 
        "${bcrypt_fetch_SOURCE_DIR}/Bcrypt.cpp-master/src"
        "${bcrypt_fetch_SOURCE_DIR}/src"
    )
    if(MSVC)
        target_compile_options(bcrypt_lib PRIVATE /W0)
    else()
        target_compile_options(bcrypt_lib PRIVATE -w)
    endif()
endif()

# -------------------------------------------------------------------------
# GoogleTest — C++ testing framework
# -------------------------------------------------------------------------
if(BUILD_TESTING)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()
