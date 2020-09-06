#include <Maia/al/al.hpp>

#include <maxmod9.h>

#include <soundbank_bin.h>
#include <soundbank.h>

al::Module::Module(uint32_t id): id{id} {
    mmLoad(id);
}

al::Module& al::Module::start(bool loop) {
    mmStart(id, loop ? MM_PLAY_LOOP : MM_PLAY_ONCE);
    return *this;
}

al::Module& al::Module::stop() {
    mmStop();
    return *this;
}

al::Module& al::Module::volume(uint32_t percent) {
    mmSetModuleVolume((1024 / 100) * percent);
    return *this;
}

void al::init(){
    mmInitDefaultMem((mm_addr)soundbank_bin);
    mmSelectMode(MM_MODE_A);
}