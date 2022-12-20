#pragma once

#include <cstdint>
#include <iostream>

struct vec2 {
    float x, y;
};

struct vec3 {
    float x, y, z;
};

struct Color {
    uint8_t r, g, b, a;
};

struct Vertex {
    vec3 vertex;
    vec3 normal;
    vec2 texcoord;
};

float dist(const vec3 &v);
float dot(const vec3 &v1, const vec3 &v2);
vec3 normalise(const vec3 &v);
vec3 cross(const vec3 &v1, const vec3 &v2);
vec3 operator+(const vec3 &v1, const vec3 &v2);
vec3 operator-(const vec3 &v1, const vec3 &v2);
vec3 operator*(float f, const vec3 &v);
std::ostream &operator<<(std::ostream &out, const vec3 &v);

//くコ:彡