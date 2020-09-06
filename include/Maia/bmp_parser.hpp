#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec3.hpp>

#include <vector>

namespace bmp
{
    struct [[gnu::packed]] Header {
        char id[2];
        uint32_t size;
        uint32_t reserved;
        uint32_t pixel_offset;
    };

    struct [[gnu::packed]] InfoHeader {
        uint32_t header_size;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bpp;
        uint32_t compression_method;
        uint32_t image_size;
        int32_t horizontal_res;
        int32_t vertical_res;
        uint32_t palette_size;
        uint32_t n_important_colours;
    };

    struct BmpParser {
        BmpParser(const uint8_t* data){
            header = (const Header*)data;

            printf("bmp: Header ID: %c%c\n", header->id[0], header->id[1]);
            printf("     Size: %ld bytes\n", header->size);

            infoheader = (const InfoHeader*)(header + 1);
            printf("     Res: %ldx%ldx%d\n", infoheader->width, infoheader->height, infoheader->bpp);

            fb.resize(infoheader->width * infoheader->height);

            sassert(infoheader->bpp >= 16, "Palletted colours are unsupported");
            sassert(infoheader->compression_method == 0, "Compressed images are unsupported");
            sassert(infoheader->planes == 1, "Only 1 plane is supported");

            dimensions = {infoheader->width, infoheader->height};
            pixels = (const uint32_t*)(data + header->pixel_offset);

            auto row_size = (infoheader->bpp * infoheader->width + 31) / 32 * 4;
            for(int32_t x = 0; x < infoheader->width; x++) {
                for(int32_t y = 0; y < infoheader->height; y++) {
                    auto pixel = pixels[(row_size * (infoheader->height - 1 - y) + x * infoheader->bpp / 8) / 4];
                    auto& dest = fb[y * infoheader->width + x];
                    
                    if(infoheader->bpp == 16)
                        dest = vec3<uint8_t>{(pixel >> 10) & 31, (pixel >> 5) & 31, pixel & 31};
                    else if(infoheader->bpp == 24)
                        dest = vec3<uint8_t>{(pixel >> 16) & 0xFF, (pixel >> 8) & 0xFF, pixel & 0xFF};
                }
            }
        }

        std::pair<size_t, size_t> dimensions;
        std::vector<vec3<uint8_t>> fb;
        const Header* header;
        const InfoHeader* infoheader;
        const uint32_t* pixels;
    };
} // namespace bmp


