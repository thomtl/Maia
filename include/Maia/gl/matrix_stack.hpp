#pragma once

#include <Maia/common.hpp>
#include <Maia/gl/command_list_builder.hpp>

#include <Maia/math/vec3.hpp>

namespace gl
{
    struct MatrixStack {
        MatrixStack& mode(int mode) {
            cmd.append(gl::packets::mtx_mode_packet{mode});

            return *this;
        }

        MatrixStack& identity() {
            cmd.append(gl::packets::mtx_identity_packet{});

            return *this;
        }

        MatrixStack& load_4x4(const m4x4& m) {
            cmd.append(gl::packets::mtx_load_4x4_packet{m});

            return *this;
        }

        MatrixStack& push() {
            cmd.append(gl::packets::mtx_push_packet{});

            return *this;
        }

        MatrixStack& pop(int n = 1) {
            cmd.append(gl::packets::mtx_pop_packet{n});

            return *this;
        }
    
        MatrixStack& translate(vec3<float> v) {
            cmd.append(gl::packets::mtx_translation_packet{(uint32_t)floattof32(v.x), (uint32_t)floattof32(v.y), (uint32_t)floattof32(v.z)});

            return *this;
        }

        MatrixStack& scale(vec3<float> v) {
            cmd.append(gl::packets::mtx_scale_packet{(uint32_t)floattof32(v.x), (uint32_t)floattof32(v.y), (uint32_t)floattof32(v.z)});

            return *this;
        }

        MatrixStack& mult_3x3(const m3x3& m) {
            cmd.append(gl::packets::mtx_mult_3x3_packet{m});

            return *this;
        }

        MatrixStack& mult_4x4(const m4x4& m) {
            cmd.append(gl::packets::mtx_mult_4x4_packet{m});

            return *this;
        }

        MatrixStack& mult_4x3(const m4x3& m) {
            cmd.append(gl::packets::mtx_mult_4x3_packet{m});

            return *this;
        }

        MatrixStack& rotate_x(float a) {
            int angle = (int)(a * DEGREES_IN_CIRCLE / 360.0);

            int sine = sinLerp(angle);
            int cosine = cosLerp(angle);
            mult_3x3({
                inttof32(1), 0, 0,
                0, cosine, sine,
                0, -sine, cosine
            });

            return *this;
        }
    
        MatrixStack& rotate_y(float a) {
            int angle = (int)(a * DEGREES_IN_CIRCLE / 360.0);

            int sine = sinLerp(angle);
            int cosine = cosLerp(angle);
            mult_3x3({
                cosine, 0, -sine,
                0, inttof32(1), 0,
                sine, 0, cosine,
            });

            return *this;
        }

        MatrixStack& perspective(float fov, float aspect, float z_near, float z_far) {
            int fovy = (int)(fov * DEGREES_IN_CIRCLE / 360.0);

            int ymax = mulf32(floattof32(z_near), tanLerp(fovy >> 1));
            int ymin = -ymax;

            int xmin = mulf32(ymin, floattof32(aspect));
            int xmax = mulf32(ymax, floattof32(aspect));

            mult_4x4({
                divf32(2 * floattof32(z_near), xmax - xmin), 0, 0, 0,
                0, divf32(2 * floattof32(z_near), ymax - ymin), 0, 0,
                divf32(xmax + xmin, xmax - xmin), divf32(ymin + ymax, ymax - ymin), -divf32(floattof32(z_far) + floattof32(z_near), floattof32(z_far) - floattof32(z_near)), floattof32(-1.0f),
                0, 0, -divf32(2 * mulf32(floattof32(z_far), floattof32(z_near)), floattof32(z_far) - floattof32(z_near)), 0
            });

            return *this;
        }

        MatrixStack& look_at(const vec3<float>& eye, const vec3<float>& center, const vec3<float>& up) {
            auto compute = [this](int32_t eye[3], int32_t center[3], int32_t up[3]) {
                int32_t forward[3] = {eye[0] - center[0], eye[1] - center[1], eye[2] - center[2]};
                normalizef32(forward);

                int32_t side[3] = {};
                crossf32(up, forward, side);
                normalizef32(side);
                crossf32(forward, side, up);

                mult_4x3({
                    side[0], up[0], forward[0],
                    side[1], up[1], forward[1],
                    side[2], up[2], forward[2],
                    -dotf32(eye, side), -dotf32(eye, up), -dotf32(eye, forward)
                });
            };

            int32_t eye_[3] = {floattof32(eye.x), floattof32(eye.y), floattof32(eye.z)};
            int32_t center_[3] = {floattof32(center.x), floattof32(center.y), floattof32(center.z)};
            int32_t up_[3] = {floattof32(up.x), floattof32(up.y), floattof32(up.z)};
            compute(eye_, center_, up_);

            return *this;
        }

        void apply(){
            cmd.execute();
        }

        cmdlist cmd{};
    }; 
} // namespace gl