#include "Vertex.h"
#include <math.h>

float dist(const vec3 &v) {
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float dot(const vec3 &v1, const vec3 &v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

vec3 normalise(const vec3 &v) {
    float d = dist(v);
    if (d <= 1e-6f)
        return vec3{0.0f, 0.0f, 0.0f};
    return {v.x / d, v.y / d, v.z / d};
}

vec3 cross(const vec3 &v1, const vec3 &v2) {
    return vec3{(v1.y * v2.z) - (v2.y * v1.z), (v1.z * v2.x) - (v2.z * v1.x),
                (v1.x * v2.y) - (v2.x * v1.y)};
}

vec3 operator+(const vec3 &v1, const vec3 &v2) {
    return vec3{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3 operator-(const vec3 &v1, const vec3 &v2) {
    return vec3{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

vec3 operator*(float f, const vec3 &v) {
    return vec3{f * v.x, f * v.y, f * v.z};
}

std::ostream &operator<<(std::ostream &out, const vec3 &v) {
    out << "{" << v.x << ", " << v.y << ", " << v.z << "}";
    return out;
}

//くコ:彡