#pragma once

#include <Maia/common.hpp>
#include <Maia/math/vec3.hpp>
#include <Maia/math/physics_object.hpp>
#include <Maia/gl/gl.hpp>

struct Planet  {
    Planet(const char* name, const char* description, size_t diameter, gl::Mesh& mesh, gl::Texture& texture): body{}, name{name}, description{description}, colour{255, 255, 255}, mesh{&mesh}, texture{&texture} {
        sphere.radius = diameter / 2;
        body.shape = &sphere;
    }
    PhysicsObject body;
    Sphere sphere;
    const char* name, *description;
    vec3<uint8_t> colour;

    gl::Mesh* mesh;
    gl::Texture* texture;

    void draw(){
        gl::MatrixStack stack{};
        stack.mode(GL_MODELVIEW).push();
        
        stack.translate(body.position);
        stack.rotate_x(body.rotation.x);
        stack.rotate_y(body.rotation.y);
        stack.rotate_z(body.rotation.z);
        stack.scale(vec3<float>{sphere.radius * 2, sphere.radius * 2, sphere.radius * 2} * 0.01);
        stack.apply();
        
        glMaterialf(GL_AMBIENT, RGB15((this->colour.x >> 3) / 10, (this->colour.y >> 3) / 10, (this->colour.z >> 3) / 10));
	    glMaterialf(GL_DIFFUSE, RGB15(this->colour.x >> 3, this->colour.y >> 3, this->colour.z >> 3));
	    glMaterialf(GL_SPECULAR, BIT(15) | RGB15((this->colour.x >> 3) / 4, (this->colour.y >> 3) / 4, (this->colour.z >> 3) / 4));

        mesh->draw(*texture);
        glPopMatrix(1);
    }

    void update() {
        body.step(1.0f / 60); // DS framerate is fixed at 60fps, so 60 Hz is the correct dt
    }

    void attract(Planet& to){
        constexpr float G = 1; // Gravitational constant

        auto dir = body.position - to.body.position;

        auto f = G * ((body.mass * to.body.mass) / dir.sqr_magnitude());
        auto force = dir.normalized() * f;
        to.body.add_force(force);
    }

    auto operator<=>(const Planet&) const = default;
};