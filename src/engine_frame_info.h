#ifndef ENGINE_FRAME_INFO_H
#define ENGINE_FRAME_INFO_H

#include "camera.h"
#include <vulkan/vulkan.h>

namespace Engine{


struct FrameInfo {
	int frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	Camera &camera;
	VkDescriptorSet globalDescriptorSet;
};


} // namespace	
#endif