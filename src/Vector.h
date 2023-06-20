#ifndef VECTOR_H
#define VECTOR_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../vendor/glm/glm.hpp"
#include <cmath>
#include <cstdlib>

namespace Engine{
class Vector3{
public:
	float x;
	float y;
	float z;		

	// CONSTRUCTOR
	Vector3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
    : x(_x), y(_y), z(_z) {}

    Vector3(const glm::vec3& v)
    : x(v.x), y(v.y), z(v.z) {}



    float magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 normalized() const {
        float mag = magnitude();
        return Vector3(x / mag, y / mag, z / mag);
    }

    Vector3 negative() const {
        return Vector3(-x, -y, -z);
    }

    float dotProduct(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    float distance(const Vector3& v) const {
        Vector3 diff = v - (*this);
        float squaredDistance = diff.dotProduct(diff);
        return std::sqrt(squaredDistance);
    }

    Vector3 crossProduct(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }


    // Conversion
    static Vector3 glmToVector3(const glm::vec3& v) {
    	return Vector3(v.x, v.y, v.z);
    }

    glm::vec3 toGLMVec3() const {
        return glm::vec3(x, y, z);
    }

    // OPERATORS //////////////////////////////////////////////
    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(float scalar) const {
	    return Vector3(x * scalar, y * scalar, z * scalar);
	}


    Vector3 operator*(const Vector3& v) const {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

};


class Vector2 {
public:
    float x;
    float y;

    // CONSTRUCTOR
    Vector2(float _x = 0.0f, float _y = 0.0f)
        : x(_x), y(_y) {}

    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2 normalized() const {
        float mag = magnitude();
        return Vector2(x / mag, y / mag);
    }

    Vector2 negative() const {
        return Vector2(-x, -y);
    }

    float distance(const Vector2& v) const {
        Vector2 diff = v - (*this);
        float squaredDistance = diff.x * diff.x + diff.y * diff.y;
        return std::sqrt(squaredDistance);
    }

    // OPERATORS //////////////////////////////////////////////
    Vector2 operator+(const Vector2& v) const {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator-(const Vector2& v) const {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator*(double scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 multiply(const Vector2& v) const {
        return Vector2(x * v.x, y * v.y);
    }
};


} // namespace

#endif
