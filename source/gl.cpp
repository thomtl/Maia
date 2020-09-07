#include <Maia/gl/gl.hpp>

void gl::init(){
    videoSetMode(MODE_0_3D);
    glInit();

    gl::enable(GL_TEXTURE_2D, GL_ANTIALIAS);

    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    glMaterialShinyness();

    vramSetBankA(VRAM_A_TEXTURE);

    MatrixStack setup_stack{};
    setup_stack.mode(GL_PROJECTION).identity();
    setup_stack.mode(GL_TEXTURE).identity();
    setup_stack.mode(GL_POSITION).identity();
    setup_stack.mode(GL_MODELVIEW).identity();
    setup_stack.apply();
}