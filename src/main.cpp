#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "App.h"

int main() 
{


    Engine::App app{};

    try{
        app.run();
    }
    catch (const std::exception &e){
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

// cd ..
// mingw32-make setup
// mingw32-make
