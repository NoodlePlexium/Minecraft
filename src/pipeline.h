#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <vulkan/vulkan.h>

#include "vulkan_device.h"

namespace Engine{

struct PipelineConfigInfo{};


class RenderPipeline{
public:
	RenderPipeline(
	    EngineDevice &device,
	    const std::string& vertFilePath,
	    const std::string& fragFilePath,
	    const PipelineConfigInfo configInfo)
	    : engineDevice(device)
	{
	    createRenderPipeline(vertFilePath, fragFilePath, configInfo);
	}


	~RenderPipeline() {}

	RenderPipeline(const RenderPipeline&) = delete;
	void operator=(const RenderPipeline&) = delete;

	static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height){
		PipelineConfigInfo configInfo{};
		return configInfo;
	}

private:

	static std::vector<char> readFile(const std::string& filePath){
		std::ifstream file{filePath, std::ios::ate | std::ios::binary};

		if (!file.is_open()){throw std::runtime_error("failed to open file: " + filePath);}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void createRenderPipeline(
		const std::string& vertFilePath, 
		const std::string& fragFilePath,
		const PipelineConfigInfo& configInfo){

		auto vertCode = readFile(vertFilePath);
		auto fragCode = readFile(fragFilePath);

		std::cout << "Vertex Shader Code Size: " << vertCode.size() << std::endl;
		std::cout << "Fragment Shader Code Size: " << fragCode.size() << std::endl;
	}

	void CreateShaderModule(const std::vector<char>& code, VkShaderModule * shaderModule){
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(engineDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS){
			throw std::runtime_error("failed to create shader module");
		}
	}

	EngineDevice &engineDevice;
	VkPipeline graphicsPipeline;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
};

} // namespace



#endif
