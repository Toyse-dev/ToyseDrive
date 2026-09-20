#include <iostream>
#include <string>
#include <iterator>
#include <random>

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