#ifndef MODEL_H
#define MODEL_H

#include "engine_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../vendor/glm/glm.hpp"
#include <vector>
#include <cassert>
#include <cstring>

namespace Engine{

class EngineModel{
public:

	struct Vertex{
		glm::vec3 position;
		glm::vec3 colour;
		
		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions(){
			std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
			bindingDescriptions[0].binding = 0;
			bindingDescriptions[0].stride = sizeof(Vertex);
			bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescriptions;
		}
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(){
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, colour);
			return attributeDescriptions;
		}
	};

	EngineModel(EngineDevice& _engineDevice, const std::vector<Vertex> &vertices) : engineDevice{_engineDevice} {
		creatVertexBuffers(vertices);
	}

	~EngineModel(){
		vkDestroyBuffer(engineDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(engineDevice.device(), vertexBufferMemory, nullptr);
	}
	
	EngineModel(const EngineModel &) = delete;
	EngineModel &operator=(const EngineModel &) = delete;		

	void bind(VkCommandBuffer commandBuffer){
		VkBuffer buffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}


	void draw(VkCommandBuffer commandBuffer){
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}


private:

	void creatVertexBuffers(const std::vector<Vertex> &vertices){
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex cout must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		engineDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory);

		void *data;
		vkMapMemory(engineDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(engineDevice.device(), vertexBufferMemory);
	}

	EngineDevice& engineDevice;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	uint32_t vertexCount; 
};	
} // namespace



#endif