// Using of IStorage interface now, which will later give the unlimited storage with TeraBox/IDrive
#pragma once
#include "../crypto/fileCrypto.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

namespace ToyseCore {
    // Abstract storage
    class IStorage {
        public:
            virtual ~IStorage() = default;
            virtual bool save(const std::string& id, const std::vector<unsigned char>& blob) = 0;
            virtual std::vector<unsigned char> load(const std::string& id) = 0;
            virtual bool remove(const std::string& id) = 0;
            virtual std::vector<std::string> listIds() = 0;
            virtual std::string getName() const = 0; // "local" or "terabox" or "IDrive"

    };

    // v2.0 Beta - Local disk, limited by user disk
    class LocalStorage : public IStorage {
        std::string baseDir;
    public:
        explicit LocalStorage(const std::string& dir = "data/files") : baseDir(dir) {
            std::filesystem::create_directories(baseDir);
        }

        bool save(const std::string& id, const std::vector<unsigned char>& blob) override {
            std::string path = baseDir + "/" + id + ".enc";
            std::string tmp = path + ".tmp";
            std::ofstream output(tmp, std::ios::binary);
            if (!output) return false;
            output.write(reinterpret_cast<const char*>(blob.data()), blob.size());
            output.close();
            if (!output.good()) return false;
            std::filesystem::rename(tmp, path); // atomic
            return true;
        }

        std::vector<unsigned char> load(const std::string& id) override {
            std::string path = baseDir + "/" + id + ".enc";
            std::ifstream input(path, std::ios::binary | std::ios::ate);
            if (!input) throw std::runtime_error("file not found: " + id);
            size_t size = input.tellg();
            input.seekg(0);
            std::vector<unsigned char> buf(size);
            input.read(reinterpret_cast<char*>(buf.data()), size);
            return buf;
        }

        bool remove(const std::string& id) override {
            return std::filesystem::remove(basDir + "/" + id + ".enc");
        }

        std::vector<std::string> listIds() override {
            std::vector<std::string> ids;
            if (!std::filesystem::exists(baseDir)) return ids;
            for (auto& p : std::filesystem::directory_iterator(baseDir)) {
                if (p.path().extension() == ".enc")
                    ids.push_back(p.path().stem().string());
            }
            return ids;
        }

        std::string getName() const override { return "local"; }
    }

    // v2.1 Future - Unlimited storage via TeraBox/IDrive API
    // You will implement this later without touching other files
    // class CloudStorage : public IStorage {
    //     bool save(...) override { // upload to TeraBox API }
    //     vector<unsigned char> load(...) override { // download from TeraBox }
    //     string getName() const override { return "terabox"; }
    // };

    class StorageFactory {
        public:
            static std::unique_ptr<IStorage> create(const std::string& type = "local") {
                if (type == "local") return std::make_unique<LocalStorage>("data/files");
                // if(type == "terabox") return std::make_unique<CloudStorage>();
                throw std::runtime_error("unknown storage type");
            }
    };

    // File metadata
    class FileRecord {
        std::string id;
        std::string fileName;
        size_t originalSize;
        std::string mimeType;

        public:
            FileRecord(std::string i, std::string n, size_t s) : id(std::move(i)),
                fileName(std::move(n)), originalSize(s) {}
            
            const std::string& getId() const { return id; }
            const std::string& getName() const { return fileName; }
            size_t getSize() const { return originalSize; }

            std::string toString() const {
                return id + " | " + fileName + " (" + std::to_string(originalSize) + " bytes)";
            }
    };
}