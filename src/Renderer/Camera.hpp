#pragma once
#include <dllapi.hpp>
#include <libs.hpp>

namespace HyperAPI {
    class DLL_API Camera : public ComponentSystem {
    public:
        float controllerSensitivity = 1;
        float cursorWinW, cursorWinH;
        //        glm::vec3 mousePosWorld, mousePosCamWorld;
        // glm::vec3 Position;
        // glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        // glm::vec3 RotationValue = glm::vec3(0.0f, 0.0f, 0.0f);
        std::vector<std::string> layers;
        bool EnttComp = false;
        bool m_MouseMovement = false;
        bool moving = false;
        entt::entity entity = entt::null;

        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 Front = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Right = glm::vec3(0.0f, 0.0f, 0.0f);

        glm::mat4 view, projection;

        float cam_fov = 45;
        float cam_near = 0.1f;
        float cam_far = 100.0f;

        float rotX, rotY;

        bool firstClick = true;
        bool mode2D = false;
        bool main_camera;

        int width;
        int height;

        float speed = 13.0f;
        float sensitivity = 100.0f;

        glm::mat4 camMatrix;

        Camera(bool mode2D, int width, int height, glm::vec3 position,
               entt::entity entity = entt::null);

        void updateMatrix(float FOVdeg, float nearPlane, float farPlane,
                          Vector2 winSize,
                          Vector2 prespectiveSize = Vector2(-15, -15));
        void Matrix(Shader &shader, const char *uniform);
        void Inputs(GLFWwindow *window, Vector2 winPos);
        void ControllerInput(GLFWwindow *window);
        void MouseMovement(Vector2 winPos);
        void ControllerCameraMove(GLFWwindow *window);
    };
} // namespace HyperAPI