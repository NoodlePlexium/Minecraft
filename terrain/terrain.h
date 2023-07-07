#ifndef TERRAIN_H
#define TERRAIN_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../vendor/glm/glm.hpp"
#include "../src/engine_mesh.h"
#include "../src/engine_device.h"
#include "../src/engine_buffer.h"
#include "../src/Vector.h"
#include "../src/compute_pipeline.h"
#include "FastNoiseLite.h"
#include <vector>

namespace Engine{
class Terrain{
public:

	struct TerrainSettings{
		float isoLevel = 0.34f;

		// World Settings
		int worldHeight = 30;	
		int surfaceHeight = 5;
		int chunkSize = 10;


		// Noise Settings
		int seed = 31584;
		int octaves = 3;
		float prominance = 0.5f;
		float caveNoiseScale = 0.2f;
		float surfaceNoiseScale = 1.0f;
		float surfaceNoiseStrength = 25.0f;

		// Material Settings
	    float snowMinHeightPercent = 0.9f;
	    float snowMaxAngle = 55.0f;
	    float grassMaxAngle = 45.0f;
	    Vector3 snowColour;
	    Vector3 grassColour;
	    Vector3 stoneColour;

	    TerrainSettings() {
	        snowColour = Vector3(1.0f, 1.0f, 1.0f);
	        grassColour = Vector3(0.45f, 0.62f, 0.2f);
	        stoneColour = Vector3(0.3f, 0.3f, 0.3f);
	    }
	};

	
	// Pass to GPU
	struct CubeGPU {
		glm::vec3 position;
		std::vector<float> Corner3DNoise;
		std::vector<float> Corner2DNoise;
	};

	struct Chunk {
		int x;
		int y;
		int z;
	};


	Terrain(TerrainSettings _settings = TerrainSettings{}) : settings(_settings) {Init();}


	// Public member variables
	std::vector<EngineGameObject> chunkObjects;

	void UpdateChunks(int renderDistance, float playerX, float playerZ, EngineDevice& engineDevice) {

	    // Chunk coordinates that bound the player
	    playerX = static_cast<int>(std::round(playerX));
	    playerZ = static_cast<int>(std::round(playerZ));

	    int CenterChunkX = static_cast<int>(std::floor(playerX / settings.chunkSize) * settings.chunkSize);
	    int CenterChunkZ = static_cast<int>(std::floor(playerZ / settings.chunkSize) * settings.chunkSize);
	    int offset = (renderDistance - 1) * settings.chunkSize / 2;

		// GENERATE 1 CHUNK PER FRAME
		bool generatedChunkThisFarme = false;

		// Chunk index marked for removal
		int markedChunkIndex = -1;

	    for (int x = 0; x < renderDistance; ++x) {
	        for (int z = 0; z < renderDistance; ++z) {
	            int worldX = x * settings.chunkSize - offset + CenterChunkX;
	            int worldZ = z * settings.chunkSize - offset + CenterChunkZ;

				// Stop if chunk was generated this frame
				if (generatedChunkThisFarme) break;

	            bool chunkPresent = false;

	            // Check if there is a chunk at the position
	            for (int i = 0; i < chunks.size(); ++i) {

	            	// 1 - check if chunk exists at current iteration position
	                if (chunks[i].x == worldX && chunks[i].z == worldZ) {
	                    chunkPresent = true;
						break;
	                }


					// 2 - mark chunks for removal
	            	int chunkDist = std::max(std::abs(chunks[i].x - CenterChunkX), std::abs(chunks[i].z - CenterChunkZ));
	            	if (chunkDist > std::floor((renderDistance * settings.chunkSize) / 2.0)){
						markedChunkIndex = i;
	            	} 
	            }

	            // No chunk present? Create a new one
	            if (!chunkPresent){
	            	GenerateChunk(worldX, worldZ, engineDevice);
					generatedChunkThisFarme = true;
	            }
	        }
	    }

		// Remove 1 Chunk per frame
		if (markedChunkIndex != -1){
			chunks.erase(chunks.begin() + markedChunkIndex);
			chunkObjects.erase(chunkObjects.begin() + markedChunkIndex);
		}
	}

private:
	void Init() {InitNoiseGenerator();}

	// Member variables
	TerrainSettings settings;
	FastNoiseLite noiseGenerator3D;
	FastNoiseLite noiseGenerator2D;
	std::vector<Chunk> chunks;

