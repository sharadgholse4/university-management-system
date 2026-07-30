-- sql/migrations/001_create_users.sql
-- WHY: We use migration scripts instead of letting an ORM auto-generate schemas. 
-- This gives us explicit control over index creation, constraints, and schema history.

CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    email TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    -- CHECK constraint ensures data integrity at the database level.
    role TEXT NOT NULL CHECK(role IN ('student', 'professor', 'admin')),
    created_at TEXT NOT NULL DEFAULT (datetime('now')),
    updated_at TEXT NOT NULL DEFAULT (datetime('now'))
);

-- Indexes on email and role for fast lookups.
-- Email is used for login, role is used for authorization checks.
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
CREATE INDEX IF NOT EXISTS idx_users_role ON users(role);
