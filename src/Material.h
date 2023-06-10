#ifndef MATERIAL_H
#define MATERIAL_H

#include "Colour.h"

class Material {
    double roughness;
    double metalness;
    double emissionStrength;
    Colour colour;

public:
    Material(double _roughness = 0.5, double _metalness = 0, double _emissionStrength = 0, const Colour& _colour = Colour())
        : roughness(_roughness), metalness(_metalness), emissionStrength(_emissionStrength), colour(_colour) {}

    Colour getColour() const { return colour; }
    double getRoughness() const { return roughness; }
    double getMetalness() const { return metalness; }
    double getEmissionStrength() const { return emissionStrength; }
};

#endif
