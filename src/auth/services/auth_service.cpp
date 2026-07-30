#include "auth/services/auth_service.hpp"
#include "common/errors.hpp"

namespace ums {

AuthService::AuthService(IUserRepository& userRepo, PasswordService& passwordService, JwtService& jwtService)
    : userRepo_(userRepo), passwordService_(passwordService), jwtService_(jwtService) {}

std::optional<AppError> AuthService::validateLoginRequest(const LoginRequest& request) {
    if (request.email.empty() || request.password.empty()) {
        return AppError{ErrorCode::ValidationError, "Email and password are required"};
    }
    return std::nullopt;
}

std::optional<AppError> AuthService::validateRegisterRequest(const RegisterRequest& request) {
    if (request.email.empty() || request.password.empty()) {
        return AppError{ErrorCode::ValidationError, "Email and password are required"};
    }
    // Validate email format by attempting to create a User
    // WHY: We reuse User::create()'s email validation to keep the rules in one place
    auto emailCheck = User::create(request.email, "validation_check", request.role);
    if (!isSuccess(emailCheck)) {
        return getError(emailCheck);
    }
    if (request.password.length() < 8) {
        return AppError{ErrorCode::ValidationError, "Password must be at least 8 characters"};
    }
    return std::nullopt;
}

Result<AuthResponse> AuthService::login(const LoginRequest& request) {
    // 1. Validate inputs
    if (auto err = validateLoginRequest(request)) {
        return *err;
    }

    // 2. Find user
    auto userResult = userRepo_.findByEmail(request.email);
    if (!isSuccess(userResult)) {
        // SECURITY: Return generic message for both "user not found" and "wrong password"
        // WHY: This prevents an attacker from using the login endpoint to enumerate 
        // valid email addresses in the system.
        return AppError{ErrorCode::Unauthorized, "Invalid credentials"};
    }
    const auto& user = getValue(userResult);

    // 3. Verify password
    if (!passwordService_.verify(request.password, user.passwordHash())) {
        // SECURITY: Same message as above.
        return AppError{ErrorCode::Unauthorized, "Invalid credentials"};
    }

    // 4. Generate token
    // Note: subjectId is 0 initially, will be populated if/when user gets a student/prof profile
    JwtPayload payload{user.id(), user.role(), 0};
    std::string token = jwtService_.createToken(payload);

    return AuthResponse{user, token};
}

Result<User> AuthService::registerUser(const RegisterRequest& request) {
    // 1. Validate inputs
    if (auto err = validateRegisterRequest(request)) {
        return *err;
    }

    // 2. Check for existing user
    auto existsResult = userRepo_.existsByEmail(request.email);
    if (!isSuccess(existsResult)) {
        return getError(existsResult);
    }
    if (getValue(existsResult)) {
        // WHY: Email must be unique.
        return AppError{ErrorCode::Conflict, "User with this email already exists"};
    }

    // 3. Hash password
    std::string hashedPw = passwordService_.hash(request.password);

    // 4. Create user in database
    return userRepo_.create(request.email, hashedPw, request.role);
}

} // namespace ums
