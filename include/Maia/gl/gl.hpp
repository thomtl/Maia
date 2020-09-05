#pragma once

#include <Maia/common.hpp>

#include <vector>
#include <utility>

#include <Maia/gl/command_list_builder.hpp>
#include <Maia/gl/matrix_stack.hpp>

namespace gl {
    template<typename... Options>
    void enable(Options&&... options){
        (glEnable(options), ...);
    }

    template<typename... Options>
    void disable(Options&&... options){
        (glDisable(options), ...);
    }

    void init();

    struct Vertex {
        v16 v[3];
        v10 n[3];
        uint8_t c[3];
    };

    struct Mesh {
        Mesh(const std::vector<Vertex>& vertices) {
            cmd.append(gl::packets::begin_vtxs_packet{GL_TRIANGLES});

            for(const auto& vertex : vertices){
                //auto float_to_v16 = [](float v) -> uint16_t { return ((uint16_t)((v) * (1 << 12))); };
                //cmd.append(gl::packets::colour_packet{vertex.c});
                cmd.append(gl::packets::normal_packet{vertex.n});
                cmd.append(gl::packets::vtx_16_packet{vertex.v});  
            }

            // Technically not needed, but might be a good idea, libnds says it can cause problems?
            //cmd.append(gl::packets::end_vtxs_packet{});

            cmd.construct();
        }

        void draw(){
            cmd.execute();
        }

        cmdlist cmd{};
    };
}