#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "GameWindow.h"
//#include "camera.h"
//#include "Vector.h"
//#include "renderer.h"
#include "pipeline.h"

int main() 
{

    int width = 800;
    int height = 600;

    GameWindow window(width, height, "Minecraft");
    window.Run();

    // Render Pipeline
    RenderPipeline renderPipeline("shaders/shader.vert.spv", "shaders/shader.frag.spv");

    //uint32_t extensionCount = 0;
    //vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);


    // Set up camera

    //Vector camPos(0, 0, 0);
    //Vector canRot(0, 0, 0);
    //Camera camera(camPos, camRot, width, height);
    return 0;
}

// cd ..
// mingw32-make setup
// mingw32-make
