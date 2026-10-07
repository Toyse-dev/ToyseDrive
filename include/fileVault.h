#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sodium.h>
#include "fileRecord.h"

class FileVault {
    private:
        std::vector <FileRecord> files;
        std::string fileSafe = "data/drive.txt";
    public:
        void add(const FileRecord& fr) {
            files.push_back(fr);
        }

        FileRecord* findById(const std::string& id) {
            for (auto &fr : files) {
                if (fr.getFileById() == id) return &fr;
            }
            return nullptr;
        }

        void saveToFile(const std::string& path, const unsigned char key[crypto_secretbox_KEYBYTES]) {
            std::stringstream ss;
            for (auto& f : files) {
                ss << f.getFileById() << "|" << f.getFile() << "|" << f.getData() << "\n";
            }
            std::string data = ss.str();
            auto blob = FileUtil::encryptFileData(data, key);
            auto base64 = FileUtil::base64_encode(blob);

            std::ofstream outFile(path, std::ios::binary);
            outFile << base64;

            sodium_memzero(data.data(), data.size()); // Securely wipe the plaintext data from memory
            sodium_memzero(blob.data(), blob.size()); // Securely wipe the encrypted blob from memory
        }

        bool loadFromFile(const std::string& path, const unsigned char key[crypto_secretbox_KEYBYTES]) {
            std::ifstream inFile(path, std::ios::binary);

            if (!inFile.is_open()) return true; // file doesn't exist yet, treat as empty vault

            std::string base64((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            inFile.close();
            if (base64.empty()) return true; // empty file, treat as empty vault

            base64.erase(std::remove(base64.begin(), base64.end(), '\n'), base64.end()); // remove newlines
            base64.erase(std::remove(base64.begin(), base64.end(), '\r'), base64.end()); // remove carriage returns

            std::string data;
            try {
                auto blob = FileUtil::base64_decode(base64);
                data = FileUtil::decryptFileData(blob, key);
            } catch (const std::exception& e) {
                return false;
            }

            files.clear();
            std::stringstream ss(data);
            std::string line;
            while (std::getline(ss, line)){
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
            sodium_memzero(data.data(), data.size()); // Securely wipe the plaintext data from memory
            return true; // success
        }

        bool deleteById(const std::string& id) {
            size_t before = files.size();
            files.erase(
                std::remove_if(files.begin(), files.end(),
                    [&](const FileRecord& fr){ return fr.getFileById() == id; }),
                files.end()
            );
            return files.size() != before;
        }

        size_t size() const { return files.size(); }
        void listAll() { for(auto &fr : files) fr.display(); }
};