#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"


class Camera{
public:

	Vector position, rotation

	Camera(Vector pos, Vector rot, int w, int h)
	: position(pos), rotation(rot), width(w), height(h) {}

	int getWidth() const { return width; }
    int getHeight() const { return height; }
    double getFOV() const { return fov; }


private:
	double fov;
	int width, height;
};

#endif