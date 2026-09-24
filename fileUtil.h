#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <random>
#include <algorithm>

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
}