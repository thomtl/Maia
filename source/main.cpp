#include <Maia/common.hpp>

#include <Maia/gl/gl.hpp>

int main() {
    auto* console = consoleDemoInit();

    bool is_dsi = (REG_SCFG_ROM & 3) == 1;
    if(is_dsi)
        printf("hw: Running on a \u001b[36;1mDSi\u001b[37;1m\n");
    else
        printf("hw: Running on a \u001b[36;1mDS\u001b[37;1m\n");

    if(is_dsi) {
        printf("hw: SCFG Locked: %d\n", !((REG_SCFG_EXT >> 31) & 1));
    }
    
    printf("gl: Initializing GL ... ");
    gl::init();
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
    printf("\u001b[32;1mDone\u001b[37;1m\n");

    std::vector<gl::Vertex> vertices {
        {{inttov16(-1), inttov16(-1), 0}, {255, 0, 0}},
        {{inttov16(1), inttov16(-1), 0}, {0, 255, 0}},
        {{inttov16(0), inttov16(1), 0}, {0, 0, 255}},
    };
    gl::Mesh mesh{vertices};

    gl::MatrixStack setup_stack{};
    setup_stack.mode(GL_PROJECTION).identity().perspective(70, 256.0 / 192.0, 0.1, 40);
    setup_stack.mode(GL_MODELVIEW).identity().look_at({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    setup_stack.mode(GL_TEXTURE).identity();
    setup_stack.apply();

    float x = 0, y = 0, a = 0;
    while(true){
        gl::MatrixStack stack{};
        stack.mode(GL_MODELVIEW).push().identity().translate(x, y, -3).rotate_x(a);
        stack.apply();

        scanKeys();
        auto keys = keysHeld();

        if(keys & KEY_START)
            break;

        if(keys & KEY_UP) y += 0.1;
		if(keys & KEY_DOWN) y -= 0.1;
		if(keys & KEY_LEFT) x -= 0.1;
		if(keys & KEY_RIGHT) x += 0.1;
        if(keys & KEY_L) a += 3.0;
        if(keys & KEY_R) a -= 3.0;

        mesh.draw();
        glPopMatrix(1);
        glFlush(0);
        swiWaitForVBlank();
    }
}