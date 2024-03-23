#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// ospcommon
#include "ospcommon/math/vec.h"

#if 0
// std
#include <iostream>
#include <stdexcept>
#include <exception>
#include <vector>
#include <functional>
#include <string>
#endif

using namespace ospcommon::math;

void writePNG(const char *fileName, const vec2i &size, const uint8_t *pixel)
{
    constexpr int nChannels {4};
    const int stride { nChannels * size.x };
    stbi_write_png(fileName, size.x, size.y, nChannels, pixel, size.x * nChannels);
}

void createTextureData(uint8_t *buffer, const vec2i& size) {
    int value;
    size_t idx {0};
    uint8_t pattern { 0x8 };
    for (int row = 0; row < size.x; row++) {
        for (int col = 0; col < size.y; col++) {
            // Each cell is 8x8, value is 0 or 255 (black or white)
            value = (((row & pattern) == 0) ^ ((col & pattern) == 0)) * 255;
            buffer[idx++] = (int)value;
            buffer[idx++] = (int)value;
            buffer[idx++] = (int)value;
            buffer[idx++] = 255;
        }
    }
}

int main(int argc, const char** argv)
{
    vec2i imgSize { std::stoi(argv[1]) };
    
    auto texture = new uint8_t[imgSize.x * imgSize.y * 4];
    
    createTextureData( texture, imgSize );
    
    writePNG("checkerboard.png", imgSize, texture);
    
    return 0;
}
