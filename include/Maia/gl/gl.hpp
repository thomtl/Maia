#pragma once

#include <Maia/common.hpp>

#include <vector>
#include <utility>

#include <Maia/math/misc.hpp>

#include <Maia/gl/command_list_builder.hpp>
#include <Maia/gl/matrix_stack.hpp>
#include <Maia/gl/texture.hpp>

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

    template<typename T>
    concept Bindable = requires (T t) {
        { t.bind() };
        { t.unbind() };
    };
    
    template<Bindable Object>
    class ScopedBind {
        public:
        explicit ScopedBind(Object& object): _object{object} {
            _object.bind();
        }

        ~ScopedBind(){
            _object.unbind();
        }

        ScopedBind& operator=(const ScopedBind&) = delete;
        ScopedBind(const ScopedBind&) = delete;

        private:
        Object& _object;
    };

    struct Vertex {
        float v[3];
        float n[3];
        float u[2];
        uint8_t c[3];
    }; 

    struct Mesh {
        Mesh(const std::vector<Vertex>& vertices, const Texture& texture): texture{texture} {
            cmd.append(gl::packets::begin_vtxs_packet{GL_TRIANGLES});
            auto [width, height] = texture.dimensions();
            auto width_transform = (8 << (log2i(width) - 3));
            auto height_transform = (8 << (log2i(height) - 3));

            for(const auto& vertex : vertices){
                //auto float_to_v16 = [](float v) -> uint16_t { return ((uint16_t)((v) * (1 << 12))); };
                //cmd.append(gl::packets::colour_packet{vertex.c});
                cmd.append(gl::packets::normal_packet{floattov10(vertex.n[0]), floattov10(vertex.n[1]), floattov10(vertex.n[2])});
                cmd.append(gl::packets::texcoord_packet{floattot16(vertex.u[0] * width_transform), floattot16(vertex.u[1] * height_transform)});
                cmd.append(gl::packets::vtx_16_packet{floattov16(vertex.v[0]), floattov16(vertex.v[1]), floattov16(vertex.v[2])});  
            }

            // Technically not needed, but might be a good idea, libnds says it can cause problems?
            cmd.append(gl::packets::end_vtxs_packet{});

            cmd.construct();
        }

        void draw(){
            ScopedBind guard{texture};
            cmd.execute();
        }

        const Texture& texture;
        cmdlist cmd{};
    };
}