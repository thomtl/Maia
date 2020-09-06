#pragma once

#include <Maia/common.hpp>
#include <Maia/math/misc.hpp>

namespace gl {
    struct Texture {
        Texture(size_t width, size_t height, const uint8_t* data): width{width}, height{height} {
            sassert(width == 8 || width == 16 || width == 32 || width == 64 || width == 128 || width == 256 || width == 512 || width == 1024, "Invalid width passed to Texture");
            sassert(height == 8 || height == 16 || height == 32 || height == 64 || height == 128 || height == 256 || height == 512 || height == 1024, "Invalid height passed to Texture");
            glGenTextures(1, &id);
            glBindTexture(0, id);

            glTexImage2D(0, 0, GL_RGB, log2i(width) - 3, log2i(height) - 3, 0, TEXGEN_TEXCOORD, data);
        }

        ~Texture(){
            glDeleteTextures(1, &id);
        }

        void bind() const {
            glBindTexture(0, id);
        }

        void unbind() const {
            glBindTexture(0, 0);
        }

        std::pair<size_t, size_t> dimensions() const {
            return {width, height};
        }

        private:
        size_t width, height;
        int id;
    };
}