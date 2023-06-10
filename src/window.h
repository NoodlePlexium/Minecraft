#ifndef WINDOW_H
#define WINDOW_H
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

class Window{
public:
	Window(int w, int h, std::string name)
	: width(w), height(h), windowName(name) {Init();}

	~Window(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Run(){
		while(!glfwWindowShouldClose(window)){
			glfwPollEvents();
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