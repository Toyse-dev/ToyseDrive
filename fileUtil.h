#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <random>

class ReadFile {
    private:
        std::string path;
    public:
        ReadFile(const std::string& p) : path(p) {}

        std::string readBinary() {
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
};

class WriteFile {
    private:
        std::string path;
    public:
        WriteFile(std::string p) : path(p) {}

        bool writeBinary(const std::string& data) const {
            std::ofstream outputFile(path, std::ios::binary | std::ios::out);
            if (!outputFile) return false;

            std::copy(
                data.begin(), data.end(),
                std::ostreambuf_iterator<char>(outputFile)
            );
            return outputFile.good();
        }
};

class FileRecord {
    private:
        std::string id;
        std::string fileName;
        std::string base64Data;
    public:
        FileRecord(std::string i, std::string fn, std::string bd) : id(i), fileName(fn), base64Data(bd) {}

        std::string getFile() const { return fileName; }
        std::string getFileById() const { return id; }

        std::string makeString() const {
            return id + "|" + fileName + "|" + base64Data;
        }

        void display() const {
            std::cout << "ID: " << id << std::endl;
            std::cout << "File Name: " << fileName << std::endl;
        }

        std::string generateId(int length) const {
            const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

            std::random_device randomId;
            std::mt19937 gen(randomId());

            std::uniform_int_distribution<> dis(0, chars.size() - 1);

            std::string result = "";
            for (int i = 0; i < length; ++i) {
                result += chars[dis(gen)]; // pick a random character and append it
            }

            return result;
        }
};

class FileVault {
    private:
        std::vector <FileRecord> files;
        std::string fileSafe = "drive.txt";
    public:
        void add(const FileRecord& fr) {
            files.push_back(fr);
        }

        FileRecord* findById(const std::string id) {
            for (auto &fr : files) {
                if (fr.getFileById() == id) return &fr;
            }
            return nullptr;
        }

        void saveToFile() {
            std::ofstream outFile(fileSafe);

            if (outFile.is_open()) {
                for (const auto& f : files) {
                    outFile << f.makeString() << "\n";
                }
                outFile.close();
            }
        }

        void loadFromFile() {
            std::ifstream inFile(fileSafe);

            if (!inFile.is_open()) return;

            std::string line;

            while (std::getline(inFile, line)){
                if (line.empty()) continue;

                size_t pos1 = line.find("|");
                size_t pos2 = line.find("|", pos1 + 1);

                if (pos1 == std::string::npos || pos2 == std::string::npos) {
                    std::cout << "Skipping corrupted line: " << line << std::endl;
                    continue;
                }

                std::string id = line.substr(0, pos1);
                std::string fileName = line.substr(pos1 + 1, pos2 - pos1 - 1);
                std::string base64Data = line.substr(pos2 + 1);

                files.emplace_back(id, fileName, base64Data);
            }
        }
};