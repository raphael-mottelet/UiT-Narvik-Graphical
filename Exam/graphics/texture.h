#pragma once
#include <glad/glad.h>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>

namespace pgm_detail {

// Skip whitespace and comment lines starting with '#' in a PGM file stream
inline void skipWSAndComments(std::istream& is){
    for(;;){
        while (std::isspace(is.peek())) is.get();
        if (is.peek() == '#'){
            std::string dummy;
            std::getline(is, dummy);
            continue;
        }
        break;
    }
}

// Read a single integer token from a PGM file, skipping whitespace and comments
inline bool readInt(std::istream& is, int& val){
    skipWSAndComments(is);
    if(!is.good()) return false;
    is >> val;
    return static_cast<bool>(is);
}

// Read raw bytes from the stream into a buffer, used for P5 pixel data
inline bool readBytes(std::istream& is, unsigned char* dst, size_t n){
    is.read(reinterpret_cast<char*>(dst), std::streamsize(n));
    return is.good();
}

} // namespace pgm_detail

// Load a grayscale PGM (P2 ASCII or P5 binary) and normalize values to 0..255 in row-major order
inline bool loadPGM(const char* path, std::vector<unsigned char>& out, int& w, int& h){
    using namespace pgm_detail;

    std::ifstream fs(path, std::ios::binary);
    if(!fs.is_open()){
        std::cerr << "Failed to open PGM file: " << path << "\n";
        return false;
    }

    char m0=0, m1=0;
    fs >> m0 >> m1;
    if(!fs || m0!='P' || (m1!='2' && m1!='5')){
        std::cerr << "Not a P2/P5 PGM file: " << path << "\n";
        return false;
    }

    int width=0, height=0, maxv=0;

    if(m1=='2'){ // ASCII P2 format with text-based pixel values
        if(!readInt(fs, width))  return false;
        if(!readInt(fs, height)) return false;
        if(!readInt(fs, maxv))   return false;
        if(width<=0 || height<=0 || maxv<=0) return false;

        out.resize(size_t(width)*size_t(height));
        for(int j=0; j<height; ++j){
            for(int i=0; i<width; ++i){
                int v=0;
                if(!readInt(fs, v)) return false;
                if(v<0) v=0;
                if(v>maxv) v=maxv;
                unsigned char g = (maxv==255)
                    ? static_cast<unsigned char>(v)
                    : static_cast<unsigned char>(std::lround((double)v * 255.0 / (double)maxv));
                out[size_t(j)*size_t(width) + size_t(i)] = g;
            }
        }
    } else { // Binary P5 format with packed pixel bytes
        if(!readInt(fs, width))  return false;
        if(!readInt(fs, height)) return false;
        if(!readInt(fs, maxv))   return false;
        if(width<=0 || height<=0 || maxv<=0) return false;

        fs.get(); // Consume single whitespace character between header and data

        if(maxv<=255){
            out.resize(size_t(width)*size_t(height));
            if(!readBytes(fs, out.data(), out.size())) return false;
            if(maxv!=255){
                for(auto& px : out){
                    px = static_cast<unsigned char>(std::lround((double)px * 255.0 / (double)maxv));
                }
            }
        } else {
            const size_t count = size_t(width)*size_t(height);
            std::vector<unsigned char> tmp(count*2);
            if(!readBytes(fs, tmp.data(), tmp.size())) return false;
            out.resize(count);
            for(size_t i=0;i<count;++i){
                unsigned int hi = tmp[2*i+0];
                unsigned int lo = tmp[2*i+1];
                unsigned int v  = (hi<<8)|lo;
                unsigned char g = static_cast<unsigned char>(std::lround((double)v * 255.0 / (double)maxv));
                out[i] = g;
            }
        }
    }

    w = width;
    h = height;
    return true;
}

// Attempt to load a grayscale image from disk using the PGM loader; used as a source for heightmaps or masks
inline bool loadGrayscaleImage(const char* path, std::vector<unsigned char>& out, int& w, int& h){
    return loadPGM(path, out, w, h);
}

// Generate a fallback grayscale heightmap (0..255) using a sinusoidal pattern when no PGM file is available
inline void generateFallbackHeightmap(int w, int h, std::vector<unsigned char>& out, int& ow, int& oh){
    ow = w;
    oh = h;
    out.resize(static_cast<size_t>(w) * static_cast<size_t>(h));
    for(int y=0; y<h; ++y){
        for(int x=0; x<w; ++x){
            float u = static_cast<float>(x) / static_cast<float>(w - 1);
            float v = static_cast<float>(y) / static_cast<float>(h - 1);
            float r = 0.5f + 0.5f * std::sin(10.0f * u) * std::cos(10.0f * v);
            out[y * w + x] = static_cast<unsigned char>(std::round(r * 255.0f));
        }
    }
}

// Create an OpenGL R8 2D texture from 8-bit grayscale data for use as a heightmap or mask in a real-time rasterization pipeline
inline GLuint createGLGrayTexture(const unsigned char* pixels, int w, int h){
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}
