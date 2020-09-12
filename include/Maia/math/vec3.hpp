#pragma once

#include <Maia/common.hpp>
#include <cmath>

template<typename T>
struct vec3 { 
    T x, y, z; 

    vec3& operator+=(const vec3& b){
        x += b.x;
        y += b.y;
        z += b.z;

        return *this;
    }

    vec3 operator-(const vec3& b) const {
        return {x - b.x, y - b.y, z - b.z};
    }

    vec3 operator+(const vec3& b) const {
        return {x + b.x, y + b.y, z + b.z};
    }

    vec3& operator*=(T b){
        x *= b;
        y *= b;
        z *= b;

        return *this;
    }

    vec3 operator*(T b) const {
        return {x * b, y * b, z * b};
    }

    vec3 operator/(T b) const {
        return {x / b, y / b, z / b};
    }

    vec3& operator/=(T b){
        x /= b;
        y /= b;
        z /= b;

        return *this;
    }

    float sqr_magnitude() const {
        return (x * x) + (y * y) + (z * z);
    }

    float magnitude() const {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    vec3 normalized() const {
        auto mag = magnitude();
        return *this / mag;
    }

    vec3 cross(const vec3& b) const {
        return {y * b.z - b.y * z, z * b.x - b.z * x, x * b.y - b.x * y};
    }

    auto operator<=>(const vec3&) const = default;
};