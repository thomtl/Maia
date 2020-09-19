#pragma once

#include <Maia/common.hpp>

namespace hw
{
    struct Quirks {
        uint32_t geometry_dma : 1;
    };

    struct Features {
        uint32_t ndma : 1;
    };
    
    extern Quirks quirks;
    extern Features features;
    void init();
} // namespace hw