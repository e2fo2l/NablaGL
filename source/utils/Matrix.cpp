#include "Matrix.h"
#include <iomanip>
#define _USE_MATH_DEFINES
#include <math.h>

mat4::mat4(float v) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (i == j)
                this->data[i + 4 * j] = v;
            else
                this->data[i + 4 * j] = 0;
}

mat4::~mat4() {}

const float &mat4::operator()(int x, int y) const { return data[x + 4 * y]; }

float &mat4::operator()(int x, int y) { return data[x + 4 * y]; }

mat4 mat4::T() {
    mat4 ret;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ret(i, j) = this->operator()(j, i);
    return ret;
}

mat4 mat4::inv() {
    // No really easy way to do this
    // https://stackoverflow.com/a/1148405
    mat4 ret;
    mat4 m = *this;

    float A2323 = m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2);
    float A1323 = m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1);
    float A1223 = m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1);
    float A0323 = m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0);
    float A0223 = m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0);
    float A0123 = m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0);
    float A2313 = m(1, 2) * m(3, 3) - m(1, 3) * m(3, 2);
    float A1313 = m(1, 1) * m(3, 3) - m(1, 3) * m(3, 1);
    float A1213 = m(1, 1) * m(3, 2) - m(1, 2) * m(3, 1);
    float A2312 = m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2);
    float A1312 = m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1);
    float A1212 = m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1);
    float A0313 = m(1, 0) * m(3, 3) - m(1, 3) * m(3, 0);
    float A0213 = m(1, 0) * m(3, 2) - m(1, 2) * m(3, 0);
    float A0312 = m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0);
    float A0212 = m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0);
    float A0113 = m(1, 0) * m(3, 1) - m(1, 1) * m(3, 0);
    float A0112 = m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0);

    float det =
        m(0, 0) * (m(1, 1) * A2323 - m(1, 2) * A1323 + m(1, 3) * A1223) -
        m(0, 1) * (m(1, 0) * A2323 - m(1, 2) * A0323 + m(1, 3) * A0223) +
        m(0, 2) * (m(1, 0) * A1323 - m(1, 1) * A0323 + m(1, 3) * A0123) -
        m(0, 3) * (m(1, 0) * A1223 - m(1, 1) * A0223 + m(1, 2) * A0123);
    det = 1.0f / det;

    ret(0, 0) = det * (m(1, 1) * A2323 - m(1, 2) * A1323 + m(1, 3) * A1223);
    ret(0, 1) = det * -(m(0, 1) * A2323 - m(0, 2) * A1323 + m(0, 3) * A1223);
    ret(0, 2) = det * (m(0, 1) * A2313 - m(0, 2) * A1313 + m(0, 3) * A1213);
    ret(0, 3) = det * -(m(0, 1) * A2312 - m(0, 2) * A1312 + m(0, 3) * A1212);
    ret(1, 0) = det * -(m(1, 0) * A2323 - m(1, 2) * A0323 + m(1, 3) * A0223);
    ret(1, 1) = det * (m(0, 0) * A2323 - m(0, 2) * A0323 + m(0, 3) * A0223);
    ret(1, 2) = det * -(m(0, 0) * A2313 - m(0, 2) * A0313 + m(0, 3) * A0213);
    ret(1, 3) = det * (m(0, 0) * A2312 - m(0, 2) * A0312 + m(0, 3) * A0212);
    ret(2, 0) = det * (m(1, 0) * A1323 - m(1, 1) * A0323 + m(1, 3) * A0123);
    ret(2, 1) = det * -(m(0, 0) * A1323 - m(0, 1) * A0323 + m(0, 3) * A0123);
    ret(2, 2) = det * (m(0, 0) * A1313 - m(0, 1) * A0313 + m(0, 3) * A0113);
    ret(2, 3) = det * -(m(0, 0) * A1312 - m(0, 1) * A0312 + m(0, 3) * A0112);
    ret(3, 0) = det * -(m(1, 0) * A1223 - m(1, 1) * A0223 + m(1, 2) * A0123);
    ret(3, 1) = det * (m(0, 0) * A1223 - m(0, 1) * A0223 + m(0, 2) * A0123);
    ret(3, 2) = det * -(m(0, 0) * A1213 - m(0, 1) * A0213 + m(0, 2) * A0113);
    ret(3, 3) = det * (m(0, 0) * A1212 - m(0, 1) * A0212 + m(0, 2) * A0112);

    return ret;
}

