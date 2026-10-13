#pragma once
#include <sodium.h>
#include <string>
#include <fstream>
#include <filesystem>

namespace ToyseCrypto {
    constexpr size_t SALT_BYTES = crypto_pwhash_SALTBYTES; // 16
    constexpr size_t KEY_BYTES = crypto_secretbox_KEYBYTES; // 32

    inline bool loadOrCreateSalt(const std::string& path, unsigned char salt[SALT_BYTES]) {
        std::ifstream::create_directories(std::filesystem::path(path).parent_path());
        std::ifstream in(path, std::ios::binary);
        if (in.good()) {
            in.read(reinterpret_cast<char*>(salt), SALT_BYTES);
            return in.gcount() == SALT_BYTES;
        }
        randombytes_buf(salt, SALT_BYTES);
        std::ofstream out(path, std::ios::binary);
        out.write(reinterpret_cast<char*>(salt), SALT_BYTES);
        return out.good();
    }

    inline bool deriveKey(const std::string& password, const unsigned char salt[SALT_BYTES], unsigned char key[KEY_BYTES]) {
        // OPSLIMIT_MODERATE + MEMLIMIT_MODERATE = Argon2id
        return crypto_pwhash(key, KEY_BYTES, password.c_str(), password.size(), salt, crypto_pwhash_OPSLIMIT_MODERATE,
                            crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT) == 0;
    }

    inline void secureWipe(void* ptr, size_t len) {
        sodium_memzero(ptr, len);
    }
}