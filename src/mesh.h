#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Vector.h"
#include "RayHit.h"
#include "Material.h"

class Mesh : public Object {
    Vector normal;
    Material material;

    // A list of all the vertices in the mesh
    std::vector<Vector> vertices;

public:
    Mesh(const Vector& position, const Vector& rotation, const Vector& scale, const Material& _material = Material(0.5, 0, 0, Colour(0.8, 0.8, 0.8, 1)))
        : Object(position, rotation, scale), material(_material) {
        CalculateVertices();
        CalculateNormal();
    }

    const Vector& getVertex(int index) const { return vertices[index]; }
    const Vector& Normal() const { return normal; }

    RayHit CalculateRayHit(const Ray& ray) const override {
        // Ray intersects with face 1 or 2
        RayHit hit1 = RayTriangleIntersect(ray, vertices[1], vertices[0], vertices[2]);
        RayHit hit2 = RayTriangleIntersect(ray, vertices[2], vertices[0], vertices[3]);

        RayHit hit;
        if (hit1.DidHit()) hit = hit1; // ray collided with face 1
        if (hit2.DidHit()) hit = hit2; // ray collided with face 2

        return hit;
    }

private:

    void CalculateNormal() {
        Vector edge1 = vertices[1] - vertices[0];
        Vector edge2 = vertices[2] - vertices[0];

        normal = edge1.crossProduct(edge2).normalized();
    }

    RayHit RayTriangleIntersect(const Ray& ray, const Vector& vertex1, const Vector& vertex2, const Vector& vertex3) const {
        Vector edge1 = vertex2 - vertex1;
        Vector edge2 = vertex3 - vertex1;

        Vector p = ray.getRayDirection().crossProduct(edge2);
        double determinant = edge1.dotProduct(p);

        if (determinant == 0.0)
            return RayHit(ray);

        double invDeterminant = 1.0 / determinant;

        Vector origin = ray.getRayOrigin() - vertex1;
        double u = origin.dotProduct(p) * invDeterminant;

        if (u < 0.0 || u > 1.0)
            return RayHit(ray);

        Vector q = origin.crossProduct(edge1);
        double v = ray.getRayDirection().dotProduct(q) * invDeterminant;

        if (v < 0.0 || u + v > 1.0)
            return RayHit(ray);

        double dist = edge2.dotProduct(q) * invDeterminant;

        if (dist < 0.0)
            return RayHit(ray);

        Vector intersectionPos = ray.getRayOrigin() + ray.getRayDirection() * dist;
        Vector faceNormal = edge1.crossProduct(edge2).normalized();

        return RayHit(ray, intersectionPos, dist, faceNormal, material, true);
    }
};

#endif