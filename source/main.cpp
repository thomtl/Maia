#include <Maia/common.hpp>

#include <Maia/gl/gl.hpp>
#include <Maia/planet.hpp>

#define PAR_OCTASPHERE_IMPLEMENTATION
#include <Maia/3rdparty/octasphere.h>

#include <Maia/bmp_parser.hpp>

gl::Mesh make_planet(const Texture& tex) {
    const par_octasphere_config cfg = {
        .corner_radius = 5,
        .width = 0,
        .height = 0,
        .depth = 0,
        .num_subdivisions = 2,
        .uv_mode = PAR_OCTASPHERE_UV_LATLONG,
        .normals_mode = PAR_OCTASPHERE_NORMALS_SMOOTH
    };

    // Allocate memory for the mesh and opt-out of normals.
    uint32_t num_indices;
    uint32_t num_vertices;
    par_octasphere_get_counts(&cfg, &num_indices, &num_vertices);
    
    std::vector<float> vertices, normals, uvs;
    vertices.resize(num_vertices * 3);
    normals.resize(num_vertices * 3);
    uvs.resize(num_vertices * 2);

    std::vector<uint16_t> indices;
    indices.resize(num_indices);
    
    par_octasphere_mesh mesh = {
        .positions = vertices.data(),
        .normals = normals.data(),
        .texcoords = uvs.data(),
        .indices = indices.data(),
        .num_indices = 0,
        .num_vertices = 0
    };

    // Generate vertex coordinates, UV's, and triangle indices.
    par_octasphere_populate(&cfg, &mesh);

    std::vector<gl::Vertex> mesh_vertices{};

    for(auto i : indices)
        mesh_vertices.push_back({{vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]}, 
                                 {normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]},
                                 {uvs[2 * i], uvs[2 * i + 1]},
                                 {255, 255, 255}});

    return gl::Mesh{mesh_vertices, tex};
}

#include <Maia/planet_texture.h>

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

    gl::MatrixStack setup_stack{};
    setup_stack.mode(GL_PROJECTION).identity();
    setup_stack.mode(GL_MODELVIEW).identity();
    setup_stack.mode(GL_TEXTURE).identity();
    setup_stack.mode(GL_POSITION).identity();
    setup_stack.mode(GL_MODELVIEW).apply();

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT1);

    glLight(0, RGB15(16,16,16) , 0,		floattov10(-1.0),		0);
	glLight(1, RGB15(16,16,16),   floattov10(1.0) - 1,	0,		0);

    glMaterialShinyness();

    //glColor3b(255, 255, 255);
    printf("\u001b[32;1mDone\u001b[37;1m\n");

    bmp::BmpParser parser{planet_texture_bin};
    std::vector<rgb> fb{};
    for(auto& v : parser.fb)
        fb.push_back(RGB8(v.x, v.y, v.z));
    auto [width, height] = parser.dimensions;

    Texture texture{width, height, (const uint8_t*)fb.data()};

    auto mesh = make_planet(texture);

    Planet a{mesh};
    a.colour = {255, 255, 255};//{0xf9, 0x90, 0x6f};
    a.mass = 1000;

    Planet b{mesh};
    b.colour = {255, 255, 255};//{0x00, 0x71, 0xc5};
    b.pos = {-10, 0, 0};
    b.vel = {0, 10, 0};
    b.mass = 1;

    std::vector<Planet> planets = {std::move(a), std::move(b)};

    float x = 0, fov = 70;
    while(true){
        scanKeys();
        auto keys = keysHeld();

        if(keys & KEY_START)
            break;

        if(keys & KEY_LEFT) x -= 1;
        if(keys & KEY_RIGHT) x += 1;

        if(keys & KEY_R) fov -= 0.5;
        if(keys & KEY_L) fov += 0.5;
        fov = std::clamp(fov, 2.0f, 100.0f);
        
        gl::MatrixStack s{};
        s.mode(GL_PROJECTION).identity().perspective(fov, 256.0 / 192.0, 0.1, 4000);
        s.mode(GL_MODELVIEW).identity().look_at({sin(x * 0.1) * 20, 5, cos(x * 0.1) * 20}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
        s.apply();

        for(auto& planet : planets) {
            planet.draw();

            planet.update();
            for(auto& other : planets)
                if(other != planet)
                    other.attract(planet);       
        }

        glFlush(0);
        swiWaitForVBlank();
    }
}