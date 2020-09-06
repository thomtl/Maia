#include <Maia/common.hpp>
#include <Maia/al/al.hpp>
#include <Maia/gl/gl.hpp>
#include <Maia/planet.hpp>

#define PAR_OCTASPHERE_IMPLEMENTATION
#include <Maia/3rdparty/octasphere.h>

#include <Maia/bmp_parser.hpp>

#include <Maia/gl/camera.hpp>

#include <Maia/planet_texture.h>
#include <soundbank.h>

gl::Mesh make_planet(const gl::Texture& tex) {
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

int main() {
    consoleDemoInit();

    printf("\x1b[2;0H--------------------------------");

    bool is_dsi = (REG_SCFG_ROM & 3) == 1;
    if(is_dsi)
        printf("hw: Running on a \x1b[36;1mDSi\x1b[37;1m\n");
    else
        printf("hw: Running on a \x1b[36;1mDS\x1b[37;1m\n");

    if(is_dsi) {
        printf("hw: SCFG Locked: %d\n", !((REG_SCFG_EXT >> 31) & 1));
    }

    printf("al: Initializing AL ... ");
    al::init();
    al::Module theme{MOD_REZ_MONDAY};
    theme.volume(100).start(true);
    printf("\x1b[32;1mDone\x1b[37;1m\n");
    
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
    printf("\x1b[32;1mDone\x1b[37;1m\n");

    bmp::BmpParser parser{planet_texture_bin};
    std::vector<rgb> fb{};
    for(auto& v : parser.fb)
        fb.push_back(RGB8(v.x, v.y, v.z));
    auto [width, height] = parser.dimensions;

    gl::Texture texture{width, height, (const uint8_t*)fb.data()};

    auto mesh = make_planet(texture);

    Planet a{"Sirius 1", "\x1b[31;1mWarning\x1b[37;1m: Pandemic on planet", mesh};
    a.pos = {0, 0, 0};
    a.colour = {255, 255, 255};//{0xf9, 0x90, 0x6f};
    a.mass = 1000;
    a.radius = 20;

    Planet b{"Sirius 1a", "Small moon bumbling with animals", mesh};
    b.colour = {255, 255, 255};//{0x00, 0x71, 0xc5};
    b.pos = {10, 0, 0};
    b.vel = {0, 0, -10};
    b.mass = 10;
    b.radius = 10;

    Planet c{"Sirius 1b", "High-Metal Concentration world", mesh};
    c.colour = {255, 255, 255};//{0x00, 0x71, 0xc5};
    c.pos = {5, 0, 0};
    c.vel = {0, 0, 15};
    c.mass = 5;
    c.radius = 10;

    std::vector<Planet> planets = {std::move(a), std::move(b), std::move(c)};
    gl::Camera camera{};

    size_t curr_planet = 0;

    float fov = 70;
    while(true){
        scanKeys();
                         
        printf("\x1b[0;0H                                \n"); // Clear line
        printf("\x1b[1;0H                                \n"); // Clear line
        printf("\x1b[0;0H[%s] Mass: %d Radius: %d\n", planets[curr_planet].name, planets[curr_planet].mass, planets[curr_planet].radius);
        printf("\x1b[1;0H%s\n", planets[curr_planet].description);
        camera.set_center(planets[curr_planet].pos);

        auto keys_down = keysDown();
        if(keys_down & KEY_UP){
            if(curr_planet == planets.size() - 1)
                curr_planet = 0;
            else
                curr_planet++;
        }
        if(keys_down & KEY_DOWN){
            if(curr_planet == 0)
                curr_planet = planets.size() - 1;
            else
                curr_planet--;
        }

        auto keys_held = keysHeld();
        if(keys_held & KEY_START)
            break;

        if(keys_held & KEY_R) fov -= 0.5;
        if(keys_held & KEY_L) fov += 0.5;
        fov = std::clamp(fov, 2.0f, 100.0f);

        camera.update();
        
        gl::MatrixStack s{};
        s.mode(GL_PROJECTION).identity().perspective(fov, 256.0 / 192.0, 0.1, 4000);
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