#ifndef APP_H
#define APP_H

#include "GameWindow.h"
#include "pipeline.h"
#include "vulkan_device.h"

namespace Engine{

class App{
public:
	static constexpr int width = 800;
	static constexpr int height = 600;

	App() : 
		window(width, height, "Minecraft"), 
		engineDevice(window), 
		renderPipeline(engineDevice, 
		"shaders/shader.vert.spv", 
		"shaders/shader.frag.spv", 
		RenderPipeline::defaultPipelineConfigInfo(width, height)) {}

	void run(){
		window.Run();
	}


private:
	GameWindow window;
    EngineDevice engineDevice;
    RenderPipeline renderPipeline;
};



} // namespace


#endif