#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "credential.h"
#include "fileUtil.h"

class Vault {
    private:
        std::vector <Credential> items;
        std::string fileName = "data/vault.txt";
    public:
        void add(const Credential& c) {
            items.push_back(c);
        }

        size_t size() const {
            return items.size();
        }

        void listAll() {
            if (items.empty()) {
                std::cout << "This vault is empty" << std::endl;
                return;
            }

            std::cout << "---- Vault Items ----" << std::endl;
            for (const auto& item : items) {
                item.display();
            }
        }

        void saveToFile(const std::string& path, const unsigned char key[crypto_secretbox_KEYBYTES]) {
            std::stringstream ss;

            // Loop through credentials vector and build one bid string
            for (const auto& item : items) {
                ss << item.getSite() << "|" << item.getUser() << "|" << item.getPass() << "\n";
            }

            std::string data = ss.str();

            // Encrypt and Base64 encode the combined string
            auto blob = FileUtil::encryptFileData(data, key);
            auto base64 = FileUtil::base64_encode(blob);

            // Write only the base64 string to the file
            std::ofstream outFile(path, std::ios::binary);
            if (outFile.is_open()) {
                outFile << base64;
                outFile.close();
            }
        }

        bool loadFromFile(const std::string& path, const unsigned char key[crypto_secretbox_KEYBYTES]) {
            // Open the file in binary mode and read its contents
            std::ifstream inFile(path, std::ios::binary);
            if (!inFile.is_open()) return true;

            // Read whole file as Base64 string
            std::string base64((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            inFile.close();
            if (base64.empty()) return true;

            base64.erase(std::remove(base64.begin(), base64.end(), '\n'), base64.end()); // clean newlines
            base64.erase(std::remove(base64.begin(), base64.end(), '\r'), base64.end()); // clean carriage returns

            // Decode + Decrypt
            try {
                auto blob = FileUtil::base64_decode(base64);
                std::string data = FileUtil::decryptFileData(blob, key);
                items.clear();
                std::stringstream dataStream(data);
                std::string line;
                while (std::getline(dataStream, line)) {
                    if (line.empty()) continue;
                    size_t p1 = line.find("|"); size_t p2 = line.find("|", p1 + 1);
                    if (p1 == std::string::npos || p2 == std::string::npos) {
                        std::cout << "Skipping corrupted line: " << line << std::endl;
                        continue;
                    }
                    return true; // success
                }
            } catch (const std::exception& e) {
                std::cerr << "Vault decrypt failed - Wrong master password or corrupted vault: " << e.what() << std::endl;
                return false;
            }
        }

        void searchBySite(const std::string query) {
            bool found = false;
            for (const auto& c : items) {
                if (c.getSite() == query || c.getUser() == query) {
                    c.display();
                    std::cout << "---------------------" << std::endl;
                    found = true;
                }
            }

            if (!found) {
                std::cout << "No Credential found " << query << std::endl;
            }
        }

        void deleteByUserName(const std::string deleteUser, const std::string& path, const unsigned char key[crypto_secretbox_KEYBYTES]) {
            items.erase(
                std::remove_if(items.begin(), items.end(), [&](const Credential& c) {
                    return c.getUser() == deleteUser;;
                }), items.end()
            );
            saveToFile(path, key);
        }
};