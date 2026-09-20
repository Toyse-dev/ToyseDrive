#include <iostream>
#include <string>

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

    bool writeBinary(const std::string& path, const std::string& data) {
        std::ofstream outputFile(path, std::ios::binary | std::ios::out);
        if (!outputFile) return false;

        std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(outputFile));

        return outputFile.good();
        
    }
}