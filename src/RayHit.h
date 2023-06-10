#ifndef RAYHIT_H
#define RAYHIT_H

#include "Vector.h"
#include "Colour.h"
#include "Material.h"
#include "Ray.h"

class RayHit {
    Ray ray;
    Vector hitPosition;
    Vector hitNormal;
    double hitDistance;
    Material meshMaterial;
    bool didHit;

public:
    RayHit(Ray _ray = Ray(Vector(0,0,0), Vector(0,0,0)), Vector _hitPosition = Vector(0,0,0), double _hitDistance = std::numeric_limits<double>::infinity(), Vector _hitNormal = Vector(0,0,0), const Material _meshMaterial = Material(), bool _didHit = false)
    : ray(_ray), hitPosition(_hitPosition), hitDistance(_hitDistance), hitNormal(_hitNormal), meshMaterial(_meshMaterial), didHit(_didHit) {}

    Ray getRay() {return ray;}
    Vector getHitPosition() {return hitPosition;}
    Vector getHitNormal() {return hitNormal;}
    Colour getMeshColour() {return meshMaterial.getColour();}
    Material getMeshMaterial() {return meshMaterial;}
    double getHitDistance() {return hitDistance;}
    bool DidHit() {return didHit;}
};

#endif