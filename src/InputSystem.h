#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include "engine_game_object.h"
#include "GameWindow.h"
#include "camera.h"
#include <limits>

namespace Engine {

enum MouseMode {play, normal, hidden}; 

class InputSystem {
public:
    InputSystem(GLFWwindow* _window) : window(_window) {}

    void UpdateInputs(){

        // Update mouse
        glfwSetCursorPosCallback(window, mouseCallback);

        if (glfwGetKey(window, keys.escapeDown) == GLFW_PRESS && !escDown) {
            escDown = true;
            if (getMouseMode() == MouseMode::play){
                setMouseModeNormal();
            }
            else{
                setMouseModePlay();
            }
        }
        if (glfwGetKey(window, keys.escapeDown) == GLFW_RELEASE) {escDown = false;}
    }

    struct KeyMappings {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_SPACE;
        int moveDown = GLFW_KEY_LEFT_SHIFT;
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
        int escapeDown = GLFW_KEY_ESCAPE;
    };

    glm::vec2 Movement() {
        double x = 0.0f;
        double y = 0.0f;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) x += 1;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) x -= 1;
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) y += 1;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) y -= 1;
        glm::vec2 moveInput{x, y};
        return moveInput;
    }

    float MovementY() {
        float y = 0.0f;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) y += 1;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) y -= 1;
        return y;
    }

    glm::vec2 MouseLook() {
        if (mouseMode == MouseMode::normal) return glm::vec2{0.0f, 0.0f};
        return glm::vec2{getMouseDeltaX(), getMouseDeltaY()};
    }

    glm::vec2 MousePosition() {
        return glm::vec2{mouseX, mouseY};
    }

    double MouseX() {return mouseX;}
    double MouseY() {return mouseY;}

    void setMouseModePlay(){
        mouseMode = MouseMode::play;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void setMouseModeHidden(){
        mouseMode = MouseMode::hidden;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void setMouseModeNormal(){
        mouseMode = MouseMode::normal;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    MouseMode getMouseMode(){
        return mouseMode;
    }

    MouseMode mouseMode = MouseMode::normal; 


    KeyMappings keys{};

private:
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        // Calculate mouse delta
        mouseDeltaX = xpos - prevMouseX;
		mouseDeltaY = ypos - prevMouseY;

        if (mouseDeltaY < 2 && mouseDeltaY > -2) mouseDeltaY = 0;
        if (mouseDeltaX < 2 && mouseDeltaX > -2) mouseDeltaX = 0;

        mouseX = xpos;
        mouseY = ypos;

        // Update previous mouse position
        prevMouseX = xpos;
        prevMouseY = ypos;
    }

    double getMouseDeltaX() {
        return mouseDeltaX;
    }

    double getMouseDeltaY() {
        return mouseDeltaY;
    }

    static double prevMouseX;
    static double prevMouseY;
    static double mouseX;
    static double mouseY;
    static double mouseDeltaX;
    static double mouseDeltaY;

    GLFWwindow* window;
    bool escDown = false;
};


// Define static member variables
double InputSystem::prevMouseX = 0.0;
double InputSystem::prevMouseY = 0.0;
double InputSystem::mouseX = 0.0;
double InputSystem::mouseY = 0.0;
double InputSystem::mouseDeltaX = 0.0;
double InputSystem::mouseDeltaY = 0.0;
} // namespace Engine

#endif