	// VULKAN
    std::unique_ptr<ComputePipeline> computePipeline;
    VkPipelineLayout pipelineLayout;
    std::unique_ptr<EngineBuffer> chunkBuffer;
	
// TERRAIN GENERATION //////////////////////////////////////////////////////////////
	void GenerateChunk(int posX, int posZ, EngineDevice& engineDevice) {

	    int offset = (settings.chunkSize-1)/2;

	    std::vector<CubeGPU> gpuCubes;

	    for (int x = 0; x < settings.chunkSize; ++x) {
	        for (int y = 0; y < settings.worldHeight; ++y) {
	            for (int z = 0; z < settings.chunkSize; ++z) {

	                int cubeX = x - offset + posX;
	                int cubeY = y;
	                int cubeZ = z - offset + posZ;

	                CubeGPU cubeGPU;
	                cubeGPU.position = glm::vec3(cubeX, cubeY, cubeZ); // cube position

	                std::vector<glm::vec3> cornerPositions;
	                cornerPositions.push_back(glm::vec3{cubeX - 0.5f, cubeY - 0.5f, cubeZ + 0.5f});
	                cornerPositions.push_back(glm::vec3{cubeX + 0.5f, cubeY - 0.5f, cubeZ + 0.5f});
	                cornerPositions.push_back(glm::vec3{cubeX + 0.5f, cubeY - 0.5f, cubeZ - 0.5f});
	                cornerPositions.push_back(glm::vec3{cubeX - 0.5f, cubeY - 0.5f, cubeZ - 0.5f});
	                cornerPositions.push_back(glm::vec3{cubeX - 0.5f, cubeY + 0.5f, cubeZ + 0.5f});
	                cornerPositions.push_back(glm::vec3{cubeX + 0.5f, cubeY + 0.5f, cubeZ + 0.5f});
	                cornerPositions.push_back(glm::vec3{cubeX + 0.5f, cubeY + 0.5f, cubeZ - 0.5f});
	                cornerPositions.push_back(glm::vec3{cubeX - 0.5f, cubeY + 0.5f, cubeZ - 0.5f});

	                // Corner noise
	                for (int i=0; i<8; ++i){
	                    cubeGPU.Corner3DNoise.push_back(GetNoise3D(cornerPositions[i].x, cornerPositions[i].y, cornerPositions[i].z));
	                    cubeGPU.Corner2DNoise.push_back(GetNoise2D(cornerPositions[i].x, cornerPositions[i].z));
	                }

	                // add to gpu cubes
	                gpuCubes.push_back(cubeGPU);
	            }
	        }
	    }

	    // create buffer
	    createCubesBuffer(gpuCubes, engineDevice);

	    // // Create a command buffer
	    // VkCommandBufferAllocateInfo allocateInfo{};
	    // allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	    // allocateInfo.commandPool = engineDevice.getCommandPool();
	    // allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	    // allocateInfo.commandBufferCount = 1;
	    // VkCommandBuffer computeCommandBuffer;
	    // vkAllocateCommandBuffers(engineDevice.device(), &allocateInfo, &computeCommandBuffer);
	    // VkCommandBufferBeginInfo beginInfo{};
	    // beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	    // beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	    // vkBeginCommandBuffer(computeCommandBuffer, &beginInfo);

	    // // Bind to compute command buffer
	    // vkCmdBindBuffer(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, chunkBuffer, 0);

	    // // End recording the compute command buffer
	    // vkEndCommandBuffer(computeCommandBuffer);

	    // // Submit the compute command buffer
	    // VkSubmitInfo submitInfo{};
	    // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	    // submitInfo.commandBufferCount = 1;
	    // submitInfo.pCommandBuffers = &computeCommandBuffer;
	    // vkQueueSubmit(engineDevice.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

	    // // Wait for the completion of the compute operation
	    // vkQueueWaitIdle(engineDevice.graphicsQueue());
	}
// TERRAIN GENERATION //////////////////////////////////////////////////////////////

// NOISE GENERATION ////////////////////////////////////////////////////////////////
	float GetNoise3D(float x, float y, float z){
		float totalNoise = 0.0f;
    	float frequency = 1.0f;
    	float amplitude = 1.0f;

		for (int i = 0; i < settings.octaves; i++) {
        	totalNoise += noiseGenerator3D.GetNoise(x*frequency/settings.caveNoiseScale, y*frequency/settings.caveNoiseScale, z*frequency/settings.caveNoiseScale) * amplitude;
        	frequency *= 2.0f;  // Increase the frequency for each octave
        	amplitude *= settings.prominance;  // Decrease the amplitude for each octave
    	}
		return (totalNoise + 1)/2.0f;
	}

	float GetNoise2D(float x, float y) {
		float totalNoise = 0.0f;
		float frequency = 1.0f;
		float amplitude = 1.0f;

		for (int i = 0; i < settings.octaves; i++) {
	    	totalNoise += noiseGenerator3D.GetNoise(x*frequency/settings.surfaceNoiseScale, y*frequency/settings.surfaceNoiseScale) * amplitude;
	    	frequency *= 2.0f;  // Increase the frequency for each octave
	    	amplitude *= settings.prominance;  // Decrease the amplitude for each octave
		}
		return (totalNoise + 1)/2.0f;
	}

	void InitNoiseGenerator(){
      noiseGenerator3D.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
      noiseGenerator2D.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
      noiseGenerator3D.SetSeed(settings.seed);
      noiseGenerator2D.SetSeed(settings.seed);
	}
// NOISE GENERATION ////////////////////////////////////////////////////////////////

// COMPUTE SHADER //////////////////////////////////////////////////////////////////

	void createCubesBuffer(std::vector<CubeGPU> cubes, EngineDevice& engineDevice){
		uint32_t cubeCount = static_cast<uint32_t>(cubes.size());
		VkDeviceSize bufferSize = sizeof(cubes[0]) * cubeCount;
		uint32_t cubeSize = sizeof(cubes[0]);

		EngineBuffer stagingBuffer{
			engineDevice,
			cubeSize,
			cubeCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void *) cubes.data());

		chunkBuffer = std::make_unique<EngineBuffer>(
			engineDevice,
			cubeSize,
			cubeCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		// Copy staging buffer to vertex buffer
		engineDevice.copyBuffer(stagingBuffer.getBuffer(), chunkBuffer->getBuffer(), bufferSize);
	}
};
} // namespace
#endif
