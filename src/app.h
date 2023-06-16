#ifndef APP_H
#define APP_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/constants.hpp"

#include "GameWindow.h"
#include "renderer.h"
#include "engine_device.h"
#include "engine_mesh.h"
#include "engine_game_object.h"
#include "render_system.h"
#include "camera.h"
#include "InputSystem.h"
#include "terrain_mesh.h"
#include "engine_buffer.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <array>
#include <chrono>

namespace Engine{

struct GlobalUbo {
	glm::mat4 projectionView{1.f};
	glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
};

class App{
public:
	static constexpr int width = 800;
	static constexpr int height = 600;

	App() {loadGameObjects();}

	~App() {}
	
	App(const App &) = delete;
	App &operator=(const App &) = delete;	


	void run() {

		EngineBuffer globalUboBuffer{
			engineDevice,
			sizeof(GlobalUbo),
			EngineSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			engineDevice.properties.limits.minUniformBufferOffsetAlignment,
		};
		globalUboBuffer.map();

	    // internal
	    float aspect = renderer.getAspectRatio();
	    auto currentTime = std::chrono::high_resolution_clock::now();
	    float frameTime;

	    // SCRIPTABLE ZONE //////////////////////////////////////////////////
	    Camera camera{};
	    camera.setPerspectiveProjection(aspect);
	    float camPitch;
	    InputSystem input{window.getGLFWwindow()};
	    input.setMouseModePlay();

	    auto Update = [&]() {

	    	// Update input system state
	    	input.UpdateInputs();
	  

	    	glm::vec2 moveInput = input.Movement() * 6.0f * frameTime;
	    	glm::vec2 mouseLook = input.MouseLook() * 0.00045f;

	    	glm::vec3 move = moveInput.x * camera.Right() + glm::vec3(0.0f, input.MovementY() * 6.0f * frameTime, 0.0f) + moveInput.y * camera.Forward();
	    	glm::vec3 rot{mouseLook.y, -mouseLook.x, 0.0f};


	    	camera.position += move;

	    	std::cout << move.x << std::endl;
	    	camera.rotation += rot;
	    	camera.rotation.x = glm::clamp(camera.rotation.x, -glm::pi<float>() * 0.5f, glm::pi<float>() * 0.5f); // clamp



			// set camera view
			camera.setView();
			aspect = renderer.getAspectRatio();
			camera.setPerspectiveProjection(aspect);
	    };
	    /////////////////////////////////////////////////////////////////////

	    // INTERNAL LOOP RUNS ONCE PER FRAME ///////////////////////////////
	    RenderSystem renderSystem{engineDevice, renderer.getSwapChainRenderPass()};
	    while (!window.shouldClose()) {
	        glfwPollEvents();

	        // calculates time elapsed since last frame
	        auto newTime = std::chrono::high_resolution_clock::now();
	        frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
	        currentTime = newTime;

	        // execute scripts before rendering to screen
	        Update();

	        if (auto commandBuffer = renderer.beginFrame()) {
	        	int frameIndex = renderer.getFrameIndex();
	        	FrameInfo frameInfo{
	        		frameIndex,
	        		frameTime,
	        		commandBuffer,
	        		camera
	        	};

	        	// update
	        	GlobalUbo ubo{};
	        	ubo.projectionView = camera.getProjection() * camera.getView();
	        	globalUboBuffer.writeToIndex(&ubo, frameIndex);
	        	globalUboBuffer.flushIndex(frameIndex);

	        	// render
	            renderer.beginSwapChainRenderPass(commandBuffer);
	            renderSystem.renderGameObjects(frameInfo, gameObjects);
	            renderer.endSwapChainRenderPass(commandBuffer);
	            renderer.endFrame();
	        }
	    }
	    vkDeviceWaitIdle(engineDevice.device());
	}



private:


	void loadGameObjects(){
		
		Engine::TerrainSettings settings{};
		TerrainMesh tMesh{settings};
		std::shared_ptr<Mesh> mesh = tMesh.GenerateTerrain(engineDevice);

		auto terrain = EngineGameObject::createGameObject();
		terrain.mesh = mesh;
		gameObjects.push_back(std::move(terrain));
	}

	GameWindow window{width, height, "Minecraft"};
    EngineDevice engineDevice{window};
    Renderer renderer{window, engineDevice};
    std::vector<EngineGameObject> gameObjects;
};



} // namespace


#endif
