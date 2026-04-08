#pragma once

#include <iostream>
#include <cmath>

#define PI 3.141592653589793238462643383279502884L

struct Vec2 {
    float x;
    float y;

    Vec2(): x(0.0f), y(0.0f) {}
    Vec2(float x, float y): x(x), y(y) {}
};

struct Vec3 {
    float x;
    float y;
    float z;

    Vec3(): x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z): x(x), y(y), z(z) {}
    Vec3(float v): x(v), y(v), z(v) {}

    Vec3 normalize() const {
        float len = sqrt(x*x + y*y + z*z);
        return Vec3(x / len, y / len, z / len);
    }

    Vec3 opposite() const {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator+(const Vec3 &other) const {
        return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
    }

    Vec3 operator-(const Vec3 &other) const {
        return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
    }

    Vec3 operator+(const float &scalar) const {
        return Vec3(this->x + scalar, this->y + scalar, this->z + scalar);
    }

    Vec3 operator-(const float &scalar) const {
        return Vec3(this->x - scalar, this->y - scalar, this->z - scalar);
    }

    Vec3 operator+=(const float &scalar) {
        this->x += scalar;
        this->y += scalar;
        this->z += scalar;
        return *this;
    }

    Vec3 operator-=(const float &scalar) {
        this->x -= scalar;
        this->y -= scalar;
        this->z -= scalar;
        return *this;
    }

};

struct Vertex {
    Vec3 position;
    Vec3 color;
    Vec2 texCoords;
    Vec3 normal;
};

float dot(const Vec3 &a, const Vec3 &b);

Vec3 cross(const Vec3 &a, const Vec3 &b);

struct Mat4 {
    float m[16];

    Mat4(float *arr) {
        for (int row = 0; row < 4; row++)
            for (int col = 0; col < 4; col++)
                m[col * 4 + row] = arr[row * 4 + col];
    }

    Mat4(float diag = 1.0f) {
        for (int i = 0; i < 16; i++)
            m[i] = 0.0f;
        m[0] = diag;
        m[5] = diag;
        m[10] = diag;
        m[15] = diag;
    }

    void reset(float diag = 1.0f) {
        for (int i = 0; i < 16; i++)
            m[i] = 0.0f;
        m[0] = diag;
        m[5] = diag;
        m[10] = diag;
        m[15] = diag;
    }

    // Surcharge operator for matrix
    Mat4 operator*(const Mat4 &other) const {
        Mat4 res(0.f);
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                for (int k = 0; k < 4; k++) {
                    res.at(row, col) += this->at(row, k) * other.at(k, col);
                }
            }
        }
        return res;
    }

    // Operations on matrices
    Mat4 translate(const Mat4 &base, const Vec3 &t) {
        Mat4 res = base;
        
        res[12] = base[0] * t.x + base[4] * t.y + base[8] * t.z + base[12]; 
        res[13] = base[1] * t.x + base[5] * t.y + base[9] * t.z + base[13];
        res[14] = base[2] * t.x + base[6] * t.y + base[10] * t.z + base[14];
        return res;
    }

    Mat4 scale(const Mat4 &base, const Vec3 &s) {
        Mat4 res = base;

        res[0] *= s.x; res[1] *= s.x; res[2] *= s.x; res[3] *= s.x;
        res[4] *= s.y; res[5] *= s.y; res[6] *= s.y; res[7] *= s.y;
        res[8] *= s.z; res[9] *= s.z; res[10] *= s.z; res[11] *= s.z;
        return res;
    }

    Mat4 rotate(const Mat4 &base, float angle, const Vec3 &axis) {
        float len = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
        float x = axis.x / len;
        float y = axis.y / len;
        float z = axis.z / len;

        float c = cos(angle);
        float s = sin(angle);
        float t = 1.f - c;

        Mat4 R(0.f);

        R[0] = t * x * x + c;
        R[1] = t * x * y + s * z;
        R[2] = t * x * z - s * y;

        R[4] = t * x * y - s * z;
        R[5] = t * y * y + c;
        R[6] = t * y * z + s * x;

        R[8] = t * x * z + s * y;
        R[9] = t * y * z - s * x;
        R[10] = t * z * z + c;

        R[15] = 1.f;

        return base * R;
    }

    Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up) {
        Mat4 res(0.f);
        Vec3 f = (eye - center).normalize();
        Vec3 r = cross(up, f).normalize();
        Vec3 u = cross(f, r);

        res[0] = r.x;
        res[1] = u.x;
        res[2] = f.x;
        res[3] = 0.f;

        res[4] = r.y;
        res[5] = u.y;
        res[6] = f.y;
        res[7] = 0.f;

        res[8] = r.z;
        res[9] = u.z;
        res[10] = f.z;
        res[11] = 0.f;

        res[12] = -dot(r, eye);
        res[13] = -dot(u, eye);
        res[14] = -dot(f, eye);
        res[15] = 1.f;
        return res;
    }

    Mat4 perspective(float fov, float aspect, float near, float far) {
        Mat4 res(0.f);

        float tanHalfFov = tan(fov / 2.f);
        float top = near * tanHalfFov;
        float right = top * aspect;

        res[0] = near / right;
        res[5] = near / top;
        res[10] = -(far + near) / (far - near);
        res[11] = -1.f;
        res[14] = -(2.f * far * near) / (far - near);
        return res;
    }

    // Accessors
    float &at(int row, int col) {
        return m[col * 4 + row];
    }

    const float &at(int row, int col) const {
        return m[col * 4 + row];
    }

    float &operator[](int i) {
        return m[i];
    }

    const float &operator[](int i) const {
        return m[i];
    }

};

float radians(float deg);

float degrees(float rad);

std::ostream &operator<<(std::ostream &os, const Mat4 &mat);

std::ostream &operator<<(std::ostream &os, const Vec3 &vec);