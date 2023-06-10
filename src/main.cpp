#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "window.h"

int main() 
{

    Window window(800, 600, "Minecraft");
    window.Run();

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";
}

// cd ..
// mingw32-make setup
// mingw32-make