#pragma once

#include <Maia/common.hpp>
#include <cmath>

template<typename T>
struct vec2 { 
    T x, y; 

    vec2& operator+=(const vec2& b){
        x += b.x;
        y += b.y;

        return *this;
    }

    vec2 operator-(const vec2& b){
        return {x - b.x, y - b.y};
    }

    vec2& operator*=(T b){
        x *= b;
        y *= b;

        return *this;
    }

    vec2 operator*(T b){
        return {x * b, y * b};
    }

    vec2 operator/(T b){
        return {x / b, y / b};
    }

    vec2& operator/=(T b){
        x /= b;
        y /= b;

        return *this;
    }

    float sqr_magnitude(){
        return (x * x) + (y * y);
    }

    float magnitude(){
        return sqrt((x * x) + (y * y));
    }

    vec2 normalized(){
        auto mag = magnitude();
        return {x / mag, y / mag};
    }

    auto operator<=>(const vec2&) const = default;
};