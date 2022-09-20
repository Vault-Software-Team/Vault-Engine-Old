#include "InputEvents.hpp"

namespace HyperAPI {

    namespace Input {
        GLFWwindow *window = nullptr;
        
        bool IsKeyPressed(int key) {
            if (glfwGetKey(window, key) == GLFW_PRESS) {
                return true;
            }
            return false;
        }

        bool IsKeyReleased(int key) {
            if (glfwGetKey(window, key) == GLFW_RELEASE) {
                return true;
            }
            return false;
        }

        bool IsMouseButtonPressed(int button) {
            if (glfwGetMouseButton(window, button) == GLFW_PRESS) {
                return true;
            }
            return false;
        }

        bool IsMouseButtonReleased(int button) {
            if (glfwGetMouseButton(window, button) == GLFW_RELEASE) {
                return true;
            }
            return false;
        }
    }
}