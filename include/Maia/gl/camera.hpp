#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec3.hpp>

#include <Maia/gl/matrix_stack.hpp>

namespace gl {
    struct Camera {
        Camera(): x{0}, eye{}, center{} {}

        void update(){
            auto keys = keysHeld();

            if(keys & KEY_LEFT) x -= 1;
            if(keys & KEY_RIGHT) x += 1;

            eye = {sin(x * 0.1) * 20, 5, cos(x * 0.1) * 20};

            gl::MatrixStack s{};
            s.mode(GL_MODELVIEW).identity().look_at({eye.x, eye.y, eye.z}, {center.x, center.y, center.z}, {0.0f, 1.0f, 0.0f});
            s.apply();
        }

        void set_center(vec3<float> center){
            this->center = center;
        }

        float x;
        vec3<float> eye, center;
    };
}