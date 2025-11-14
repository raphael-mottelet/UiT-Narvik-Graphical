#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

inline std::string loadTextFile(const std::string& path) {
    std::ifstream ifs(path);
    if(!ifs) throw std::runtime_error("Failed to open file: " + path);
    std::stringstream ss; ss << ifs.rdbuf();
    return ss.str();
}
