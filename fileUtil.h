#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>

class ReadFile {
    private:
        std::string path;
    public:
        ReadFile(const std::string& p) : path(p) {}

        std::string readBinary() {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
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
};

class WriteFile {
    private:
        std::string path;
    public:
        WriteFile(std::string p) : path(p) {}

        bool writeBinary(const std::string& data) const {
            std::ofstream outputFile(path, std::ios::binary | std::ios::trunc);
            if (!outputFile) return false;

            std::copy(
                data.begin(), data.end(),
                std::ostreambuf_iterator<char>(outputFile)
            );
            return outputFile.good();
        }
};