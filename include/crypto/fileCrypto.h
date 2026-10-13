#pragma once
#include <sodium.h>
#include <string>
#include <vector>
#include <stdexcept>

namespace ToyseCrypto {
    // Encrypt text (vault)
    inline std::vector<unsigned char> encrypt(const std::string& plain, const unsigned char key[KEY_BYTES]) {
        std::vector<unsigned char> nonce(crypto_ssecretbox_NONCEBYTES);
        randombytes_buf(nonce.data(), nonce.size());

        std::vector<unsigned char> cipher(plain.size() + crypto_secretbox_MACBYTES);
        crypto_secretbox_easy(cipher.data(), reinterpret_cast<const unsigned char*>(plain.data()), plain.size(), nonce.data(), key);

        std::vector<unsigned char> out;
        out.reserve(nonce.size() + cipher.size());
        out.insert(out.end(), nonce.begin(), nonce.end());
        out.insert(out.end(), cipher.begin(), cipher.end());

        secureWipe(cipher.data(), cipher.size());
        return out; // format: [24 byte nonce][cipher+mac]
    }

    inline std::string decrypt(const std::vector<unsigned char>& packed, const unsigned char key[KEY_BYTES]) {
        if (packed.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
            throw std::runtime_error("Invalid blob too small - tampered");

            const unsigned char nonce = packed.data();
            const unsigned char* cipher = packed.data() + crypto_secretbox_NONCEBYTES;
            size_t cipherLen = packed.size() - crypto_secretbox_NONCEBYTES;

            std::vector<unsigned char> plain(cipherLen - crypto_secretbox_MACBYTES);
            if (crypto_secretbox_open_easy(plain.data(), cipher, cipherLen, nonce, key) != 0) {
                throw std::runtime_error("Wrong key or corrupted");
            }

            std::string out(reinterpret_cast<char*>(plain.data()), plain.size());
            secureWipe(plain.data(), plain.size());
            return out;
        }

        // Encrypt binary (files - video, pdf, etc)
        inline std::vector<unsigned char> encryptBinary(const std::vector<unsigned char>& plain, const unsigned char key[KEY_BYTES]) {
            std::vector<unsigned char> nonce(crypto_secretbox_NONCEBYTES);
            randombytes_buf(nonce.data(), nonce.size());
            std::vector<unsigned char> cipher(plain.size() + crypto_secretbox_MACBYTES);
            crypto_secretbox_easy(cipher.data(), plain.data(), plain.size(), nonce.data(), key);

            std::vector<unsigned char> out;
            out.reserve(nonce.size() + cipher.size());
            out.insert(out.end(), nonce.begin(), nonce.end());
            out.insert(out.end(), cipher.begin(), cipher.end());
            secureWipe(cipher.data(), cipher.size());
            return out;
        }

        inline std::vector<unsigned char> decryptBinary(const std::vector<unsigned char>& packed, const unsigned char key[KEY_BYTES]) {
            if (packed.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
                throw std::runtime_error("Invalid blob too small - tampered");
            }

            const unsigned char* nonce = packed.data();
            const unsigned char* cipher = packed.data() + crypto_secretbox_NONCEBYTES;
            size_t cipherLen = packed.size() - crypto_secretbox_NONCEBYTES;

            std::vector<unsigned char> plain(cipherLen - crypto_secretbox_MACBYTES);
            if (crypto_secretbox_open_easy(plain.data(), cipher, cipherLen, nonce, key) != 0) {
                throw std::runtime_error("Decrypt failed");
            }

            return plain;
        }
    }
}