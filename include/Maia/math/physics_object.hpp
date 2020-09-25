#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec2.hpp>
#include <Maia/math/vec3.hpp>

struct Shape {
    virtual vec3f moment_of_inertia(float mass) = 0; // The Moment of Inertia is actually a 3x3 Matrix/Tensor but we can ignore that, see below for an explanation
    virtual vec3f distance_from_center(const vec3f& pos, const vec3f& point) = 0;

    auto operator<=>(const Shape&) const = default;
};

struct Sphere : public Shape {
    float radius;

    vec3f moment_of_inertia(float mass) {
        auto v = (2.0 / 5.0) * mass * radius * radius;
        return {v, v, v};
    }

    vec3f distance_from_center(const vec3f& pos, const vec3f& point) {
        return pos - point;
    }

    auto operator<=>(const Sphere&) const = default;
};

struct PhysicsObject {
    PhysicsObject(): shape{nullptr}, position{}, rotation{}, linear_velocity{}, angular_velocity{}, mass{}, linear_acc{}, angular_acc{} {}
    Shape* shape;
    vec3f position, rotation;
    vec3f linear_velocity, angular_velocity;
    float mass;

    void step(float dt){
        linear_velocity += linear_acc * dt;
        position += linear_velocity * dt;

        angular_velocity += angular_acc * dt;
        rotation += angular_velocity * dt;

        linear_acc = {}; // Reset acceleration for the next timestep
        angular_acc = {};
    }

    void add_force(const vec3f& f){
        linear_acc += f / mass;

        auto r = shape->distance_from_center(position, f);
        auto t = r.cross(f); 

        /*
            Angular Acc, hereafter ω
            Moment of Inertia, hereafter I, is actually a 3x3 matrix
            Torque, hereafter L

            L = I * ω
            We know both L and I, and we want to know ω
            Matrix division is undefined so the correct course of action would be ω = L * inverse(I) (or inverse(I) * L * inverse(I) ?)
            However because in pretty much all cases I is a diagonal matrix we can do an optimization where we do ω = L / {I11, I22, I33}
        */
        angular_acc += t / shape->moment_of_inertia(mass);
    }

    auto operator<=>(const PhysicsObject&) const = default;
    private:
    vec3f linear_acc, angular_acc; // Not a user changable element, just useful for keeping track of forces on the object each step
};
