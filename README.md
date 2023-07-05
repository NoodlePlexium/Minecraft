# Terrain Engine

A 3g graphics terrain engine based on marching cubes. This project uses Vulkan for 3D graphics and GLFW for windowing.
This project was started using the vulkan-cpp-starter repo: https://github.com/CapsCollective/vulkan-cpp-starter

### Licence

This project is licenced under an unmodified zlib/libpng licence, which is an OSI-certified, BSD-like licence that allows static linking with closed source software. Check [`LICENCE`](LICENSE) for further details.

### Early Terrain 
This demo uses a chunk based terrain system. Each chunk is treated as a separate mesh. Chunks are loaded and removed at runtime, one chunk per frame max, spreading the computational load across multiple frames.
![alt text](https://github.com/NoodlePlexium/Minecraft/blob/main/Screenshot1.png)

### Cave Generation 
![alt text](https://github.com/NoodlePlexium/Minecraft/blob/main/Screenshot2.png)
