#include <Maia/common.hpp>

#include <Maia/hw/hw.hpp>

#include <Maia/al/al.hpp>
#include <Maia/gl/gl.hpp>
#include <Maia/planet.hpp>

#define PAR_OCTASPHERE_IMPLEMENTATION
#include <Maia/3rdparty/octasphere.h>

#include <Maia/gl/camera.hpp>

#include <moon_texture.h>
#include <planet_texture.h>
#include <soundbank.h>

#include <array>

gl::Mesh make_planet(std::pair<size_t, size_t> tex_dimensions) {
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

    return gl::Mesh{mesh_vertices, tex_dimensions};
}

int main() {
    consoleDemoInit();

    printf("\x1b[2;0H--------------------------------");
    hw::init();

    printf("al: Initializing AL -> ");
    al::init();
    al::Module theme{MOD_MONDAY};
    theme.volume(100).start(true);
    printf("\x1b[32;1mOK\x1b[37;1m\n");
    
    printf("gl: Initializing GL -> ");
    gl::init();

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1);
    glLight(0, RGB15(16,16,16), 0, floattov10(-1.0), 0);
	glLight(1, RGB15(16,16,16), floattov10(1.0) - 1, 0,	0);
    glColor3b(255, 255, 255);

    gl::Camera camera{};
    printf("\x1b[32;1mOK\x1b[37;1m\n");

    printf("sim: Loading simulation -> ");

    gl::Texture earthlike_texture{128, 128, (const uint8_t*)planet_textureBitmap}; // TODO: Don't hardcode size in some way
    gl::Texture moon_texture{128, 128, (const uint8_t*)moon_textureBitmap};
    auto mesh = make_planet({128, 128});

    // TODO: Some kind of Proc-gen?
    Planet a{"Sirius 1", "\x1b[31;1mWarning\x1b[37;1m: Pandemic on planet", 20, mesh, earthlike_texture};
    a.colour = {255, 255, 255};

    a.body.position = {0, 0, 0};
    a.body.mass = 1000;

    Planet b{"Sirius 1a", "Small moon bumbling with animals", 10, mesh, earthlike_texture};
    b.colour = {255, 255, 255};

    b.body.position = {10, 0, 0};
    b.body.linear_velocity = {0, 0, -10};
    b.body.mass = 10;

    Planet c{"Sirius 1b", "High-Metal Concentration world", 5, mesh, moon_texture};
    c.colour = {255, 255, 255};
    
    c.body.position = {5, 0, 0};
    c.body.linear_velocity = {0, 0, 15};
    c.body.mass = 5;

    std::array<Planet, 3> planets{std::move(a), std::move(b), std::move(c)};
    size_t curr_planet = 0;
    printf("\x1b[32;1mOK\x1b[37;1m\n");

    float fov = 70;
    while(true){
        printf("\x1b[0;0H                                \n"); // Clear line
        printf("\x1b[1;0H                                \n"); // Clear line
        printf("\x1b[0;0H[%s] Mass: %.0f Scale: %.0f\n", planets[curr_planet].name, planets[curr_planet].body.mass, planets[curr_planet].sphere.radius * 2);
        printf("\x1b[1;0H%s\n", planets[curr_planet].description);

        scanKeys();
        auto keys_down = keysDown();
        auto keys_held = keysHeld();
        if(keys_held & KEY_START)
            break;
        
        if(keys_down & KEY_A){
            if(curr_planet == planets.size() - 1)
                curr_planet = 0;
            else
                curr_planet++;
        }
        if(keys_down & KEY_B){
            if(curr_planet == 0)
                curr_planet = planets.size() - 1;
            else
                curr_planet--;
        }

        if(keys_held & KEY_R) fov -= 0.5;
        if(keys_held & KEY_L) fov += 0.5;
        fov = std::clamp(fov, 2.0f, 100.0f);

        gl::MatrixStack s{};
        s.mode(GL_PROJECTION).identity().perspective(fov, 256.0 / 192.0, 0.1, 4000);
        s.apply();
    
        camera.center = planets[curr_planet].body.position;
        camera.update();

        for(auto& planet : planets) {
            planet.draw();

            for(auto& other : planets)
                if(other != planet)
                    other.attract(planet);    
            planet.update();   
        }

        glFlush(0);
        swiWaitForVBlank();
    }
}