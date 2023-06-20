#ifndef ENGINE_PHYSICS_H
#define ENGINE_PHYSICS_H

#include "Vector.h"
#include "App.h"
#include "engine_mesh.h"
#include "engine_game_object.h"

namespace Engine{


class Ray {
public:
    Ray(const Vector3& _origin, const Vector3& _direction) : origin(_origin), direction(_direction) {}

    Vector3 getOrigin() const { return origin; }
    Vector3 getDirection() const { return direction; }

    void setOrigin(Vector3 newOrigin) { origin = newOrigin;}
    void setDirection(Vector3 newDirection) { direction = newDirection; }

private:    
    Vector3 origin, direction;  
};

class RayHit {
    Ray ray;
    Vector3 point;
    Vector3 normal;
    float distance;
    bool hit;
    EngineGameObject* gameObject;

public:
    RayHit(
    Ray _ray = Ray(Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f)), 
    Vector3 _point = Vector3(0.0f,0.0f,0.0f), 
    float _distance = std::numeric_limits<float>::infinity(), 
    Vector3 _normal = Vector3(0.0f,0.0f,0.0f), 
    bool _hit = false,
    EngineGameObject* _gameObject = nullptr)
    : ray(_ray), point(_point), distance(_distance), normal(_normal), hit(_hit), gameObject(_gameObject) {}

    Ray getRay() {return ray;}
    Vector3 getPoint() const {return point;}
    Vector3 getNormal() const {return normal;}
    float getDistance() const {return distance;}
    bool didHit() const {return hit;}
    EngineGameObject* getObjectPtr() const {return gameObject;}
};




class Physics {
public:

    static RayHit RayCast(const Vector3& origin, const Vector3& direction) {

        float closestHitDistance = std::numeric_limits<float>::infinity();
        RayHit returnHit;

        // For each gameobject check intersection with each face
        for (EngineGameObject* gameObject : gameObjects) {

            std::shared_ptr<Mesh> mesh = gameObject->getMesh();

            std::vector<Mesh::Vertex> vertices = mesh->getVertices();
            std::vector<uint32_t> indices = mesh->getIndices();

            // No index buffer
            if (indices.size() == 0){
                for (size_t i = 0; i < vertices.size(); i += 3) { // Loop over vertices

                    Mesh::Vertex& vert1 = vertices[i];
                    Mesh::Vertex& vert2 = vertices[i + 1];
                    Mesh::Vertex& vert3 = vertices[i + 2];

                    Vector3 v1(vert1.position.x, vert1.position.y, vert1.position.z);
                    Vector3 v2(vert2.position.x, vert2.position.y, vert2.position.z);
                    Vector3 v3(vert3.position.x, vert3.position.y, vert3.position.z);

                    Ray ray(origin, direction);
                    RayHit hit = RayTriangleIntersect(ray, gameObject, v1, v2, v3);

                    if (hit.getDistance() < closestHitDistance){
                        closestHitDistance = hit.getDistance();
                        returnHit = hit;
                    }
                }
            }
            else
            {
                // Loop over the indices of the builder
                for (size_t i = 0; i < indices.size(); i += 3) { // Loop over indices

                    Mesh::Vertex& vert1 = vertices[indices[i]];
                    Mesh::Vertex& vert2 = vertices[indices[i + 1]];
                    Mesh::Vertex& vert3 = vertices[indices[i + 2]];

                    Vector3 v1(vert1.position.x, vert1.position.y, vert1.position.z);
                    Vector3 v2(vert2.position.x, vert2.position.y, vert2.position.z);
                    Vector3 v3(vert3.position.x, vert3.position.y, vert3.position.z);

                    Ray ray(origin, direction);
                    RayHit hit = RayTriangleIntersect(ray, gameObject, v1, v2, v3);

                    if (hit.getDistance() < closestHitDistance){
                        closestHitDistance = hit.getDistance();
                        returnHit = hit;
                    }
                }
            }
        }
        return returnHit; // Return the closest hit result
    }

    void SetGameObjectPtrs(std::vector<EngineGameObject*> _gameObjects){
        gameObjects = _gameObjects;
    }


private:
    static std::vector<EngineGameObject*> gameObjects;

    static RayHit RayTriangleIntersect(const Ray& ray, EngineGameObject* gameObject, const Vector3& vertex1, const Vector3& vertex2, const Vector3& vertex3) {
        Vector3 edge1 = vertex2 - vertex1;
        Vector3 edge2 = vertex3 - vertex1;

        Vector3 p = ray.getDirection().crossProduct(edge2);
        float determinant = edge1.dotProduct(p);

        if (determinant == 0.0)
            return RayHit(ray);

        float invDeterminant = 1.0 / determinant;

        Vector3 origin = ray.getOrigin() - vertex1;
        float u = origin.dotProduct(p) * invDeterminant;

        if (u < 0.0 || u > 1.0)
            return RayHit(ray);

        Vector3 q = origin.crossProduct(edge1);
        float v = ray.getDirection().dotProduct(q) * invDeterminant;

        if (v < 0.0 || u + v > 1.0)
            return RayHit(ray);

        float dist = edge2.dotProduct(q) * invDeterminant;

        if (dist < 0.0)
            return RayHit(ray);

        Vector3 intersectionPos = ray.getOrigin() + ray.getDirection() * dist;
        Vector3 faceNormal = edge1.crossProduct(edge2).normalized();

        return RayHit(ray, intersectionPos, dist, faceNormal, true, gameObject);
    }
};
std::vector<EngineGameObject*> Physics::gameObjects;
} // namespace


#endif