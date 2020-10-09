#include <Maia/hw/hw.hpp>

hw::Features hw::features;

void init_twl(){
    if(REG_SCFG_EXT & (1 << 31)){ // We have write access to SCFG_EXT, so enable all the features
        REG_SCFG_EXT |= (1 << 0); // Enable Revised ARM9 DMA
        REG_SCFG_EXT |= (1 << 1); // Enable Revised Geometry Engine
        REG_SCFG_EXT |= (1 << 2); // Enable Revised Renderer Engine
        REG_SCFG_EXT |= (1 << 3); // Enable Revised 2D Engine
        REG_SCFG_EXT |= (1 << 4); // Enable Revised Divider Circuit

        REG_SCFG_EXT |= (1 << 16); // Enable NDMA

        printf("twl: Enabled Revised HW + NDMA\n");
    }

    // We still need to do these checks incase we can't access SCFG_EXT
    if(REG_SCFG_EXT & (1 << 0))
        hw::features.revised_dma = 1;

    if(REG_SCFG_EXT & (1 << 16))
        hw::features.ndma = 1;

    printf("twl: Setting TWL CPU Clock -> ");
    setCpuClock(true);
    printf("\x1b[32;1mOK\x1b[37;1m"); // This has no '\n' because it *just* fits on the screen and that would make it skip one line
}

bool is_nocashgba() {
	const auto* debug_str = (const char*)0x4FFFA00;
	return strncmp(debug_str, "no$gba", 6) == 0;
}

void hw::init(){
    auto is_dsi = isDSiMode();
    printf("hw: Running on a \x1b[36;1m%s\x1b[37;1m\n", is_dsi ? "DSi" : "DS");
    if(is_nocashgba()) {
        printf("hw: Running on NO$GBA\n");
        hw::features.nocashgba = 1;
    }

    if(is_dsi)
        init_twl();
}