mat4 mat4::perspective(float fov, float aspect, float near, float far) {
    // https://gamedev.stackexchange.com/a/120355
    mat4 ret;

    fov = fov * (float)M_PI / 180.0f; // math.h tan uses radians
    ret(0, 0) = 1.0f / (aspect * tan(fov / 2.0f));
    ret(1, 1) = 1.0f / (tan(fov / 2.0f));
    ret(2, 2) = -(far + near) / (far - near);
    ret(2, 3) = -(2 * far * near) / (far - near);
    ret(3, 2) = -1;

    return ret;
}

mat4 mat4::lookAt(const vec3 &position, const vec3 &target, const vec3 &up) {
    // GLM-like lookAt function for easy camera management
    // https://stackoverflow.com/a/19740748

    mat4 ret(1);

    vec3 f = normalise(target - position);
    vec3 s = normalise(cross(f, up));
    vec3 u = cross(s, f);

    ret(0, 0) = s.x;
    ret(0, 1) = s.y;
    ret(0, 2) = s.z;
    ret(1, 0) = u.x;
    ret(1, 1) = u.y;
    ret(1, 2) = u.z;
    ret(2, 0) = -f.x;
    ret(2, 1) = -f.y;
    ret(2, 2) = -f.z;

    ret(0, 3) = -dot(s, position);
    ret(1, 3) = -dot(u, position);
    ret(2, 3) = dot(f, position);

    return ret;
}

mat4 mat4::transform(const vec3 &position, const vec3 &rotation,
                     const vec3 &scale) {
    mat4 ret(1);

    // Convert from degrees to radians
    vec3 r = vec3{rotation.x * (float)M_PI / 180.0f,
                            rotation.y * (float)M_PI / 180.0f,
                            rotation.z * (float)M_PI / 180.0f};

    ret(0, 0) = cos(r.y) * cos(r.z);
    ret(1, 0) = cos(r.y) * sin(r.z);
    ret(2, 0) = -sin(r.y);
    ret(3, 0) = 0.0f;

    ret(0, 1) = (sin(r.x) * sin(r.y) * cos(r.z)) - (cos(r.x) * sin(r.z));
    ret(1, 1) = (sin(r.x) * sin(r.y) * sin(r.z)) + (cos(r.x) * cos(r.z));
    ret(2, 1) = (sin(r.x) * cos(r.y));
    ret(3, 1) = 0.0f;

    ret(0, 2) = (cos(r.x) * sin(r.y) * cos(r.z)) + (sin(r.x) * sin(r.z));
    ret(1, 2) = (cos(r.x) * sin(r.y) * sin(r.z)) - (sin(r.x) * cos(r.z));
    ret(2, 2) = (cos(r.x) * cos(r.y));
    ret(3, 2) = 0.0f;

    ret(0, 3) = position.x;
    ret(1, 3) = position.y;
    ret(2, 3) = position.z;
    ret(3, 3) = 1.0f;

    mat4 scaleMatrix(1.0f);
    scaleMatrix(0, 0) = scale.x;
    scaleMatrix(1, 1) = scale.y;
    scaleMatrix(2, 2) = scale.z;
    ret = ret * scaleMatrix;

    return ret;
}

std::ostream &operator<<(std::ostream &out, const mat4 &mat) {
    for (int i = 0; i < 4; i++) {
        out << "|";
        for (int j = 0; j < 4; j++) {
            out << std::setprecision(3) << mat(i, j) << "\t|";
        }
        out << "\n";
    }
    return out;
}

mat4 operator*(const mat4 &m1, const mat4 &m2) {
    mat4 ret;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ret(i, j) = 0;
            for (int k = 0; k < 4; k++)
                ret(i, j) += m1(i, k) * m2(k, j);
        }
    }

    return ret;
}

//くコ:彡