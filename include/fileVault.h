#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
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