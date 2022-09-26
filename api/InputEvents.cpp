#include "InputEvents.hpp"
#include "api.hpp"

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

        int GetHorizontalAxis() {
            if (IsKeyPressed(GLFW_KEY_A)) {
                return -1;
            }
            else if (IsKeyPressed(GLFW_KEY_D)) {
                return 1;
            }
            return 0;
        }

        int GetVerticalAxis() {
            if (IsKeyPressed(GLFW_KEY_W)) {
                return 1;
            }
            else if (IsKeyPressed(GLFW_KEY_S)) {
                return -1;
            }
            return 0;
        }

        int GetMouseXAxis() {
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            if (x < winPos.x + winSize.x / 2) {
                return -1;
            }
            else if (x > winPos.x + winSize.x / 2) {
                return 1;
            }
        }

        int GetMouseYAxis() {
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            if (y < winPos.y + winSize.y / 2) {
                return -1;
            }
            else if (y > winPos.y + winSize.y / 2) {
                return 1;
            }
        }

        bool SetMouseHidden(bool hidden) {
            if (hidden) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                return true;
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                return false;
            }
        }
        
        int SetMousePosition(float x, float y) {
            glfwSetCursorPos(window, x, y);
            return 0;
        }

    }
}