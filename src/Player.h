#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "InputSystem.h"
#include "engine_physics.h"
#include "app.h"

namespace Engine{
class Player{
public:

	Player(Camera _camera, InputSystem _input, float aspect)
        : camera(_camera), input(_input)
    {
        input.SetMouseMode(MouseMode::Play);
    }



	void Update(float frameTime, float aspect){

		// Update input system state
    	input.UpdateInputs();


		glm::vec2 moveInput = input.Movement() * 10.0f * frameTime;
    	glm::vec2 mouseLook = input.MouseLook() * 0.00045f;
    	glm::vec3 move = moveInput.x * camera.Right() + glm::vec3(0.0f, input.MovementY() * 10.0f * frameTime, 0.0f) + moveInput.y * camera.Forward();
    	glm::vec3 rot{mouseLook.y, -mouseLook.x, 0.0f};


    	camera.position += move;
    	camera.rotation += rot;
    	camera.rotation.x = glm::clamp(camera.rotation.x, -glm::pi<float>() * 0.5f, glm::pi<float>() * 0.5f); // clamp

		// set camera view
		camera.setView();
		camera.setPerspectiveProjection(aspect);

		//RayHit hit = Physics::RayCast(Vector3(camera.position), Vector3(camera.Forward()));

		if (input.GetKeyDown(InputSystem::KeyCode::Escape) && input.GetMouseMode() == MouseMode::Play) input.SetMouseMode(MouseMode::Normal);
        else input.SetMouseMode(MouseMode::Play);
	}


	struct CapsuleCollider{
		Vector3 position;
		float radius;
		float height;
	};

	glm::vec3 getPlayerPosition(){
		return camera.position;
	}

	// Public member variables
	Camera camera;	

private:

	// Private member variables
    InputSystem input;
};
}

#endif
