#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include "engine_game_object.h"
#include "GameWindow.h"
#include "camera.h"
#include <limits>
#include <unordered_map>

namespace Engine {

enum class MouseMode { Play, Normal, Hidden };

class InputSystem {
public:
    enum class KeyCode {
        Unknown = -1,
        Apostrophe = GLFW_KEY_APOSTROPHE,
        Semicolon = GLFW_KEY_SEMICOLON,
        Comma = GLFW_KEY_COMMA,
        Period = GLFW_KEY_PERIOD,
        Num0 = GLFW_KEY_0,
        Num1 = GLFW_KEY_1,
        Num2 = GLFW_KEY_2,
        Num3 = GLFW_KEY_3,
        Num4 = GLFW_KEY_4,
        Num5 = GLFW_KEY_5,
        Num6 = GLFW_KEY_6,
        Num7 = GLFW_KEY_7,
        Num8 = GLFW_KEY_8,
        Num9 = GLFW_KEY_9,
        Plus = GLFW_KEY_KP_ADD,
        Minus = GLFW_KEY_MINUS,
        Equal = GLFW_KEY_EQUAL,
        Slash = GLFW_KEY_SLASH,
        A = GLFW_KEY_A,
        B = GLFW_KEY_B,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        E = GLFW_KEY_E,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        I = GLFW_KEY_I,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        M = GLFW_KEY_M,
        N = GLFW_KEY_N,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,
        Q = GLFW_KEY_Q,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        U = GLFW_KEY_U,
        V = GLFW_KEY_V,
        W = GLFW_KEY_W,
        X = GLFW_KEY_X,
        Y = GLFW_KEY_Y,
        Z = GLFW_KEY_Z,
        Space = GLFW_KEY_SPACE,
        LeftShift = GLFW_KEY_LEFT_SHIFT,
        RightShift = GLFW_KEY_RIGHT_SHIFT,
        LeftControl = GLFW_KEY_LEFT_CONTROL,
        RightControl = GLFW_KEY_RIGHT_CONTROL,
        Alt = GLFW_KEY_LEFT_ALT,
        Tab = GLFW_KEY_TAB,
        Escape = GLFW_KEY_ESCAPE,
        Left = GLFW_KEY_LEFT,
        Right = GLFW_KEY_RIGHT,
        Up = GLFW_KEY_UP,
        Down = GLFW_KEY_DOWN
    };

    struct KeyMappings {
        KeyCode moveLeft = KeyCode::A;
        KeyCode moveRight = KeyCode::D;
        KeyCode moveForward = KeyCode::W;
        KeyCode moveBackward = KeyCode::S;
        KeyCode moveUp = KeyCode::Space;
        KeyCode moveDown = KeyCode::LeftShift;
        KeyCode lookUp = KeyCode::Up;
        KeyCode lookDown = KeyCode::Down;
        KeyCode escapeDown = KeyCode::Escape;
    };

    InputSystem(GLFWwindow* _window) : window(_window) {
        InitializeKeyMapper();
    }

    void UpdateInputs() {
        UpdateMouse();
        UpdateKeyboard();
    }

    glm::vec2 Movement() {
        double x = GetKeyState(keys.moveRight) - GetKeyState(keys.moveLeft);
        double y = GetKeyState(keys.moveForward) - GetKeyState(keys.moveBackward);
        return glm::vec2(x, y);
    }

    float MovementY() {
        return GetKeyState(keys.moveUp) - GetKeyState(keys.moveDown);
    }

    glm::vec2 MouseLook() {
        if (mouseMode == MouseMode::Normal)
            return glm::vec2(0.0f, 0.0f);
        else
            return glm::vec2(mouseDeltaX, mouseDeltaY);
    }

    glm::vec2 MousePosition() {
        return glm::vec2(mouseX, mouseY);
    }

    double MouseX() { return mouseX; }
    double MouseY() { return mouseY; }

