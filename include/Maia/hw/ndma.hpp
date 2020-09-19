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

        enum class NdmaStartupModes : uint8_t {
            // Common between ARM7 and ARM9
            Timer0 = 0,
            Timer1 = 1,
            Timer2 = 2,
            Timer3 = 3,
            DsCartridgeSlot = 4,
            VBlank = 6,
            StartImmediately = 16,

            // ARM7 Exclusives
            Arm7NDSWifi = 7,
            Arm7SDMMC = 8,
            Arm7DSIWifi = 9,
            Arm7AESIn = 10,
            Arm7AEXOut = 11,
            Arm7Microphone = 12,

            // ARM9 Exclusives
            Arm9HBlank = 7,
            Arm9DisplaySync = 8,
            Arm9WorkRam = 9,
            Arm9GeometryFifo = 10,
            Arm9Camera = 11
        };
    } // namespace hw
    
    struct Transfer {
        enum class IncementMode { Increment = 0, Decrement = 1, Fixed = 2 };

        IncementMode src_inc = IncementMode::Increment, dst_inc = IncementMode::Increment;

        hw::NdmaStartupModes startup_mode = hw::NdmaStartupModes::StartImmediately;

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