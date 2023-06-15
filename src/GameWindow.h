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

	GameWindow(const GameWindow &) = delete;
	GameWindow &operator=(const GameWindow &) = delete;

	bool shouldClose() {return glfwWindowShouldClose(window);}

	void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
			throw std::runtime_error("failed to create a window surface");
		}
	}

	VkExtent2D getExtent() {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}

	bool wasWindowResized() {return framebufferResized;}
	void resetWindowResizedFlag() {framebufferResized = false;}

	GLFWwindow *getGLFWwindow() const {return window;}

private:

	static void framebufferResizeCallBack(GLFWwindow *window, int width, int height){
		auto gameWindow = reinterpret_cast<GameWindow *>(glfwGetWindowUserPointer(window));
		gameWindow->framebufferResized = true;
		gameWindow->width = width;
		gameWindow->height = height;
	}

	void Init(){
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallBack);
	}


	int width;
	int height;
	bool framebufferResized = false;

	std::string windowName;
	GLFWwindow *window;
};

#endif
