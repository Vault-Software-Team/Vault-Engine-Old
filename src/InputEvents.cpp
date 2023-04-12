#include "lib/InputEvents.hpp"
#include "lib/api.hpp"
#include "scene.hpp"

namespace HyperAPI {
    namespace Input {
        glm::vec3 mouseRay;
        void setMouseRay(const glm::vec2 &pos, Camera *camera, glm::vec3 &scale, glm::vec2 &renderer) {

            glm::mat4 proj = camera->projection;
            if (camera->mode2D) {
                proj = glm::perspective(glm::radians(camera->cam_fov), (renderer.x / renderer.y), camera->cam_near, camera->cam_far);
                proj = glm::scale(proj, glm::vec3(scale.x, scale.y, 1.0f));
            }

            glm::mat4 invMat = glm::inverse(proj * camera->view);
            glm::vec4 m_near((pos.x - (renderer.x / 2)) / (renderer.x / 2), -1 * (pos.y - (renderer.y / 2)) / (renderer.y / 2), -1, 1.0);
            glm::vec4 m_far((pos.x - (renderer.x / 2)) / (renderer.x / 2), -1 * (pos.y - (renderer.y / 2)) / (renderer.y / 2), 1, 1.0);
            glm::vec4 nearResult(0, 0, 0, 0);
            nearResult = invMat * m_near;
            glm::vec4 farResult(0, 0, 0, 0);
            farResult = invMat * m_far;
            nearResult /= nearResult.w;
            farResult /= farResult.w;
            glm::vec3 dir(farResult - nearResult);
            mouseRay = dir;
        }

        void set_ray(glm::vec2 &pos, glm::vec3 &scale, glm::vec2 &renderer) {
            setMouseRay(pos, Scene::mainCamera, scale, renderer);
        }

        GLFWwindow *window = nullptr;
        glm::vec3 winPos = glm::vec3(0.0f);
        glm::vec3 winSize = glm::vec3(0.0f);

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
            } else if (IsKeyPressed(GLFW_KEY_D)) {
                return 1;
            }
            return 0;
        }

        int GetVerticalAxis() {
            if (IsKeyPressed(GLFW_KEY_W)) {
                return 1;
            } else if (IsKeyPressed(GLFW_KEY_S)) {
                return -1;
            }
            return 0;
        }

        int GetMouseXAxis() {
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            if (x < winPos.x + winSize.x / 2) {
                return -1;
            } else if (x > winPos.x + winSize.x / 2) {
                return 1;
            }
        }

        int GetMouseYAxis() {
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            if (y < winPos.y + winSize.y / 2) {
                return -1;
            } else if (y > winPos.y + winSize.y / 2) {
                return 1;
            }
        }

        bool SetMouseHidden(bool hidden) {
            if (hidden) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                return true;
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                return false;
            }
        }

        int SetMousePosition(float x, float y) {
            glfwSetCursorPos(window, x, y);
            return 0;
        }

    } // namespace Input

    namespace Input::Controller {
        int currentController;

        void SetCurrentController(int controller) {
            currentController = controller;
        }

        float GetLeftAnalogX() {
            if (1 == glfwJoystickPresent(currentController)) {
                int count;
                const float *axes = glfwGetJoystickAxes(currentController, &count);
                return axes[0];
            }

            return 0;
        }

        float GetLeftAnalogY() {
            if (1 == glfwJoystickPresent(currentController)) {
                int count;
                const float *axes = glfwGetJoystickAxes(currentController, &count);
                return axes[1];
            }

            return 0;
        }
        float GetRightAnalogX() {
            if (1 == glfwJoystickPresent(currentController)) {
                int count;
                const float *axes = glfwGetJoystickAxes(currentController, &count);
                return axes[3];
            }

            return 0;
        }
        float GetRightAnalogY() {
            if (1 == glfwJoystickPresent(currentController)) {
                int count;
                const float *axes = glfwGetJoystickAxes(currentController, &count);
                return axes[4];
            }

            return 0;
        }
        float GetL2() {
            if (1 == glfwJoystickPresent(currentController)) {
                int count;
                const float *axes = glfwGetJoystickAxes(currentController, &count);
                return axes[2];
            }

            return 0;
        }
        float GetR2() {
            if (1 == glfwJoystickPresent(currentController)) {
                int count;
                const float *axes = glfwGetJoystickAxes(currentController, &count);
                return axes[5];
            }

            return 0;
        }
        bool IsButtonPressed(int button) {
            if (1 == glfwJoystickPresent(currentController)) {
                int count;
                const unsigned char *buttons = glfwGetJoystickButtons(currentController, &count);

                if (buttons[button] == GLFW_PRESS) {
                    return true;
                }
            }

            return false;
        }
        bool IsButtonReleased(int button) {
            if (1 == glfwJoystickPresent(currentController)) {
                int count;
                const unsigned char *buttons = glfwGetJoystickButtons(currentController, &count);

                if (buttons[button] == GLFW_RELEASE) {
                    return true;
                }
            }

            return false;
        }
    } // namespace Input::Controller
} // namespace HyperAPI