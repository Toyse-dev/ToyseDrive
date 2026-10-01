#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "credential.h"

class Vault {
    private:
        std::vector <Credential> items;
        std::string fileName = "vault.txt";
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

        void saveToFile() {
            std::ofstream outFile(fileName);

            if (outFile.is_open()) {
                for (const auto& item : items) {
                    outFile << item.makeString() << '\n';
                }
                outFile.close();
            }
        }

        void loadFromFile() {
            std::ifstream inFile(fileName);

            if (!inFile.is_open()) return;

            std::string line;

            while (std::getline(inFile, line)) {
                if (line.empty()) continue;
                
                size_t pos1 = line.find("|");
                size_t pos2 = line.find("|", pos1 + 1);

                if (pos1 == std::string::npos || pos2 == std::string::npos) {
                    std::cout << "Skipping corrupted line: " << line << std::endl;
                    continue;
                }

                std::string site = line.substr(0, pos1);
                std::string username = line.substr(pos1 + 1, pos2 - pos1 - 1);
                std::string password = line.substr(pos2 + 1);

                items.push_back(Credential(site, username, password));
            }
            inFile.close();
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

        void deleteByUserName(const std::string deleteUser) {
            items.erase(
                std::remove_if(items.begin(), items.end(), [&](const Credential& c) {
                    return c.getUser() == deleteUser;;
                }), items.end()
            );
            saveToFile();
        }
};