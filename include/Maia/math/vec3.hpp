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

    vec3 operator-(const vec3& b){
        return {x - b.x, y - b.y, z - b.z};
    }

    vec3& operator*=(T b){
        x *= b;
        y *= b;
        z *= b;

        return *this;
    }

    vec3 operator*(T b){
        return {x * b, y * b, z * b};
    }

    vec3 operator/(T b){
        return {x / b, y / b, z / b};
    }

    vec3& operator/=(T b){
        x /= b;
        y /= b;
        z /= b;

        return *this;
    }

    float sqr_magnitude(){
        return (x * x) + (y * y) + (z * z);
    }

    float magnitude(){
        return sqrt((x * x) + (y * y) + (z * z));
    }

    vec3 normalized(){
        auto mag = magnitude();
        return {x / mag, y / mag, z / mag};
    }

    auto operator<=>(const vec3&) const = default;
};