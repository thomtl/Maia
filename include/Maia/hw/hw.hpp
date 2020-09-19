#pragma once

#include <Maia/common.hpp>

namespace hw
{
    struct Features {
        uint32_t revised_dma : 1;
        uint32_t ndma : 1;
    };
    
    extern Features features;
    void init();
} // namespace hw