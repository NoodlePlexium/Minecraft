#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

class Vector {
    double x, y, z;

public:
    Vector(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {}

    double x, y, z;

    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector normalized() const {
        double mag = magnitude();
        return Vector(x / mag, y / mag, z / mag);
    }

    Vector negative() const {
        return Vector(-x, -y, -z);
    }

    double dotProduct(const Vector& v) const {
        return x * v.getX() + y * v.getY() + z * v.getZ();
    }

    double distance(const Vector& v) const {
        Vector diff = v - (*this);
        double squaredDistance = diff.dotProduct(diff);
        return std::sqrt(squaredDistance);
    }


    Vector crossProduct(const Vector& v) const {
        return Vector(
            y * v.getZ() - z * v.getY(),
            z * v.getX() - x * v.getZ(),
            x * v.getY() - y * v.getX()
        );
    }

    Vector operator+(const Vector& v) const {
        return Vector(x + v.getX(), y + v.getY(), z + v.getZ());
    }

    Vector operator-(const Vector& v) const {
        return Vector(x - v.getX(), y - v.getY(), z - v.getZ());
    }

    Vector operator*(double scalar) const {
        return Vector(x * scalar, y * scalar, z * scalar);
    }

    Vector multiply(const Vector& v) const {
        return Vector(x * v.getX(), y * v.getY(), z * v.getZ());
    }


    Vector rotate(const Vector& angles) const {

        // PI constant
        double Pi = 3.14159265358979323846;

        // Convert rotation angles from degrees to radians
        double angleX = angles.getX() * (Pi / 180.0);
        double angleY = angles.getY() * (Pi  / 180.0);
        double angleZ = angles.getZ() * (Pi / 180.0);

        // Apply rotations around X, Y, and Z axes
        Vector rotatedVector = *this;

        // Rotation around X-axis
        double cosX = std::cos(angleX);
        double sinX = std::sin(angleX);
        double rotatedY = rotatedVector.getY() * cosX - rotatedVector.getZ() * sinX;
        double rotatedZ = rotatedVector.getY() * sinX + rotatedVector.getZ() * cosX;
        rotatedVector = Vector(rotatedVector.getX(), rotatedY, rotatedZ);

        // Rotation around Y-axis
        double cosY = std::cos(angleY);
        double sinY = std::sin(angleY);
        double rotatedZ2 = rotatedVector.getZ() * cosY - rotatedVector.getX() * sinY;
        double rotatedX = rotatedVector.getZ() * sinY + rotatedVector.getX() * cosY;
        rotatedVector = Vector(rotatedX, rotatedVector.getY(), rotatedZ2);

        // Rotation around Z-axis
        double cosZ = std::cos(angleZ);
        double sinZ = std::sin(angleZ);
        double rotatedX2 = rotatedVector.getX() * cosZ - rotatedVector.getY() * sinZ;
        double rotatedY2 = rotatedVector.getX() * sinZ + rotatedVector.getY() * cosZ;
        rotatedVector = Vector(rotatedX2, rotatedY2, rotatedVector.getZ());

        return rotatedVector;
    }

    static Vector RandomDirectionInHemisphere(const Vector& normal) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

        float theta = 2.0f * 3.1415926f * distribution(gen);
        float phi = std::acos(1 - 2 * distribution(gen));
        float x = std::sin(phi) * std::cos(theta);
        float y = std::sin(phi) * std::sin(theta);
        float z = std::cos(phi);

        Vector randomDirection(x, y, z);
        if (randomDirection.dotProduct(normal) < 0.0f) {
            randomDirection = randomDirection * -1.0f;
        }

        return randomDirection;
    }
};

#endif 