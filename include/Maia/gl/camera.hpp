#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec3.hpp>

#include <Maia/gl/matrix_stack.hpp>

namespace gl {
    struct Camera {
        Camera(): pitch{0}, yaw{0}, eye{}, center{} {}

        void update(){
            auto keys = keysHeld();

            if(keys & KEY_RIGHT) yaw -= 0.1;
            if(keys & KEY_LEFT) yaw += 0.1;

            if(keys & KEY_UP) pitch -= 0.1;
            if(keys & KEY_DOWN) pitch += 0.1;

            constexpr float pi_2 = 1.57079632679489661923;

            pitch = std::clamp(pitch, -pi_2 + 0.1f, pi_2 - 0.1f);

            eye = {-cos(pitch) * sin(yaw) * 25,
                   -sin(pitch) * 25,
                   cos(pitch) * cos(yaw) * 25};

            gl::MatrixStack s{};
            s.mode(GL_MODELVIEW).identity().look_at(eye, center, {0.0f, 1.0f, 0.0f});
            s.apply();
        }

        float pitch, yaw;
        vec3f eye, center;
    };
}