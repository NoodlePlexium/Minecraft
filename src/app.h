#ifndef APP_H
#define APP_H

#include "GameWindow.h"
#include "pipeline.h"
#include "engine_swap_chain.h"
#include "vulkan_device.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace Engine{

class App{
public:
	static constexpr int width = 800;
	static constexpr int height = 600;

	App() 
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}


	~App() {vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);}
	
	App(const App &) = delete;
	App &operator=(const App &) = delete;	


	void run(){
		window.Run();
	}


private:

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

	void createCommandBuffers() {};
	void drawFrame() {};


	GameWindow window{width, height, "Minecraft"};
    EngineDevice engineDevice{window};
    EngineSwapChain engineSwapChain{engineDevice, window.getExtent()};
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
};



} // namespace


#endif
