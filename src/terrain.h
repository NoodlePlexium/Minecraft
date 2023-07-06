#ifndef TERRAIN_H
#define TERRAIN_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../vendor/glm/glm.hpp"
#include "../src/engine_mesh.h"
#include "../src/engine_device.h"
#include "../src/engine_buffer.h"
#include "../src/Vector.h"
#include "tables.h"
#include "FastNoiseLite.h"
#include "../src/compute_pipeline.h"
#include <vector>
#include <cstdlib>
#include <cmath>
#include <unordered_map>
#include <functional>


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

	struct Corner { 
		float x = 0;
		float y = 0;
		float z = 0;
		float activation = 0.0f;

		bool operator==(const Corner &other) const
		{ 
			return (x == other.x && y == other.y && z == other.z);
		}
	};
	struct CornerHasher {
        std::size_t operator()(const Corner& p) const {
            using std::size_t;
            using std::hash;

            return ((hash<float>()(p.x)
                     ^ (hash<float>()(p.y) << 1)) >> 1)
                     ^ (hash<float>()(p.z) << 1);
        }
    };
	struct Cube {
		int x;
		int y;
		int z;
		std::vector<Corner> corners{};

		// Initialize corner positions
		void InitCorners(){

			for (int i=0; i<8; ++i){
				Corner newCorner{};
				corners.push_back(newCorner);
			}

			corners[0].x = x - 0.5f;
			corners[0].y = y - 0.5f;
			corners[0].z = z + 0.5f;
			corners[1].x = x + 0.5f;
			corners[1].y = y - 0.5f;
			corners[1].z = z + 0.5f;
			corners[2].x = x + 0.5f;
			corners[2].y = y - 0.5f;
			corners[2].z = z - 0.5f;
			corners[3].x = x - 0.5f;
			corners[3].y = y - 0.5f;
			corners[3].z = z - 0.5f;
			corners[4].x = x - 0.5f;
			corners[4].y = y + 0.5f;
			corners[4].z = z + 0.5f;
			corners[5].x = x + 0.5f;
			corners[5].y = y + 0.5f;
			corners[5].z = z + 0.5f;
			corners[6].x = x + 0.5f;
			corners[6].y = y + 0.5f;
			corners[6].z = z - 0.5f;
			corners[7].x = x - 0.5f;
			corners[7].y = y + 0.5f;
			corners[7].z = z - 0.5f;
		}

		int getCubeIndex(float isolevel){
			int cubeIndex = 0;
			for (int i=0; i<8; ++i){	
			if (corners[i].activation > isolevel) cubeIndex |= (1 << i);}
			return cubeIndex;
		}
	};
	struct Chunk {
		std::vector<Cube> cubes;
		int x;
		int z;
	};

	// Pass to GPU
	struct CubeGPU {
		std::vector<glm::vec3> cornerPositions;
		std::vector<float> Corner3DNoise;
		std::vector<float> Corner2DNoise;
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
	void Init() {
		InitNoiseGenerator();


	}

	// Member variables
	TerrainSettings settings;
	FastNoiseLite noiseGenerator3D;
	FastNoiseLite noiseGenerator2D;
	std::vector<Chunk> chunks;

	// VULKAN
    std::unique_ptr<ComputePipeline> computePipeline;
    VkPipelineLayout pipelineLayout;
    std::unique_ptr<EngineBuffer> cubeBuffer;
	
// TERRAIN GENERATION //////////////////////////////////////////////////////////////
	void GenerateChunk(int posX, int posZ, EngineDevice& engineDevice) {
	    Chunk chunk;
	    chunk.x = posX;
	    chunk.z = posZ;

		int offset = (settings.chunkSize-1)/2;

	    for (int x = 0; x < settings.chunkSize; ++x) {
	        for (int y = 0; y < settings.worldHeight; ++y) {
	            for (int z = 0; z < settings.chunkSize; ++z) {
	                Cube cube{};
	                cube.x = x - offset + chunk.x;
	                cube.y = y;
	                cube.z = z - offset + chunk.z;
	                cube.InitCorners();

	                // calc corner activations
	                for (int i = 0; i < cube.corners.size(); ++i) {
	                    // Surface Noise
	                    float height = cube.corners[i].y;
	                    float surfaceHeight = settings.surfaceHeight + GetNoise2D(cube.corners[i].x, cube.corners[i].z) * settings.surfaceNoiseStrength;
	                    bool aboveSurface = height > surfaceHeight;
	                    float distAboveSurface = height - surfaceHeight;

	                    // Cave holes in surface
	                    float caveValue = GetNoise3D(cube.corners[i].x, cube.corners[i].y, cube.corners[i].z) + (height / surfaceHeight) * 0.1f;
	                    bool isSurfaceHole = caveValue < settings.isoLevel && distAboveSurface < 1.0f && aboveSurface;

						if (height >= settings.worldHeight){
							cube.corners[i].activation = 1;
						}
	                    else if (aboveSurface && !isSurfaceHole) {
	                        cube.corners[i].activation = 1 - distAboveSurface;
	                    } else {
	                        cube.corners[i].activation = caveValue;
	                    }
	                }

	                // Append cube to chunk
	                chunk.cubes.push_back(cube);
	            }
	        }
	    }
		// Add chunk struct
	    chunks.push_back(chunk);

		// Add chunkObject
		EngineGameObject terrainObject = EngineGameObject::createGameObject();
	    terrainObject.mesh = MeshFromChunk(chunk, engineDevice);
	    chunkObjects.push_back(std::move(terrainObject));
	}

	std::shared_ptr<Mesh> MeshFromChunk(const Chunk& chunk, EngineDevice& engineDevice){

		Mesh::Builder meshBuilder{};

		// Hashmap to check for duplicate vertices
		std::unordered_map<Corner, uint32_t, CornerHasher> vertexMap;

		// For each cube in chunk
		for(int j=0; j<chunk.cubes.size(); ++j){

			Cube cube = chunk.cubes[j];
			int cubeIndex = cube.getCubeIndex(settings.isoLevel);

			// Append vertex buffer vertices
			const int* TriTableRow = TriTable[cubeIndex];
			int i = 0;
			while(TriTableRow[i] != -1){
				int a0 = cornerIndexAFromEdge[TriTableRow[i]];
				int b0 = cornerIndexBFromEdge[TriTableRow[i]];
				int a1 = cornerIndexAFromEdge[TriTableRow[i+1]];
				int b1 = cornerIndexBFromEdge[TriTableRow[i+1]];
				int a2 = cornerIndexAFromEdge[TriTableRow[i+2]];
				int b2 = cornerIndexBFromEdge[TriTableRow[i+2]];

				Corner p1 = VertexInterp(settings.isoLevel,cube.corners[a0], cube.corners[b0]);
				Corner p2 = VertexInterp(settings.isoLevel,cube.corners[a1], cube.corners[b1]);
				Corner p3 = VertexInterp(settings.isoLevel,cube.corners[a2], cube.corners[b2]);

				// Calculate the normal
				Vector3 normal = CalculateNormal(p1, p2, p3);

				// Check for vertex duplicates. Vertex does not exist, add it to the hashmap and vector
				auto it1 = vertexMap.find(p1);
				if (it1 != vertexMap.end()) {
				    meshBuilder.indices.push_back(it1->second);
				} else {
				   vertexMap[p1] = meshBuilder.vertices.size();
				   meshBuilder.indices.push_back(meshBuilder.vertices.size());
				   meshBuilder.vertices.push_back(ConstructVertex(p1, normal));
				}

				auto it2 = vertexMap.find(p2);
				if (it2 != vertexMap.end()) {
				    meshBuilder.indices.push_back(it2->second);
				} else {
				   vertexMap[p2] = meshBuilder.vertices.size();
				   meshBuilder.indices.push_back(meshBuilder.vertices.size());
				   meshBuilder.vertices.push_back(ConstructVertex(p2, normal));
				}

				auto it3 = vertexMap.find(p3);
				if (it3 != vertexMap.end()) {
				    meshBuilder.indices.push_back(it3->second);
				} else {
				   vertexMap[p3] = meshBuilder.vertices.size();
				   meshBuilder.indices.push_back(meshBuilder.vertices.size());
				   meshBuilder.vertices.push_back(ConstructVertex(p3, normal));
				}
				i+=3;
			}
		}
		// Return mesh
		return std::make_unique<Mesh>(engineDevice, meshBuilder);
	}

	Mesh::Vertex ConstructVertex(Corner p, Vector3 normal){
		Vector2 uv = CalculateUV(p);
		Vector3 c = CalculateColour(p.y, normal);
		return {{p.x, p.y, p.z}, {c.x, c.y, c.z}, {normal.x, normal.y, normal.z}, {uv.x, uv.y}};
	}

	Corner VertexInterp(float isolevel, const Corner& p1, const Corner& p2) {
		float t = (isolevel - p1.activation) / (p2.activation - p1.activation);
		Corner p;
		p.x = p1.x + t * (p2.x - p1.x); // Smooth
		p.y = p1.y + t * (p2.y - p1.y);
		p.z = p1.z + t * (p2.z - p1.z);
		// p.x = (p1.x + p2.x)/2.0f; // Blocky
		// p.y = (p1.y + p2.y)/2.0f;
		// p.z = (p1.z + p2.z)/2.0f;
		return p;
	}

	Vector3 CalculateNormal(const Corner& p1, const Corner& p2, const Corner& p3){
		Vector3 v1(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
	    Vector3 v2(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);

	    // Calculate the cross product
	    Vector3 normal = v1.crossProduct(v2);

	    // return the normalized vector
	    return normal.normalized();
	}

	Vector2 CalculateUV(const Corner& Corner) {
    	float u = Corner.x; // calculate the u coordinate based on the Corner's position
    	float v = Corner.z; // calculate the v coordinate based on the Corner's position
    	return Vector2(u, v);
	}

	Vector3 CalculateColour(float y, Vector3 normal){

    	// Calculate the steepness of the vertice
    	float angle = std::acos(normal.y * -1.0f) * (180.0 / 3.141592654f);

    	bool isSnowHeight = (y/settings.worldHeight) > settings.snowMinHeightPercent;


    	// Snow 
    	if (isSnowHeight && angle < settings.snowMaxAngle)
    	{
    		return settings.snowColour;
    	}
    	else if (isSnowHeight && angle > settings.snowMaxAngle || !isSnowHeight && angle > settings.grassMaxAngle)
    	{
    		return settings.stoneColour;
    	}
    	else
    	{
			return settings.grassColour;
    	}
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

		cubeBuffer = std::make_unique<EngineBuffer>(
			engineDevice,
			cubeSize,
			cubeCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		// Copy staging buffer to vertex buffer
		engineDevice.copyBuffer(stagingBuffer.getBuffer(), cubeBuffer->getBuffer(), bufferSize);
	}
};
} // namespace
#endif
