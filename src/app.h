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
#include "terrain_render_system.h"
#include "camera.h"
#include "InputSystem.h"
#include "../terrain/terrain.h"
#include "engine_buffer.h"
#include "engine_descriptor.h"
#include "player.h"
#include "engine_physics.h"

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

	App() {
		// Descriptor set pool
		globalPool = EngineDescriptorPool::Builder(engineDevice)
		.setMaxSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
		.build();
		loadGameObjects();
	}

	~App() {}
	
	App(const App &) = delete;
	App &operator=(const App &) = delete;	


	void run() {

		// UNIFORM BUFFER OBJECT
		std::vector<std::unique_ptr<EngineBuffer>> uboBuffers(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i =0; i < uboBuffers.size(); i++){
			uboBuffers[i] = std::make_unique<EngineBuffer>(
				engineDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		
			uboBuffers[i]->map();
		}

		auto globalSetLayout = EngineDescriptorSetLayout::Builder(engineDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
		.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); ++i){
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			EngineDescriptorWriter(*globalSetLayout, *globalPool)
			.writeBuffer(0, &bufferInfo)
			.build(globalDescriptorSets[i]);
		}


		// WORLD TERRAIN ////////////////////////////////////////////////////
		loadTerrain();


	    // internal
	    float aspect = renderer.getAspectRatio();
	    auto currentTime = std::chrono::high_resolution_clock::now();
	    float frameTime;


	    // ENGINE PHYSICS ///////////////////////////////////////////////////   
		Physics physics{};
		physics.SetGameObjectPtrs(CreateGameObjectPtrs());

	    // SCRIPTABLE ZONE //////////////////////////////////////////////////
	    Camera camera{};
	    camera.setPerspectiveProjection(aspect);
	    InputSystem input{window.getGLFWwindow()};
	    Player player(camera, input, aspect);
	    /////////////////////////////////////////////////////////////////////

	    // RENDER SYSTEMS SETUP ///////////////////////////////
	    RenderSystem renderSystem{engineDevice, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()}; // Game Object Render System
		TerrainRenderSystem terrainRenderSystem{engineDevice, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()}; // Terrain Render System

		// INTERNAL LOOP RUNS ONCE PER FRAME ///////////////////////////////
	    while (!window.shouldClose()) {
	        glfwPollEvents();

	        // calculates time elapsed since last frame
	        auto newTime = std::chrono::high_resolution_clock::now();
	        frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
	        currentTime = newTime;

	        // execute scripts before rendering to screen
	        player.Update(frameTime, renderer.getAspectRatio());

	        // Update terrain
	       	glm::vec3 playerPos = player.getPlayerPosition();
			UpdateTerrain(playerPos.x, playerPos.z);



	        if (auto commandBuffer = renderer.beginFrame()) {
	        	int frameIndex = renderer.getFrameIndex();
	        	FrameInfo frameInfo{
	        		frameIndex,
	        		frameTime,
	        		commandBuffer,
	        		camera,
	        		globalDescriptorSets[frameIndex]
	        	};

	        	// update
	        	GlobalUbo ubo{};
	        	ubo.projectionView = player.camera.getProjection() * player.camera.getView();
	        	uboBuffers[frameIndex]->writeToBuffer(&ubo);
	        	uboBuffers[frameIndex]->flush();

	        	// render
	            renderer.beginSwapChainRenderPass(commandBuffer);
	            renderSystem.renderGameObjects(frameInfo, gameObjects);
				terrainRenderSystem.renderTerrain(frameInfo, terrain);
	            renderer.endSwapChainRenderPass(commandBuffer);
	            renderer.endFrame();
	        }
	    }
	    vkDeviceWaitIdle(engineDevice.device());
	}

	// Return a vector of pointers belonging to App
	std::vector<EngineGameObject*> CreateGameObjectPtrs(){
		std::vector<EngineGameObject*> gameObjectPtrs;
	    gameObjectPtrs.reserve(gameObjects.size());  // Reserve memory to avoid reallocations

	    for (auto& gameObject : gameObjects) {
	        gameObjectPtrs.push_back(&gameObject);
	    }
	    return gameObjectPtrs;
	}






private:


	void loadGameObjects(){
		
	}

	void loadTerrain() {
		Terrain::TerrainSettings terrainSettings;
		terrain = Terrain(terrainSettings);
	}

	void UpdateTerrain(float playerX, float playerZ) {
		terrain.UpdateChunks(20, playerX, playerZ, engineDevice);
	}


	GameWindow window{width, height, "World"};
    EngineDevice engineDevice{window};
    Renderer renderer{window, engineDevice};

    std::unique_ptr<EngineDescriptorPool> globalPool{};
    std::vector<EngineGameObject> gameObjects;

    // TERRAIN
	Terrain terrain;
};
} // namespace
#endif
