#pragma once
#include <gmock/gmock.h>
#include "auth/repositories/i_user_repository.hpp"

namespace ums::test {

class MockUserRepository : public IUserRepository {
public:
    MOCK_METHOD(Result<User>, findById, (int id), (override));
    MOCK_METHOD(Result<User>, findByEmail, (const std::string& email), (override));
    MOCK_METHOD(Result<User>, create, (const std::string& email, const std::string& passwordHash, Role role), (override));
    MOCK_METHOD((Result<std::vector<User>>), findAll, (), (override));
    MOCK_METHOD(Result<bool>, existsByEmail, (const std::string& email), (override));
};

} // namespace ums::test
