#pragma once

#include <Maia/common.hpp>

namespace ndma
{
    namespace hw
    {
        constexpr size_t n_channels = 4;
        constexpr uintptr_t mmio_base = 0x4004100;

        struct NdmaRegion {
            uint32_t global_control;
            struct {
                uint32_t src;
                uint32_t dst;
                uint32_t total;
                uint32_t len;
                uint32_t clk;
                uint32_t fill;
                uint32_t control;
            } channels[n_channels];
        };
    } // namespace hw
    

    struct Transfer {
        enum class IncementMode { Increment = 0, Decrement = 1, Fixed = 2 };

        IncementMode src_inc = IncementMode::Increment, dst_inc = IncementMode::Increment;

        uint8_t startup_mode = 0x10;

        uint32_t src, dst;
        size_t len;
    };
    
    class NDMAController {
        public:
        NDMAController();
        uint8_t transfer_async(const Transfer& transfer);
        void transfer_await(uint8_t channel);

        void transfer_sync(const Transfer& transfer);

        private:
        uint8_t free_channel();

        uint8_t channels_busy;
        volatile hw::NdmaRegion* mmio;
    };

    // The only reason we need 2 is for the channels busy, there is no real difference in the drivers
    extern NDMAController ndma9;
    extern NDMAController ndma7;
} // namespace ndma