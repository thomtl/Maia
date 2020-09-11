#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec3.hpp>

struct PhysicsObject {
    PhysicsObject(): pos{}, vel{}, mass{}, acc{} {}
    vec3<float> pos, vel;
    size_t mass;

    void step(float dt){
        vel += acc * dt;
        pos += vel * dt;

        acc = {}; // Reset acc for the next timestep
    }

    void add_force(const vec3<float>& f){
        acc += f / mass;
    }

    auto operator<=>(const PhysicsObject&) const = default;

    private:
    vec3<float> acc; // Not a user changable element, just useful for keeping track of forces on the object each step
};