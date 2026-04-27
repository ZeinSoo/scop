#include <fstream>
#include <vector>
#include <string>

#include "Logger.hpp"

static unsigned int read_u32_le(const u_int8_t *buf) {
    return static_cast<unsigned int>(buf[0]) |
           (static_cast<unsigned int>(buf[1]) << 8) |
           (static_cast<unsigned int>(buf[2]) << 16) |
           (static_cast<unsigned int>(buf[3]) << 24);
}

static int read_i32_le(const u_int8_t *buf) {
    return static_cast<int>(read_u32_le(buf));
}

unsigned char *loadTexture(const char *filePath, int *width, int *height) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        Logger::log(std::string("ERROR::BMPLOADER::COULD_NOT_OPEN_FILE: ") + filePath);
        return NULL;
    }

    u_int8_t fileHeader[14] = {};
    u_int8_t infoHeader[40] = {};

    file.read(reinterpret_cast<char *>(fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char *>(infoHeader), sizeof(infoHeader));

    if (!file) {
        Logger::log(std::string("ERROR::BMPLOADER::FILE_READ_ERROR: ") + filePath);
        return NULL;
    }

    if (fileHeader[0] != 'B' || fileHeader[1] != 'M') {
        Logger::log(std::string("ERROR::BMPLOADER::NOT_A_BMP_FILE: ") + filePath);
        return NULL;
    }

    u_int8_t bpp = infoHeader[14];
    if (bpp != 24 && bpp != 32) {
        Logger::log("ERROR::BMPLOADER::UNSUPPORTED_BPP");
        return NULL;
    }

    const unsigned int pixelOffset = read_u32_le(&fileHeader[10]);
    const int w = read_i32_le(&infoHeader[4]);
    const int hSigned = read_i32_le(&infoHeader[8]);
    const int absH = (hSigned < 0) ? -hSigned : hSigned;
    const bool bmpTopDown = (hSigned < 0);
    if (w <= 0 || absH <= 0) {
        Logger::log("ERROR::BMPLOADER::INVALID_DIMENSIONS");
        return NULL;
    }

    *width = w;
    *height = absH;

    const unsigned int bytesPerPixel = static_cast<unsigned int>(bpp / 8);
    const unsigned int rowSize = ((static_cast<unsigned int>(w) * bytesPerPixel + 3u) / 4u) * 4u;
    const unsigned int padding = rowSize - static_cast<unsigned int>(w) * bytesPerPixel;

    file.seekg(pixelOffset, std::ios::beg);
    if (!file) {
        Logger::log("ERROR::BMPLOADER::INVALID_PIXEL_OFFSET");
        return NULL;
    }

    unsigned char *data = new unsigned char[static_cast<size_t>(w) * static_cast<size_t>(absH) * 4u]; // RGBA

    // We store pixels in memory as top-down rows (row 0 = top of image)
    for (int yFile = 0; yFile < absH; ++yFile) {
        const int yDest = bmpTopDown ? yFile : (absH - 1 - yFile);
        for (int x = 0; x < w; ++x) {
            const size_t i = (static_cast<size_t>(x) + static_cast<size_t>(yDest) * static_cast<size_t>(w)) * 4u;

            u_int8_t bgrx[4] = {0, 0, 0, 255};
            file.read(reinterpret_cast<char *>(bgrx), bytesPerPixel);
            if (!file) {
                Logger::log(std::string("ERROR::BMPLOADER::FILE_READ_ERROR: ") + filePath);
                delete[] data;
                return NULL;
            }

            data[i + 0] = bgrx[2];
            data[i + 1] = bgrx[1];
            data[i + 2] = bgrx[0];
            data[i + 3] = (bytesPerPixel == 4) ? bgrx[3] : 255;
        }
        if (padding)
            file.ignore(padding);
    }
    return data;
}