#pragma once
#include <string>
#include <optional>
#include "common/types.hpp"
#include "auth/models/user.hpp"
#include "auth/repositories/i_user_repository.hpp"
#include "auth/services/password_service.hpp"
#include "auth/services/jwt_service.hpp"

namespace ums {

// WHY: DTOs (Data Transfer Objects) clearly define what inputs a service method requires,
// decoupling the service logic from HTTP-specific structures like JSON.
struct LoginRequest {
    std::string email;
    std::string password;
};

struct RegisterRequest {
    std::string email;
    std::string password;
    Role role;
};

// WHY: Groups the User object and their new token to return together upon successful authentication.
struct AuthResponse {
    User user;
    std::string token;
};

// WHY: Contains the core business logic for authentication.
// It orchestrates interactions between the repository, password service, and JWT service.
class AuthService {
public:
    AuthService(IUserRepository& userRepo, PasswordService& passwordService, JwtService& jwtService);
    
    Result<AuthResponse> login(const LoginRequest& request);
    Result<User> registerUser(const RegisterRequest& request);

private:
    IUserRepository& userRepo_;
    PasswordService& passwordService_;
    JwtService& jwtService_;
    
    // WHY: Centralizes validation logic. Returns std::nullopt on success, or an AppError on failure.
    std::optional<AppError> validateLoginRequest(const LoginRequest& request);
    std::optional<AppError> validateRegisterRequest(const RegisterRequest& request);
};

} // namespace ums
