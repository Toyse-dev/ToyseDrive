#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <random>
#include <algorithm>
#include <vector>

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
}