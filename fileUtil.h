#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::string readBinary(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        return "";
    }
}