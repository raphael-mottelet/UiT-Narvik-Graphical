#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

// read a whole text file into a single string
inline std::string loadTextFile(const std::string& path) {
    // open the file (text mode)
    std::ifstream ifs(path);
    // if opening failed, report which path and stop
    if(!ifs) throw std::runtime_error("Failed to open file: " + path);
    // read everything into a stringstream, then get the string
    std::stringstream ss; ss << ifs.rdbuf();
    // return file contents
    return ss.str();
}
