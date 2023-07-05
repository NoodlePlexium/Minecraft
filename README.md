# Terrain Engine

A 3g graphics terrain engine based on marching cubes. This project uses Vulkan for 3D graphics and GLFW for windowing.
This project was started using the vulkan-cpp-starter repo: https://github.com/CapsCollective/vulkan-cpp-starter

### Licence

This project is licenced under an unmodified zlib/libpng licence, which is an OSI-certified, BSD-like licence that allows static linking with closed source software. Check [`LICENCE`](LICENSE) for further details.

### Early Terrain 
This demo uses a chunk based terrain system. Each chunk is treated as a separate mesh. 
Chunks are loaded and removed at runtime, one chunk per frame max, spreading the computational load across multiple frames.
The terrain uses a typical layered perlin noise and materials are set according to rules.
A combination of 2D and 3D noise is used, 2D noise as a heightmap for the surface and 3D noise for caves and eventually overhangs.

![alt text](https://github.com/NoodlePlexium/Minecraft/blob/main/Screenshot1.png)

### Cave Generation 
![alt text](https://github.com/NoodlePlexium/Minecraft/blob/main/Screenshot2.png)


### Todo's
Need to integrate the marching cubes compute shader into the source code to massively improve the mesh generation and increase the framerate.
Complete the ray cast algorithm and spacial partitioning system.
Terrain terraforming - add/remove density values from the marching cubes, the remesh the chunk.
Implement capsule-triangle collision, allowing the player to walk on the surface.
Implement textures into the rendering process, and redefine the material rules for the terrain.
Create a billboard class - used for rendering grass and worldspace UI elements.
Add Point lights.

### Experimentation and research
THis project will be the grounds of personal research into computer rendering, particularly concerning Realtime Global Illumination. 
I intend to create marching cubes terrain specific Global Illumination using radiosity. 
This project marks the second step in my lighting journey. My first experiment with lighting can be found here: https://github.com/NoodlePlexium/Raytracer 
