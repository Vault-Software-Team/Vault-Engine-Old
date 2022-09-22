#include "InputEvents.hpp"

namespace HyperAPI {

    namespace Input {
        GLFWwindow *window = nullptr;
        glm::vec3 winPos;
        glm::vec3 winSize;
        
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

        float GetMouseX() {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            return x;
        }

        float GetMouseY() {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            return y;
        }

        int GetMouseAxisX() {
            double mouseX;
            double mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            double rotX = (double)(mouseY - (winPos.y + (winSize.y / 2))) / winSize.y;
            return rotX;
        }

        int GetMouseAxisY() {
            double mouseX;
            double mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            double rotY = (double)(mouseX - (winPos.x + (winSize.x / 2))) / winSize.x;
            return rotY;
        }
    }
}