#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec3.hpp>
#include <Maia/math/physics_object.hpp>
#include <Maia/gl/gl.hpp>

struct Planet : public PhysicsObject {
    Planet(const char* name, const char* description, gl::Mesh& mesh): PhysicsObject{}, name{name}, description{description}, mesh{&mesh}, colour{255, 255, 255} {}
    size_t radius;
    const char* name, *description;
    gl::Mesh* mesh;
    vec3<uint8_t> colour;

    void draw(){
        gl::MatrixStack stack{};
        stack.mode(GL_MODELVIEW).push().translate(this->pos.x, this->pos.y, this->pos.z).scale(0.01 * radius, 0.01 * radius, 0.01 * radius);
        stack.apply();
        
        glMaterialf(GL_AMBIENT, RGB15((this->colour.x >> 3) / 10, (this->colour.y >> 3) / 10, (this->colour.z >> 3) / 10));
	    glMaterialf(GL_DIFFUSE, RGB15(this->colour.x >> 3, this->colour.y >> 3, this->colour.z >> 3));
	    glMaterialf(GL_SPECULAR, BIT(15) | RGB15((this->colour.x >> 3) / 4, (this->colour.y >> 3) / 4, (this->colour.z >> 3) / 4));

        mesh->draw();
        glPopMatrix(1);
    }

    void update() {
        this->step(1.0f / 60);
    }

    void attract(Planet& to){
        constexpr float G = 1; // Gravitational constant

        auto dir = pos - to.pos;

        auto f = G * ((mass * to.mass) / dir.sqr_magnitude());
        auto force = dir.normalized() * f;
        to.add_force(force);
    }

    auto operator<=>(const Planet&) const = default;
};