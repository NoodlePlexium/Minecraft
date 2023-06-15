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

#include <memory>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <array>

namespace Engine{

struct SimplePushConstantData{
	glm::mat4 transform{1.f};
	alignas(16) glm::vec3 colour;
};

class RenderSystem{
public:

	RenderSystem(EngineDevice& device, VkRenderPass renderPass) : engineDevice{device} 
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}


	~RenderSystem() {vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);};
	
	RenderSystem(const RenderSystem &) = delete;
	RenderSystem &operator=(const RenderSystem &) = delete;	


	void renderGameObjects(
		VkCommandBuffer commandBuffer, 
		std::vector<EngineGameObject>& gameObjects, 
		const Camera &camera)
	{
		graphicsPipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& obj : gameObjects){

			SimplePushConstantData push{};
			push.colour = obj.colour;
			push.transform = projectionView * obj.transform.mat4();

			vkCmdPushConstants(
				commandBuffer, 
				pipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(SimplePushConstantData), 
				&push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}


private:

	void createPipelineLayout(){

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void createPipeline(VkRenderPass renderPass){
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