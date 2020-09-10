#pragma once

#include <Maia/common.hpp>

#include <vector>
#include <utility>

#include <Maia/math/misc.hpp>

#include <Maia/gl/command_list_builder.hpp>
#include <Maia/gl/matrix_stack.hpp>
#include <Maia/gl/texture.hpp>

#include <Maia/math/vec3.hpp>
#include <Maia/math/vec2.hpp>

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
        vec3<float> position, normal;
        vec2<float> uv;
        vec3<uint8_t> colour;
    }; 

    struct Mesh {
        Mesh(const std::vector<Vertex>& vertices, const gl::Texture& texture): texture{texture} {
            auto [width, height] = texture.dimensions();
            auto width_transform = (8 << (log2i(width) - 3));
            auto height_transform = (8 << (log2i(height) - 3));

            cmd.append(gl::packets::begin_vtxs_packet{GL_TRIANGLES});

            /*
                TODO:
                Optimize buffer usage, if the colour doesn't change between vertices we don't need multiple packets
                Use relative vertex packets and packets which only change 2 of the 3 coords
            */   
            for(const auto& vertex : vertices){
                cmd.append(gl::packets::colour_packet{vertex.colour});
                cmd.append(gl::packets::normal_packet{vertex.normal});
                cmd.append(gl::packets::texcoord_packet{{vertex.uv.x * width_transform, vertex.uv.y * height_transform}});
                cmd.append(gl::packets::vtx_16_packet{vertex.position});  
            }

            // Technically not needed, but might be a good idea, libnds says it can cause problems?
            cmd.append(gl::packets::end_vtxs_packet{});
        }

        void draw(){
            ScopedBind guard{texture};
            cmd.execute();
        }

        const gl::Texture& texture;
        cmdlist cmd{};
    };
}