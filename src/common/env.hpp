#pragma once
#include <string>
#include <cstdlib>
#include <stdexcept>

namespace ums {

// WHY: Secrets and environment-specific configuration (like DB paths and ports) MUST NOT be hardcoded.
// This ensures the code can be deployed to Dev/Test/Prod environments without modification.
// We use std::getenv as a simple, zero-dependency way to access environment variables.

inline std::string getEnv(const std::string& key, const std::string& defaultValue = "") {
    const char* val = std::getenv(key.c_str());
    if (val == nullptr) {
        return defaultValue;
    }
    return std::string(val);
}

inline std::string requireEnv(const std::string& key) {
    const char* val = std::getenv(key.c_str());
    if (val == nullptr) {
        throw std::runtime_error("Required environment variable not set: " + key);
    }
    return std::string(val);
}

} // namespace ums
