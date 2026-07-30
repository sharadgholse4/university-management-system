#include "auth/services/jwt_service.hpp"
#include "crow/json.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <array>

namespace ums {

namespace {

// Pure C++ SHA-256 implementation
class Sha256 {
public:
    Sha256() { reset(); }

    void reset() {
        m_len = 0;
        m_size = 0;
        m_h[0] = 0x6a09e667; m_h[1] = 0xbb67ae85; m_h[2] = 0x3c6ef372; m_h[3] = 0xa54ff53a;
        m_h[4] = 0x510e527f; m_h[5] = 0x9b05688c; m_h[6] = 0x1f83d9ab; m_h[7] = 0x5be0cd19;
    }

    void update(const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            m_buf[m_size++] = data[i];
            if (m_size == 64) {
                transform();
                m_len += 512;
                m_size = 0;
            }
        }
    }

    std::array<uint8_t, 32> final() {
        m_len += m_size * 8;
        m_buf[m_size++] = 0x80;
        if (m_size > 56) {
            while (m_size < 64) m_buf[m_size++] = 0;
            transform();
            m_size = 0;
        }
        while (m_size < 56) m_buf[m_size++] = 0;
        for (int i = 7; i >= 0; --i) {
            m_buf[m_size++] = static_cast<uint8_t>(m_len >> (i * 8));
        }
        transform();

        std::array<uint8_t, 32> hash;
        for (int i = 0; i < 8; ++i) {
            hash[i * 4]     = static_cast<uint8_t>(m_h[i] >> 24);
            hash[i * 4 + 1] = static_cast<uint8_t>(m_h[i] >> 16);
            hash[i * 4 + 2] = static_cast<uint8_t>(m_h[i] >> 8);
            hash[i * 4 + 3] = static_cast<uint8_t>(m_h[i]);
        }
        return hash;
    }

private:
    static uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }

    void transform() {
        static const uint32_t K[64] = {
            0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
            0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
            0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
            0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
            0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
            0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
            0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3
        };

        uint32_t w[64];
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<uint32_t>(m_buf[i * 4]) << 24) |
                   (static_cast<uint32_t>(m_buf[i * 4 + 1]) << 16) |
                   (static_cast<uint32_t>(m_buf[i * 4 + 2]) << 8) |
                   (static_cast<uint32_t>(m_buf[i * 4 + 3]));
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        uint32_t a = m_h[0], b = m_h[1], c = m_h[2], d = m_h[3];
        uint32_t e = m_h[4], f = m_h[5], g = m_h[6], h = m_h[7];

        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + K[i] + w[i];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;

            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }

        m_h[0] += a; m_h[1] += b; m_h[2] += c; m_h[3] += d;
        m_h[4] += e; m_h[5] += f; m_h[6] += g; m_h[7] += h;
    }

    uint32_t m_h[8];
    uint8_t m_buf[64];
    size_t m_size;
    uint64_t m_len;
};

// Helper: HMAC-SHA256
std::string hmacSha256(const std::string& key, const std::string& message) {
    std::array<uint8_t, 64> k_ipad{};
    std::array<uint8_t, 64> k_opad{};

    if (key.size() > 64) {
        Sha256 sha;
        sha.update(reinterpret_cast<const uint8_t*>(key.data()), key.size());
        auto hashedKey = sha.final();
        std::copy(hashedKey.begin(), hashedKey.end(), k_ipad.begin());
        std::copy(hashedKey.begin(), hashedKey.end(), k_opad.begin());
    } else {
        std::copy(key.begin(), key.end(), k_ipad.begin());
        std::copy(key.begin(), key.end(), k_opad.begin());
    }

    for (size_t i = 0; i < 64; ++i) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    Sha256 innerSha;
    innerSha.update(k_ipad.data(), 64);
    innerSha.update(reinterpret_cast<const uint8_t*>(message.data()), message.size());
    auto innerHash = innerSha.final();

    Sha256 outerSha;
    outerSha.update(k_opad.data(), 64);
    outerSha.update(innerHash.data(), 32);
    auto outerHash = outerSha.final();

    return std::string(reinterpret_cast<const char*>(outerHash.data()), 32);
}

