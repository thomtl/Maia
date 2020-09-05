#include <Maia/common.hpp>

#include <Maia/gl/gl.hpp>

#define PAR_OCTASPHERE_IMPLEMENTATION
#include <Maia/3rdparty/octasphere.h>

auto sqr = [](auto v) { return v * v; };

gl::Mesh make_planet() {
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
    
    std::vector<float> vertices, normals;
    vertices.resize(num_vertices * 3);
    normals.resize(num_vertices * 3);

    std::vector<uint16_t> indices;
    indices.resize(num_indices);
    
    par_octasphere_mesh mesh = {
        .positions = vertices.data(),
        .normals = normals.data(),
        .indices = indices.data(),
        .num_indices = 0,
        .num_vertices = 0
    };

    // Generate vertex coordinates, UV's, and triangle indices.
    par_octasphere_populate(&cfg, &mesh);

    std::vector<gl::Vertex> mesh_vertices{};

    for(auto i : indices)
        mesh_vertices.push_back({{floattov16(vertices[3 * i]), floattov16(vertices[3 * i + 1]), floattov16(vertices[3 * i + 2])}, 
                                 {floattov10(normals[3 * i]), floattov10(normals[3 * i + 1]), floattov10(normals[3 * i + 2])},
                                 {255, 255, 255}});

    return gl::Mesh{mesh_vertices};
}

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
    setup_stack.mode(GL_PROJECTION).identity().perspective(70, 256.0 / 192.0, 0.1, 4000);
    setup_stack.mode(GL_MODELVIEW).identity();
    setup_stack.mode(GL_TEXTURE).identity();
    setup_stack.mode(GL_MODELVIEW).apply();

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT1);

    glLight(0, RGB15(16,16,16) , 0,		floattov10(-1.0),		0);
	glLight(1, RGB15(16,16,16),   floattov10(1.0) - 1,	0,		0);

    glMaterialShinyness();

    glMaterialf(GL_AMBIENT, RGB15(2, 2, 2));
	glMaterialf(GL_DIFFUSE, RGB15(15, 15, 15));
	glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8,8,8));

    glColor3b(255, 255, 255);
    printf("\u001b[32;1mDone\u001b[37;1m\n");

    auto mesh = make_planet();

    struct Planet {
        struct vec3 { 
            float x, y, z; 

            vec3& operator+=(const vec3& b){
                x += b.x;
                y += b.y;
                z += b.z;

                return *this;
            }

            vec3 operator-(const vec3& b){
                return {x - b.x, y - b.y, z - b.z};
            }

            vec3& operator*=(float b){
                x *= b;
                y *= b;
                z *= b;

                return *this;
            }

            vec3 operator*(float b){
                return {x * b, y * b, z * b};
            }

            vec3 operator/(float b){
                return {x / b, y / b, z / b};
            }

            vec3& operator/=(float b){
                x /= b;
                y /= b;
                z /= b;

                return *this;
            }

            float sqr_magnitude(){
                return sqr(x) + sqr(y) + sqr(z);
            }

            float magnitude(){
                return sqrt(sqr(x) + sqr(y) + sqr(z));
            }

            vec3 normalized(){
                auto mag = magnitude();
                return {x / mag, y / mag, z / mag};
            }

            auto operator<=>(const vec3&) const = default;
        };

        vec3 pos, vel, acc;
        size_t mass;

        auto operator<=>(const Planet&) const = default;

        void step(float dt){
            vel += acc * dt;
            pos += vel * dt;
            acc = {};
        }

        void add_force(vec3 f){
            acc += f / mass;
        }
    };

    std::vector<Planet> planets = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1000},
        {{-10, 0, 0}, {0, 10, 0}, {0, 0, 0}, 1}
    };

    float x = 0, y = 0;
    while(true){
        scanKeys();
        auto keys = keysHeld();

        if(keys & KEY_START)
            break;

        if(keys & KEY_LEFT) x -= 1;
        if(keys & KEY_RIGHT) x += 1;

        if(keys & KEY_UP) y += 3;
        if(keys & KEY_DOWN) y -= 3;
        
        gl::MatrixStack s{};
        s.mode(GL_MODELVIEW).identity().look_at({sin(x * 0.1) * 20, 5, cos(x * 0.1) * 20}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}).apply();

        for(auto& planet : planets) {
            gl::MatrixStack stack{};
            stack.mode(GL_MODELVIEW).push().translate(planet.pos.x, planet.pos.y, planet.pos.z).scale(0.2, 0.2, 0.2);
            stack.apply();

            mesh.draw();
            glPopMatrix(1);

            planet.step(1.0f / 60);

            for(auto& interaction : planets) {
                if(interaction != planet) {
                    Planet::vec3 dir = planet.pos - interaction.pos;

                    auto F = (planet.mass * interaction.mass) / dir.sqr_magnitude();
                    auto force = dir.normalized() * F;
                    interaction.add_force(force);
                }
            }
                    
        }

        glFlush(0);
        swiWaitForVBlank();
    }
}