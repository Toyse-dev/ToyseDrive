#pragma once
#include <iostream>
#include <sodium.h>
#include <fstream>
#include <string>
#include <iterator>
#include <random>
#include <algorithm>
#include <vector>
#include "tinyfiledialogs.h"

namespace FileUtil {
    std::string readBinary(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << path << std::endl;
            return "";
        }

        std::string rawBytes{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };

        std::cout << "Read " << rawBytes.size() << " bytes from file: " << path << std::endl;
        return rawBytes;
    }

    inline bool writeBinary(const std::string& path, const std::string& data) {
        std::ofstream outputFile(path, std::ios::binary | std::ios::out);
        if (!outputFile) return false;
        outputFile.write(data.data(), data.size());
        return outputFile.good();
    }

    inline std::string generateId(int length = 8) {
        const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, chars.size() - 1);
        std::string result;
        for (int i = 0; i < length; ++i) result += chars[dis(gen)];
        return result;
    }

    const std::string base64Alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    inline std::string base64_encode(const std::string& raw) {
        std::string output;
        int val = 0, valb = -6;

        for (unsigned char c : raw) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                output.push_back(base64Alpha[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) output.push_back(base64Alpha[((val << 8) >> (valb + 8)) & 0x3F]);
        while (output.size() % 4) output.push_back('=');
        return output;
    }

    inline std::string base64_decode(const std::string& encoded) {
        std::vector<int> T(256, -1);
        for (int i = 0; i < 64; i++) T[base64Alpha[i]] = i;

        std::string outFile;
        int val = 0, valb = -8;
        for (unsigned char c : encoded) {
            if (T[c] == -1) continue;

            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                outFile.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return outFile;
    }

    inline std::string openFileDialog() {
        const char* filters[] = {"*.pdf", "*.jpg", "*.png", "*.zip", "*.*"};
        const char* filterDesc = "Supported files";

        const char* file = tinyfd_openFileDialog(
            "Select file to upload to ToyseDrive", // title
            "", // default path
            5, // number of filters
            filters,
            filterDesc,
            0 // allow multiple = 0
        );

        if (!file) return ""; // user pressed cancel
        return std::string(file);
    }

    inline std::string saveFileDialog(const std::string& defaultName) {
        const char* filters[] = {"*.pdf", "*.jpg", "*.png", "*.*"};
        const char* file = tinyfd_saveFileDialog(
            "Save file as",
            defaultName.c_str(),
            4,
            filters,
            NULL
        );
        if (!file) return "";
        return std::string(file);
    }

    // Slat Manager
    inline void loadOrCreateSalt(const std::string& path, unsigned char salt[crypto_pwhash_SALTBYTES]) {
        std::ifstream in(path, std::ios::binary);
        if (in) {
            in.read(reinterpret_cast<char*>(salt), crypto_pwhash_SALTBYTES);
            if (in.gcount() == crypto_pwhash_SALTBYTES){
                return; // loaded existing salt
            }
        }

        // Create new salt - first run
        randombytes_buf(salt, crypto_pwhash_SALTBYTES);
        std::ofstream out(path, std::ios::binary);
        if (!out) throw std::runtime_error("Failed to create salt file. Does data/ folder exist?");
        out.write(reinterpret_cast<char*>(salt), crypto_pwhash_SALTBYTES);
    }

    void deriveKey(const std::string& password, const unsigned char salt[crypto_pwhash_SALTBYTES], unsigned char outKey[crypto_secretbox_KEYBYTES]) {
        // Ensure libsodium is initialized before calling cryptographic functions
        if (sodium_init() < 0) throw std::runtime_error("Failed to initialize libsodium.");

        // Call crypto_pwhash to fill outKey with 32 bytes
        if (crypto_pwhash(outKey, crypto_secretbox_KEYBYTES, password.c_str(), password.length(),
            salt, crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE,
            crypto_pwhash_ALG_DEFAULT) != 0) {
                throw std::runtime_error("Key derivation failed (likely out of memory).");
        }
    }

    std::string encryptFileData(const std::string& raw, const unsigned char key[crypto_secretbox_KEYBYTES]) {
        // Allow space for Nonce + Ciphertext (Rawtext size + Mac size)
        size_t ciphertext_len = raw.size() + crypto_secretbox_MACBYTES;
        std::vector<unsigned char> output(crypto_secretbox_NONCEBYTES + ciphertext_len);

        // Generate a random nonce directly into the beginning of the output buff
        unsigned char* nonce_ptr = output.data();
        randombytes_buf(nonce_ptr, crypto_secretbox_NONCEBYTES);

        // Encrypt the raw text into the space right after the nonce
        unsigned char* ciphertext_ptr = output.data() + crypto_secretbox_NONCEBYTES;

        int result = crypto_secretbox_easy(
            ciphertext_ptr, reinterpret_cast<const unsigned char*>(raw.data()),
            raw.size(),nonce_ptr, key
        );
        
        if (result != 0) throw std::runtime_error("Encryption failed.");
        
        return std::string(reinterpret_cast<char*>(output.data()), output.size());
    }
    
    std::string decryptFileData(const std::string& cipherBlob, const unsigned char key[crypto_secretbox_KEYBYTES]) {
        // Validation: must be at least large enough to hold a Nonce and Mac
        if (cipherBlob.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
            throw std::invalid_argument("Ciphertext is too short or malformed.");
        }

        // Extract pointers for the nonce and the actual ciphertext
        const unsigned char* nonce_ptr = reinterpret_cast<const unsigned char*>(cipherBlob.data());
        const unsigned char* ciphertext_ptr = nonce_ptr + crypto_secretbox_NONCEBYTES;
        size_t ciphertext_len = cipherBlob.size() - crypto_secretbox_NONCEBYTES;

        // Allocate space for the decrypted plaintext
        size_t plaintext_len = ciphertext_len - crypto_secretbox_MACBYTES;
        std::vector<unsigned char> plaintext(plaintext_len);

        // Decrypt and verify
        int result = crypto_secretbox_open_easy(
            plaintext.data(), ciphertext_ptr,
            ciphertext_len, nonce_ptr, key
        );

        if (result != 0) {
            throw std::runtime_error("Decryption failed. Data may be corrupted or corrupted key.");
        }

        return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext.size());
    }
}