// Helper: Base64URL Encoding (RFC 4648)
std::string base64UrlEncode(const std::string& input) {
    static const char charSet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(charSet[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) result.push_back(charSet[((val << 8) >> (valb + 8)) & 0x3F]);
    while (result.size() % 4) result.push_back('=');

    std::string base64url;
    for (char c : result) {
        if (c == '+') base64url.push_back('-');
        else if (c == '/') base64url.push_back('_');
        else if (c != '=') base64url.push_back(c);
    }
    return base64url;
}

// Helper: Base64URL Decoding
std::string base64UrlDecode(const std::string& input) {
    std::string base64 = input;
    for (char& c : base64) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }
    while (base64.size() % 4) base64.push_back('=');

    static const std::string charSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[charSet[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : base64) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            result.push_back(char( (val >> valb) & 0xFF ));
            valb -= 8;
        }
    }
    return result;
}

} // anonymous namespace

JwtService::JwtService(std::string secret, int expirySeconds)
    : secret_(std::move(secret)), expirySeconds_(expirySeconds) {}

std::string JwtService::createToken(const JwtPayload& payload) const {
    auto now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    auto exp = now + expirySeconds_;

    // Header: {"alg":"HS256","typ":"JWT"}
    crow::json::wvalue headerJson;
    headerJson["alg"] = "HS256";
    headerJson["typ"] = "JWT";
    std::string headerB64 = base64UrlEncode(headerJson.dump());

    // Payload: {"iss":"ums","userId":"1","role":"student","subjectId":"1","exp":...,"iat":...}
    crow::json::wvalue payloadJson;
    payloadJson["iss"] = "ums";
    payloadJson["userId"] = std::to_string(payload.userId);
    payloadJson["role"] = roleToString(payload.role);
    payloadJson["subjectId"] = std::to_string(payload.subjectId);
    payloadJson["exp"] = exp;
    payloadJson["iat"] = now;
    std::string payloadB64 = base64UrlEncode(payloadJson.dump());

    std::string unsignedToken = headerB64 + "." + payloadB64;
    std::string rawSig = hmacSha256(secret_, unsignedToken);
    std::string sigB64 = base64UrlEncode(rawSig);

    return unsignedToken + "." + sigB64;
}

Result<JwtPayload> JwtService::verifyToken(const std::string& token) const {
    try {
        size_t dot1 = token.find('.');
        if (dot1 == std::string::npos) return AppError{ErrorCode::Unauthorized, "Malformed token"};
        size_t dot2 = token.find('.', dot1 + 1);
        if (dot2 == std::string::npos) return AppError{ErrorCode::Unauthorized, "Malformed token"};

        std::string headerB64 = token.substr(0, dot1);
        std::string payloadB64 = token.substr(dot1 + 1, dot2 - dot1 - 1);
        std::string sigB64 = token.substr(dot2 + 1);

        std::string unsignedToken = headerB64 + "." + payloadB64;
        std::string expectedSigRaw = hmacSha256(secret_, unsignedToken);
        std::string expectedSigB64 = base64UrlEncode(expectedSigRaw);

        if (sigB64 != expectedSigB64) {
            return AppError{ErrorCode::Unauthorized, "Invalid token signature"};
        }

        std::string payloadJsonStr = base64UrlDecode(payloadB64);
        auto payloadData = crow::json::load(payloadJsonStr);
        if (!payloadData) {
            return AppError{ErrorCode::Unauthorized, "Malformed token payload"};
        }

        auto now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        if (payloadData.count("exp") > 0 && payloadData["exp"].i() < now) {
            return AppError{ErrorCode::Unauthorized, "Token expired"};
        }

        JwtPayload payload;
        if (payloadData.count("userId") > 0) {
            std::string uId = payloadData["userId"].s();
            payload.userId = std::stoi(uId);
        }
        if (payloadData.count("role") > 0) {
            std::string r = payloadData["role"].s();
            payload.role = stringToRole(r);
        }
        if (payloadData.count("subjectId") > 0) {
            std::string sId = payloadData["subjectId"].s();
            payload.subjectId = std::stoi(sId);
        }

        return payload;
    } catch (const std::exception& e) {
        return AppError{ErrorCode::Unauthorized, std::string("Token error: ") + e.what()};
    }
}

} // namespace ums
