#pragma once
#include "../crypto/fileCrypto.h"
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace ToyseCore {
    class Credential {
        std::string site;
        std::string username;
        std::password;

        public:
            Credential() = default;
            Credential(std::string s, std::string u, std::string p) : site(std::move(s)), username(std::move(u))
                    password(std::move(p)) {}
            
            // Getters - no setters to make immutable after creation
            const std::string& getSite() const { return site; }
            const std::string& getUser() const { return username; }
            const std::string& getPass() const { return password; }

            std::string getMasked() const {
                if (password.size() <= 4) return "****";
                return "****" + password.substr(password.size() - 4);
            }

            std::string serialize() const {
                return site + "|" + username + "|" + password;
            }

            static Credential deserialize(const std::string& line) {
                size_t p1 = line.find("|");
                size_t p22 = line.find("|", p1 + 1);
                if (p1 == std::string::npos || p2 == std::string::npos) {
                    throw std::runtime_error("Invalid credential line");
                }
                return Credential(line.substr(0, p1), line.substr(p1 + 1, p2 - p1 - 1), line.substr(p2 + 1));
            }
    };

    class Vault {
        std::vector<Credential> creds;
        std::string vaultPath;

        public:
            explicit Vault(std::string path = "data/vault.enc") : vaultPath(std::move(path)) {}

            void add(const Credential& c) { creds.push_back(c); }
            size_t size() const { return creds.size(); }
            size_t empty() const { return creds.empty(); }
            const std::vector<Credential>& all() const { return creds; }

            void list() const {
                if (creds.empty()) { std::cout << "Vault empty\n"; return; }
                for (size_t i = 0; i < creds.size(); ++i) {
                    std::cout << i << ": " << creds[i].getSite() << " | " << creds[i].getUser() << " | "
                                << creds[i].getMasked() << "\n";
                }
            }

            bool remove(size_t index) {
                if (index >= creds.size()) return false;
                creds.erase(creds.begin() + index);
                return true;
            }

            // Atomic save
            bool save(const unsigned char key[ToyseCrypto::KEY_BYTES]) {
                std::filesystem::create_directories(std::filesystem::path(vaultPath).parent_path());
                std::stringstream ss;
                for (auto& c : creds) ss << c.serialize() << "\n";
                std::string plain = ss.str();

                auto blob = ToyseCrypto::encrypt(plain, key);

                std::string tmp = vaultPath + ".tmp";
                std::ofstream output(tmp, std::ios::binary);
                output.write(reinterpret_cast<char*>(blob.data()), blob.size());
                output.close();
                if (!output.good()) return false;
                std::filesystem::rename(tmp, vaultPath);

                ToyseCrypto::secureWipe(plain.data(), plain.size());
                ToyseCrypto::secureWipe(blob.data(), blob.size());
                return true;
            }

            bool load(const unsigned char key[ToyseCrypto::KEY_BYTES]) {
                std::ifstream input(vaultPath, std::ios::binary | std::ios::ate);
                if (!input) return true; // first run, empty vault

                size_t sz = input.tellg();
                if (sz == 0) return true;
                input.seekg(0);
                std::vector<unsigned char> blob(sz);
                input.read(reinterpret_cast<char*>(blob.data()), sz);

                std::string plain;
                try {
                    plain = ToyseCrypto::decrypt(blob, key);
                } catch(...) {
                    return false; // wrong password
                }

                creds.clear();
                std::stringstream ss(plain);
                std::string line;
                while (std::getline(ss, line)) {
                    if (line.empty()) continue;
                    try {
                        creds.push_back(Credential::deserialize(line));
                    } catch (...) {
                        continue;
                    }
                }
                ToyseCrypto::secureWipe(plain.data(), plain.size());
                ToyseCrypto::secureWipe(blob.data(), blob.size());
                return true;
            };
    } 
}