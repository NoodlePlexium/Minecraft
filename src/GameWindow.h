#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>

class GameWindow{
public:
	GameWindow(int w, int h, std::string name)
	: width(w), height(h), windowName(name) {Init();}

	~GameWindow(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Run(){
		while(!glfwWindowShouldClose(window)){
			glfwPollEvents();
		}
	}

	void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
			throw std::runtime_error("failed to create a window surface");
		}
	}


private:

	void Init(){
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}


	const int width;
	const int height;
	std::string windowName;
	GLFWwindow *window;
};

#endif
