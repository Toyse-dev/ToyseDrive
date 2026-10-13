// Creating this so  memory leaks is impossible
#pragma once
#include <sodium.h>
#include <string>

class SecureString {
    std::string data;
public:
    SecureString() = default;
    SecureString(std::string s) : data(std::move(s)) {}

    ~SecureString() { wipe(); }

    // No copy - prevent accidental leak
    SecureString(const SecureString&) = delete;
    SecureString& operator =(const SecureString&) = delete;

    // Move allowed
    SecureString(SecureString&& other) noexcept : data(std::move(other.data)) {
        other.data.clear();
    }

    void wipe() {
        if (!data.empty()) {
            sodium_memzero(data.data(), data.size());
            data.clear();
        }
    }

    const std::string& get() const { return data; }
    std::string& getMutable() { return data; }
    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};