#pragma once

#include "Vertex.h"
#include <iostream>

class mat4 {
  private:
    float data[16];

  public:
    mat4(float v = 0.0f);
    ~mat4();

    float *getData() { return data; }

    const float &operator()(int x, int y) const;
    float &operator()(int x, int y);

    mat4 T();
    mat4 inv();

    static mat4 perspective(float fov, float aspect, float near, float far);
    static mat4 lookAt(const vec3 &position, const vec3 &target,
                       const vec3 &up);
    static mat4 rotate(const mat4 &m, const vec3 &v);
    static mat4 scale(const mat4 &m, const vec3 &v);
    static mat4 transform(const vec3 &position, const vec3 &rotation,
                          const vec3 &scale);
};

std::ostream &operator<<(std::ostream &out, const mat4 &mat);
mat4 operator*(const mat4 &m1, const mat4 &m2);

//くコ:彡