    void SetMouseMode(MouseMode mode) {
        mouseMode = mode;
        int cursorMode;

        if (mode == MouseMode::Play)
            cursorMode = GLFW_CURSOR_DISABLED;
        if (mode == MouseMode::Hidden)
            cursorMode = GLFW_CURSOR_HIDDEN;
        if (mode == MouseMode::Normal)
            cursorMode = GLFW_CURSOR_NORMAL;

        glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
    }

    MouseMode GetMouseMode() {
        return mouseMode;
    }

    bool mouse1Pressed = false;
    bool mouse1Down = false;
    bool mouse2Pressed = false;
    bool mouse2Down = false;

    bool GetKeyDown(KeyCode key) {
        int glfwKey = GetGLFWKeyCode(key);

        auto currentFrameState = keyStateMap[glfwKey];
        auto previousFrameState = lastFrameKeyStateMap[glfwKey];

        // Key Down - if key is pressed in the current frame and was not pressed in the previous frame
        if (currentFrameState && !previousFrameState) {
            return true;
        }

        return false;
    }


    bool GetKey(KeyCode key) {
        int glfwKey = GetGLFWKeyCode(key);
        return glfwGetKey(window, glfwKey) == GLFW_PRESS;
    }

private:
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
        mouseDeltaX = xpos - prevMouseX;
        mouseDeltaY = ypos - prevMouseY;

        if (mouseDeltaY < 2 && mouseDeltaY > -2)
            mouseDeltaY = 0;
        if (mouseDeltaX < 2 && mouseDeltaX > -2)
            mouseDeltaX = 0;

        mouseX = xpos;
        mouseY = ypos;

        prevMouseX = xpos;
        prevMouseY = ypos;
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

        if (inputSystem == nullptr) {
            return;
        }

