#ifndef APP_H
#define APP_H

#include "GameWindow.h"
#include "pipeline.h"
#include "engine_swap_chain.h"
#include "engine_device.h"
#include "engine_model.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <array>

namespace Engine{

class App{
public:
	static constexpr int width = 800;
	static constexpr int height = 600;

	App() 
	{
		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}


	~App() {vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);}
	
	App(const App &) = delete;
	App &operator=(const App &) = delete;	


	void run(){
		while(!window.shouldClose()){
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(engineDevice.device());
	}


private:

	void loadModels(){
		std::vector<EngineModel::Vertex> vertices {
			{{0.0f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}}
		};

		engineModel = std::make_unique<EngineModel>(engineDevice, vertices);
	}

	void createPipelineLayout(){
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void createPipeline(){

		std::cout << "Width: " << engineSwapChain.getSwapChainExtent().width << std::endl;


		auto pipelineConfig = 
			GraphicsPipeline::defaultPipelineConfigInfo(engineSwapChain.getSwapChainExtent().width, engineSwapChain.getSwapChainExtent().height);

		pipelineConfig.renderPass  = engineSwapChain.getRenderPass();	
		pipelineConfig.pipelineLayout = pipelineLayout;
		graphicsPipeline = std::make_unique<GraphicsPipeline>(
			engineDevice, 
			"shaders/shader.vert.spv", 
			"shaders/shader.frag.spv", 
			pipelineConfig);
	}

	void createCommandBuffers() {
		commandBuffers.resize(engineSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = engineDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(engineDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for (int i = 0; i < commandBuffers.size(); i++){
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS){
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = engineSwapChain.getRenderPass();
			renderPassInfo.framebuffer = engineSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = engineSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = {0.1f, 0.1f, 0.1f, 0.1f};
			clearValues[1].depthStencil = {1.0f, 0};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);	
			
			graphicsPipeline->bind(commandBuffers[i]);
			engineModel->bind(commandBuffers[i]);
			engineModel->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	void drawFrame() {
		uint32_t imageIndex;
		auto result = engineSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
			throw std::runtime_error("failed to aquire swap chain image!");
		}

		result = engineSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS){
			throw std::runtime_error("failed to present swap chain image!");
		}
	}


	GameWindow window{width, height, "Minecraft"};
    EngineDevice engineDevice{window};
    EngineSwapChain engineSwapChain{engineDevice, window.getExtent()};
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::unique_ptr<EngineModel> engineModel;
};



} // namespace


#endif
