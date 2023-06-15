#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../vendor/glm/glm.hpp"
#include<cassert>
#include <limits>


namespace Engine{

class Camera {
public:

	Camera(float _fov = 80.0f, float _near = 1.0f, float _far = 1000.0f)
	: fov(_fov), near(_near), far(_far) {}

	void setOrthographicProjection(
		float left, 
		float right, 
		float top, 
		float bottom, 
		float near, 
		float far)
	{
		projectionMatrix = glm::mat4{1.0f};
		projectionMatrix[0][0] = 2.f / (right - left);
		projectionMatrix[1][1] = 2.f / (bottom - top);
		projectionMatrix[2][2] = 1.f / (far - near);
		projectionMatrix[3][0] = -(right + left) / (right - left);
		projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		projectionMatrix[3][2] = -near / (far - near);
	}

	void setPerspectiveProjection(float aspect) {
	    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
	    const float tanHalfFovx = tan(fov / 2.f);
	    const float tanHalfFovy = tanHalfFovx / aspect;
	    projectionMatrix = glm::mat4{0.0f};
	    projectionMatrix[0][0] = 1.f / tanHalfFovx;
	    projectionMatrix[1][1] = 1.f / tanHalfFovy;
	    projectionMatrix[2][2] = far / (far - near);
	    projectionMatrix[2][3] = 1.f;
	    projectionMatrix[3][2] = -(far * near) / (far - near);
	}

	void setViewDirection(
		glm::vec3 position, 
		glm::vec3 forward, 
		glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f})
	{
		const glm::vec3 w{glm::normalize(forward)};
		const glm::vec3 u{glm::normalize(glm::cross(w, up))};
		const glm::vec3 v{glm::cross(w, u)};

		viewMatrix = glm::mat4{1.f};
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, position);
		viewMatrix[3][1] = -glm::dot(v, position);
		viewMatrix[3][2] = -glm::dot(w, position);
	}

	void setViewTarget(
		glm::vec3 position, 
		glm::vec3 target, 
		glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f})
	{
		setViewDirection(position, target - position, up);
	}

	void setView()
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
		const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
		const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
		viewMatrix = glm::mat4{1.f};
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, position);
		viewMatrix[3][1] = -glm::dot(v, position);
		viewMatrix[3][2] = -glm::dot(w, position);
	}


	const glm::mat4& getProjection() const {return projectionMatrix;}
	const glm::mat4& getView() const {return viewMatrix;}

	glm::vec3 Forward(){
		float yaw = rotation.y;
  		return glm::vec3{sin(yaw), 0.0f, cos(yaw)};
	}

	glm::vec3 Right() {
    	float yaw = rotation.y;
    	glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
    	glm::vec3 rightDir = glm::normalize(glm::cross(forwardDir, glm::vec3{0.0f, 1.0f, 0.0f}));
    	return rightDir;
	}

	glm::vec3 Up() {
    	float yaw = rotation.y;
    	glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
    	glm::vec3 upDir{0.0f, forwardDir.y, 0.0f};
    	return upDir;
	}


	glm::vec3 position{};
	glm::vec3 rotation{};

private:
	glm::mat4 projectionMatrix{1.f};
	glm::mat4 viewMatrix{1.f};

	float fov;
	float near;
	float far;
};
} // namespace

#endif
