#pragma once

#include <Maia/common.hpp>

namespace al
{
    struct Module {
        Module(uint32_t id);
        Module& start(bool loop = false);
        Module& stop();
        
        Module& volume(uint32_t percent);

        private:
        uint32_t id;
    };

    void init();
} // namespace al