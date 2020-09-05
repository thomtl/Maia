#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec3.hpp>

struct PhysicsObject {
    PhysicsObject(): pos{}, vel{}, acc{}, mass{} {}
    vec3<float> pos, vel, acc;
    size_t mass;

    void step(float dt){
        vel += acc * dt;
        pos += vel * dt;
        acc = {};
    }

    void add_force(vec3<float> f){
        acc += f / mass;
    }

    auto operator<=>(const PhysicsObject&) const = default;
};