        if (action == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_1) {
                inputSystem->mouse1Pressed = true;
            } else if (button == GLFW_MOUSE_BUTTON_2) {
                inputSystem->mouse2Pressed = true;
            }
        } else if (action == GLFW_RELEASE) {
            if (button == GLFW_MOUSE_BUTTON_1) {
                inputSystem->mouse1Pressed = false;
            } else if (button == GLFW_MOUSE_BUTTON_2) {
                inputSystem->mouse2Pressed = false;
            }
        }
    }

    void UpdateMouse() {
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        mouse1Down = mouse1Pressed;
        mouse2Down = mouse2Pressed;
    }

    void UpdateKeyboard() {
        lastFrameKeyStateMap = keyStateMap;

        for (auto& keyState : keyStateMap) {
            int key = keyState.first;
            keyState.second = glfwGetKey(window, key) == GLFW_PRESS;
        }
    }

    double GetKeyState(KeyCode key) {
        int glfwKey = GetGLFWKeyCode(key);
        return glfwGetKey(window, glfwKey) == GLFW_PRESS ? 1.0 : 0.0;
    }

    int GetGLFWKeyCode(KeyCode key) {
        auto it = keyMap.find(key);
        if (it != keyMap.end()) {
            return it->second;
        }
        return GLFW_KEY_UNKNOWN;
    }

    void InitializeKeyMapper() {
        keyMap.clear();
        keyMap[KeyCode::Unknown] = GLFW_KEY_UNKNOWN;
        keyMap[KeyCode::Apostrophe] = GLFW_KEY_APOSTROPHE;
        keyMap[KeyCode::Semicolon] = GLFW_KEY_SEMICOLON;
        keyMap[KeyCode::Comma] = GLFW_KEY_COMMA;
        keyMap[KeyCode::Period] = GLFW_KEY_PERIOD;
        keyMap[KeyCode::Num0] = GLFW_KEY_0;
        keyMap[KeyCode::Num1] = GLFW_KEY_1;
        keyMap[KeyCode::Num2] = GLFW_KEY_2;
        keyMap[KeyCode::Num3] = GLFW_KEY_3;
        keyMap[KeyCode::Num4] = GLFW_KEY_4;
        keyMap[KeyCode::Num5] = GLFW_KEY_5;
        keyMap[KeyCode::Num6] = GLFW_KEY_6;
        keyMap[KeyCode::Num7] = GLFW_KEY_7;
        keyMap[KeyCode::Num8] = GLFW_KEY_8;
        keyMap[KeyCode::Num9] = GLFW_KEY_9;
        keyMap[KeyCode::Plus] = GLFW_KEY_KP_ADD;
        keyMap[KeyCode::Minus] = GLFW_KEY_MINUS;
        keyMap[KeyCode::Equal] = GLFW_KEY_EQUAL;
        keyMap[KeyCode::Slash] = GLFW_KEY_SLASH;
        keyMap[KeyCode::A] = GLFW_KEY_A;
        keyMap[KeyCode::B] = GLFW_KEY_B;
        keyMap[KeyCode::C] = GLFW_KEY_C;
        keyMap[KeyCode::D] = GLFW_KEY_D;
        keyMap[KeyCode::E] = GLFW_KEY_E;
        keyMap[KeyCode::F] = GLFW_KEY_F;
        keyMap[KeyCode::G] = GLFW_KEY_G;
        keyMap[KeyCode::H] = GLFW_KEY_H;
        keyMap[KeyCode::I] = GLFW_KEY_I;
        keyMap[KeyCode::J] = GLFW_KEY_J;
        keyMap[KeyCode::K] = GLFW_KEY_K;
        keyMap[KeyCode::L] = GLFW_KEY_L;
        keyMap[KeyCode::M] = GLFW_KEY_M;
        keyMap[KeyCode::N] = GLFW_KEY_N;
        keyMap[KeyCode::O] = GLFW_KEY_O;
        keyMap[KeyCode::P] = GLFW_KEY_P;
        keyMap[KeyCode::Q] = GLFW_KEY_Q;
        keyMap[KeyCode::R] = GLFW_KEY_R;
        keyMap[KeyCode::S] = GLFW_KEY_S;
        keyMap[KeyCode::T] = GLFW_KEY_T;
        keyMap[KeyCode::U] = GLFW_KEY_U;
        keyMap[KeyCode::V] = GLFW_KEY_V;
        keyMap[KeyCode::W] = GLFW_KEY_W;
        keyMap[KeyCode::X] = GLFW_KEY_X;
        keyMap[KeyCode::Y] = GLFW_KEY_Y;
        keyMap[KeyCode::Z] = GLFW_KEY_Z;
        keyMap[KeyCode::Space] = GLFW_KEY_SPACE;
        keyMap[KeyCode::LeftShift] = GLFW_KEY_LEFT_SHIFT;
        keyMap[KeyCode::RightShift] = GLFW_KEY_RIGHT_SHIFT;
        keyMap[KeyCode::LeftControl] = GLFW_KEY_LEFT_CONTROL;
        keyMap[KeyCode::RightControl] = GLFW_KEY_RIGHT_CONTROL;
        keyMap[KeyCode::Alt] = GLFW_KEY_LEFT_ALT;
        keyMap[KeyCode::Tab] = GLFW_KEY_TAB;
        keyMap[KeyCode::Escape] = GLFW_KEY_ESCAPE;
        keyMap[KeyCode::Left] = GLFW_KEY_LEFT;
        keyMap[KeyCode::Right] = GLFW_KEY_RIGHT;
        keyMap[KeyCode::Up] = GLFW_KEY_UP;
        keyMap[KeyCode::Down] = GLFW_KEY_DOWN;
    }



    GLFWwindow* window;
    KeyMappings keys{};

    static double prevMouseX;
    static double prevMouseY;
    static double mouseX;
    static double mouseY;
    static double mouseDeltaX;
    static double mouseDeltaY;

    MouseMode mouseMode = MouseMode::Normal;

    std::unordered_map<int, bool> keyStateMap;
    std::unordered_map<int, bool> lastFrameKeyStateMap;
    std::unordered_map<KeyCode, int> keyMap;
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

