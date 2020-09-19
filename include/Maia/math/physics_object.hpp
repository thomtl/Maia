#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec2.hpp>
#include <Maia/math/vec3.hpp>

struct Shape {
    virtual float moment_of_inertia(float mass) = 0;
    virtual vec3f distance_from_center(const vec3f& pos, const vec3f& point) = 0;

    auto operator<=>(const Shape&) const = default;
};

struct Sphere : public Shape {
    float radius;
    float moment_of_inertia(float mass) {
        return (2.0 / 5.0) * mass * radius * radius;
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
        angular_acc += t / shape->moment_of_inertia(mass);
    }

    auto operator<=>(const PhysicsObject&) const = default;
    private:
    vec3f linear_acc, angular_acc; // Not a user changable element, just useful for keeping track of forces on the object each step
};
