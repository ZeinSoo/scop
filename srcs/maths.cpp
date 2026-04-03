#include "maths.hpp"

float dot(const Vec3 &a, const Vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(const Vec3 &a, const Vec3 &b) {
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

float radians(float deg) {
    return deg * PI / 180.0f;
}

float degrees(float rad) {
    return rad * 180.0f / PI;
}

std::ostream &operator<<(std::ostream &os, const Mat4 &mat) {
    os << "Mat4:" << std::endl;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            os << mat.at(row, col) << " ";
        }
        os << std::endl;
    }
    return os;
}

#include <iomanip>
std::ostream &operator<<(std::ostream &os, const Vec3 &vec) {
    os << "Vec3:\n" 
        << "x: " << std::setprecision(7) << vec.x << "\n"
        << "y: " << std::setprecision(7) << vec.y << "\n"
        << "z: " << std::setprecision(7) << vec.z << std::endl;
    return os;
}