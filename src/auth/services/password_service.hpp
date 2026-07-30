#pragma once
#include <string>

namespace ums {

// WHY: A dedicated password service abstracts away the specific hashing algorithm.
// This allows us to easily change the hashing mechanism or cost factor in the future
// without modifying the business logic that uses it.
class PasswordService {
public:
    // WHY: Default cost of 12 is a good balance between security and performance for bcrypt.
    explicit PasswordService(int cost = 12);
    
    // Hash a plaintext password using BCrypt
    std::string hash(const std::string& password) const;
    
    // Verify a plaintext password against a BCrypt hash
    // Returns true if they match
    bool verify(const std::string& password, const std::string& hash) const;

private:
    int cost_;
};

} // namespace ums
