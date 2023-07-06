#ifndef COMPUTE_PIPELINE_H
#define COMPUTE_PIPELINE_H

#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <vulkan/vulkan.h>
#include <cassert>

#include "engine_device.h"
#include "engine_mesh.h"


namespace Engine{



class ComputePipeline{
public:

	ComputePipeline(
	    EngineDevice &device,
	    const std::string& marchingCubesFilePath)
	    : engineDevice(device)
	{
	    createComputePipeline(marchingCubesFilePath);
	}


	~ComputePipeline() {
		vkDestroyShaderModule(engineDevice.device(), marchingCubesShaderModule, nullptr);
		vkDestroyPipeline(engineDevice.device(), computePipeline, nullptr);
	}

	ComputePipeline(const ComputePipeline&) = delete;
	ComputePipeline& operator=(const ComputePipeline&) = delete;

	void bind(VkCommandBuffer commandBuffer) {
	    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
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


	void createComputePipeline(
		const std::string& marchingCubesFilePath){

		auto computeShaderCode = readFile(marchingCubesFilePath);
		CreateShaderModule(computeShaderCode, &marchingCubesShaderModule);


		VkPipelineShaderStageCreateInfo computeShaderStageCreateInfo{};
		computeShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		computeShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		computeShaderStageCreateInfo.module = marchingCubesShaderModule;
		computeShaderStageCreateInfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.stage = computeShaderStageCreateInfo;
		computePipelineCreateInfo.layout = nullptr;

		if (vkCreateComputePipelines(engineDevice.device(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &computePipeline) != VK_SUCCESS) {
		    throw std::runtime_error("Failed to create compute pipeline");
		}

		vkDestroyShaderModule(engineDevice.device(), marchingCubesShaderModule, nullptr);
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
	VkPipeline computePipeline;
	VkShaderModule marchingCubesShaderModule;
};
} // namespace
#endif
