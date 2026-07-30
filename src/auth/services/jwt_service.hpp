#pragma once
#include <string>
#include "common/types.hpp"
#include "common/errors.hpp"

namespace ums {

// WHY: Represents the data embedded inside a JWT. Extracting this into a struct
// makes it easier to pass around and ensures type safety compared to passing raw JSON.
struct JwtPayload {
    int userId;
    Role role;
    int subjectId; // student_id or professor_id (0 for admin)
};

// WHY: Centralizes JWT generation and validation logic.
// This prevents token manipulation vulnerabilities by ensuring a consistent
// implementation of signing and verification across the application.
class JwtService {
public:
    // WHY: Passing the secret and expiry allows them to be configured via environment variables.
    JwtService(std::string secret, int expirySeconds = 3600);
    
    // Generates a signed JWT for the given payload
    std::string createToken(const JwtPayload& payload) const;
    
    // Verifies a JWT and extracts the payload
    // Returns AppError if the token is invalid, expired, or tampered with
    Result<JwtPayload> verifyToken(const std::string& token) const;

private:
    std::string secret_;
    int expirySeconds_;
};

} // namespace ums
