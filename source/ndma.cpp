#include <Maia/hw/ndma.hpp>

using namespace ndma;

NDMAController ndma::ndma9{};
NDMAController ndma::ndma7{};

NDMAController::NDMAController(): channels_busy{0}, mmio{(hw::NdmaRegion*)hw::mmio_base}{
    mmio->global_control = 0;
}

uint8_t NDMAController::transfer_async(const Transfer& transfer){
    auto ch = free_channel();
    sassert(ch != ~0, "Couldn't find free NDMA channel");

    mmio->channels[ch].src = transfer.src;
    mmio->channels[ch].dst = transfer.dst;
    mmio->channels[ch].total = transfer.len / 4;
    mmio->channels[ch].len = transfer.len / 4;
    mmio->channels[ch].clk = 0;
    mmio->channels[ch].control = ((uint32_t)transfer.dst_inc) << 10 |
                                 ((uint32_t)transfer.src_inc) << 13 |
                                 ((uint32_t)transfer.startup_mode) << 24 |
                                 (uint32_t)1 << 31; // Enable bit

    channels_busy |= (1 << ch);

    return ch;
}

void NDMAController::transfer_await(uint8_t channel) {
    // TODO: If transfer.enable_irq was true use swiIntrAwait here instead of polling and make sure its enabled in IE / IF
    while(mmio->channels[channel].control & (1 << 31))
        ;

    channels_busy &= ~(1 << channel);
}

void NDMAController::transfer_sync(const Transfer& transfer) {
    auto channel = transfer_async(transfer);
    transfer_await(channel);
}

uint8_t NDMAController::free_channel(){
    for(size_t i = 0; i < hw::n_channels; i++)
        if(!(channels_busy & (1 << i)))
            return i;

    return ~0;
}