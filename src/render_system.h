#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/constants.hpp"

#include "pipeline.h"
#include "engine_device.h"
#include "engine_game_object.h"
#include "camera.h"
#include "engine_frame_info.h"
#include "terrain.h"


#include <memory>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <array>

namespace Engine{

struct SimplePushConstantData{
	glm::mat4 meshMatrix{1.0f};
	glm::mat4 normalMatrix{1.0f};
};

class RenderSystem{
public:

	RenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : engineDevice{device} 
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}


	~RenderSystem() {vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);};
	
	RenderSystem(const RenderSystem &) = delete;
	RenderSystem &operator=(const RenderSystem &) = delete;	


	void renderGameObjects(FrameInfo &frameInfo, std::vector<EngineGameObject>& gameObjects, Terrain& terrain)
	{
		graphicsPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 
			1, 
			&frameInfo.globalDescriptorSet,
			0, 
			nullptr);

		// Render game objects
		for (auto& obj : gameObjects){

			SimplePushConstantData push{};
			push.meshMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer, 
				pipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(SimplePushConstantData), 
				&push);
			obj.mesh->bind(frameInfo.commandBuffer);
			obj.mesh->draw(frameInfo.commandBuffer);
		}

		// Render terrain
		for (auto& obj : terrain.chunkObjects){

			SimplePushConstantData push{};
			push.meshMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer, 
				pipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(SimplePushConstantData), 
				&push);
			obj.mesh->bind(frameInfo.commandBuffer);
			obj.mesh->draw(frameInfo.commandBuffer);
		}
	}


private:

	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};



		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		GraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;	
		pipelineConfig.pipelineLayout = pipelineLayout;
		graphicsPipeline = std::make_unique<GraphicsPipeline>(
			engineDevice, 
			"shaders/shader.vert.spv", 
			"shaders/shader.frag.spv", 
			pipelineConfig);
	}


    EngineDevice& engineDevice;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    VkPipelineLayout pipelineLayout;
};



} // namespace


#endif
