#include "Camera.hpp"
#include "Shader.hpp"
#include "../Components/Transform.hpp"

namespace HyperAPI {
    Camera::Camera(bool mode2D, int width, int height, glm::vec3 position,
                   entt::entity entity) {
        Camera::width = width;
        Camera::height = height;
        Camera::mode2D = mode2D;

        this->entity = entity;

        if (entity != entt::null) {
            EnttComp = true;
        } else {
            EnttComp = false;
        }

        if (EnttComp) {
            // auto &transform =
            // Scene::m_Registry.get<Experimental::Transform>(entity);
            // transform.position = position;
            // transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);
        } else {
            TransformComponent transform;
            transform.position = position;
            transform.rotation = glm::vec3(0.0f, 0.0f, glm::radians(-1.0f));
            AddComponent(transform);
        }
    }

    void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane,
                              Vector2 winSize, Vector2 prespectiveSize) {
        bool usePrespectiveSize = false;
        if (prespectiveSize.x != -15)
            usePrespectiveSize = true;

        if (EnttComp) {
            auto &transform =
                Scene::m_Registry.get<Experimental::Transform>(entity);

            view = glm::mat4(1.0f);
            projection = glm::mat4(1.0f);

            width = winSize.x;
            height = winSize.y;

            view = glm::lookAt(
                transform.position,
                transform.position + glm::degrees(transform.rotation), Up);
            float aspect = usePrespectiveSize
                               ? prespectiveSize.x / prespectiveSize.y
                               : width / height;

            aspect = (float)Scene::aspect_width / Scene::aspect_height;
            if (mode2D) {

                projection =
                    glm::ortho(-aspect, aspect, -1.0f, 1.0f, 0.1f, 5000.0f);
                projection =
                    glm::scale(projection, glm::vec3(transform.scale.x,
                                                     transform.scale.y, 1.0f));
            } else {
                projection = glm::perspective(glm::radians(FOVdeg), aspect,
                                              nearPlane, farPlane);
                projection =
                    glm::scale(projection, glm::vec3(transform.scale.x,
                                                     transform.scale.y, 1.0f));
            }
            //            int mouseX = Input::GetMouseX();
            //            int mouseY = -Input::GetMouseY();
            //
            //            glm::vec3 mouseWorld = glm::unProject(glm::vec3(mouseX
            //            * mouseSensitivity, mouseY * mouseSensitivity, 0.0f),
            //            view, projection, glm::vec4(0, 0, width, height));
            //            // its very slow
            //            // make it faster
            //            // STOP COUTING I
            //            mousePosWorld = mouseWorld;

            // mouse coordinates in world space
            //            glm::vec4 mousePos = glm::inverse(projection) *
            //            glm::vec4(mouseX, mouseY, 0.0f, 1.0f); mousePos /=
            //            mousePos.w; mousePos = glm::inverse(view) * mousePos;
            //            mousePos /= mousePos.w;
            //
            //            mousePosWorld = glm::vec3(mousePos.x, mousePos.y,
            //            mousePos.z); mousePosWorld =
            //            glm::normalize(mousePosWorld);
            //
            //            glm::vec4 mousePosCam = glm::inverse(view) *
            //            glm::vec4(mouseX, mouseY, 0.0f, 1.0f); mousePosCam /=
            //            mousePosCam.w; mousePosCamWorld =
            //            glm::vec3(mousePosCam.x, mousePosCam.y,
            //            mousePosCam.z); mousePosCamWorld =
            //            glm::normalize(mousePosCamWorld);

            camMatrix = projection * view;
        } else {
            auto transform = GetComponent<TransformComponent>();

            view = glm::mat4(1.0f);
            projection = glm::mat4(1.0f);

            width = winSize.x;
            height = winSize.y;

            view = glm::lookAt(transform.position,
                               transform.position + transform.rotation, Up);
            float aspect = usePrespectiveSize
                               ? prespectiveSize.x / prespectiveSize.y
                               : width / height;

            if (mode2D) {
                aspect = (float)Scene::aspect_width / Scene::aspect_height;
                projection =
                    glm::ortho(-aspect, aspect, -1.0f, 1.0f, 0.1f, 5000.0f);
                projection =
                    glm::scale(projection, glm::vec3(transform.scale.x,
                                                     transform.scale.y, 1.0f));
            } else {
                projection = glm::perspective(glm::radians(FOVdeg), aspect,
                                              nearPlane, farPlane);
            }

            //            int mouseX = sceneMouseX;
            //            int mouseY = sceneMouseY;
            //
            //            float ndc_x = (2.0f * mouseX) / width - 1.0f;
            //            float ndc_y = 1.0f - (2.0f * mouseY) / height;
            //
            //            float focal_length = 1.0f / tanf(glm::radians(FOVdeg
            //            / 2.0f)); float ar = width / height; Vector3
            //            ray_view(ndc_x / focal_length, (ndc_y * ar) /
            //            focal_length, 1.0f);
            //
            //            Vector4 ray_ndc_4d(ndc_x, ndc_y, 1.0f, 1.0f);
            //            Vector4 ray_view_4d = glm::inverse(projection) *
            //            ray_ndc_4d;
            //
            //            if(Scene::m_Object != nullptr) {
            //                auto &m_Transform =
            //                Scene::m_Object->GetComponent<Experimental::Transform>();
            //                Vector4 view_space_intersect = Vector4(ray_view *
            //                transform.position.z, 1.0f); Vector4 point_world =
            //                glm::inverse(view) * view_space_intersect;
            //                m_Transform.position = Vector3(point_world.x,
            //                point_world.y, m_Transform.position.z);
            //            }

            camMatrix = projection * view;
        }
    }

    void Camera::Matrix(Shader &shader, const char *uniform) {
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1,
                           GL_FALSE, glm::value_ptr(camMatrix));
        shader.SetUniformMat4("cam_view", view);
        shader.SetUniformMat4("cam_projection", projection);
    }

    void Camera::ControllerCameraMove(GLFWwindow *window) {
        if (!EnttComp)
            return;

        auto &transform =
            Scene::m_Registry.get<Experimental::Transform>(entity);

        if (!mode2D) {
            rotX = Input::Controller::GetRightAnalogY();
            rotY = Input::Controller::GetRightAnalogX();

            if (rotX < 0.1f && rotX > -0.2f) {
                rotX = 0.0f;
            }

            if (rotY < 0.1f && rotY > -0.2f) {
                rotY = 0.0f;
            }

            glm::vec3 newOrientation =
                glm::rotate(transform.rotation,
                            glm::radians(-(rotX * controllerSensitivity)),
                            glm::normalize(glm::cross(transform.rotation, Up)));

            if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <=
                glm::radians(85.0f)) {
                transform.rotation = newOrientation;
            }

            transform.rotation =
                glm::rotate(transform.rotation,
                            glm::radians(-(rotY * controllerSensitivity)), Up);
        }
    }

    void Camera::MouseMovement(glm::vec2 winPos) {
        if (!EnttComp)
            return;

        auto &transform =
            Scene::m_Registry.get<Experimental::Transform>(entity);
        if (!mode2D) {
            glfwSetInputMode(Input::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
#ifndef GAME_BUILD
            if (glfwGetMouseButton(Input::window, GLFW_MOUSE_BUTTON_LEFT) ==
                GLFW_PRESS) {
                glfwSetInputMode(Input::window, GLFW_CURSOR,
                                 GLFW_CURSOR_HIDDEN);

                if (firstClick) {
                    glfwSetCursorPos(Input::window, (width / 2), (height / 2));
                    firstClick = false;
                }

                double mouseX;
                double mouseY;
                glfwGetCursorPos(Input::window, &mouseX, &mouseY);

                rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
                rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

                glm::vec3 newOrientation = glm::rotate(
                    transform.rotation, glm::radians(-rotX),
                    glm::normalize(glm::cross(transform.rotation, Up)));

                if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <=
                    glm::radians(85.0f)) {
                    transform.rotation = newOrientation;
                }

                transform.rotation =
                    glm::rotate(transform.rotation, glm::radians(-rotY), Up);

                glfwSetCursorPos(Input::window, (width / 2), (height / 2));
            } else if (glfwGetMouseButton(Input::window,
                                          GLFW_MOUSE_BUTTON_LEFT) ==
                       GLFW_RELEASE) {
                glfwSetInputMode(Input::window, GLFW_CURSOR,
                                 GLFW_CURSOR_NORMAL);
                // glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y +
                // (height / 2));
                firstClick = true;
            }
#else
            glfwSetInputMode(Input::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            double mouseX;
            double mouseY;
            glfwGetCursorPos(Input::window, &mouseX, &mouseY);

            rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
            rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

            glm::vec3 newOrientation =
                glm::rotate(transform.rotation, glm::radians(-rotX),
                            glm::normalize(glm::cross(transform.rotation, Up)));

            // if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <=
            // glm::radians(85.0f))
            // {
            transform.rotation = newOrientation;
            // }

            transform.rotation =
                glm::rotate(transform.rotation, glm::radians(-rotY), Up);

            glfwSetCursorPos(Input::window, (width / 2), (height / 2));
#endif
        }
    }

    void Camera::Inputs(GLFWwindow *window, Vector2 winPos) {
        if (EnttComp) {
            auto &transform =
                Scene::m_Registry.get<Experimental::Transform>(entity);
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                if (!mode2D) {
                    transform.position += speed * transform.rotation;
                } else {
                    transform.position.y += speed;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                transform.position +=
                    speed * -glm::normalize(glm::cross(transform.rotation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                if (!mode2D) {
                    transform.position += speed * -transform.rotation;
                } else {
                    transform.position.y -= speed;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                transform.position +=
                    speed * glm::normalize(glm::cross(transform.rotation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                transform.position += speed * Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                transform.position += speed * -Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                speed = config.editorCamera.shiftSpeed;
            } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ==
                       GLFW_RELEASE) {
                speed = 0.1f;
            }

            if (!mode2D) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
                    GLFW_PRESS) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

                    if (firstClick) {
                        glfwSetCursorPos(window, (width / 2), (height / 2));
                        firstClick = false;
                    }

                    double mouseX;
                    double mouseY;
                    glfwGetCursorPos(window, &mouseX, &mouseY);

                    rotX =
                        sensitivity * (float)(mouseY - (height / 2)) / height;
                    rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

                    glm::vec3 newOrientation = glm::rotate(
                        transform.rotation, glm::radians(-rotX),
                        glm::normalize(glm::cross(transform.rotation, Up)));

                    // if (abs(glm::angle(newOrientation, Up) -
                    // glm::radians(90.0f))
                    // <= glm::radians(85.0f))
                    // {
                    transform.rotation = newOrientation;
                    // }

                    transform.rotation = glm::rotate(transform.rotation,
                                                     glm::radians(-rotY), Up);

                    glfwSetCursorPos(window, (width / 2), (height / 2));
                } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
                           GLFW_RELEASE) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    // glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y
                    // + (height / 2));
                    firstClick = true;
                }
            }
        } else {
            auto transform = GetComponent<TransformComponent>();
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                if (!mode2D) {
                    transform.position += speed * transform.rotation;
                } else {
                    transform.position.y += speed;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                transform.position +=
                    speed * -glm::normalize(glm::cross(transform.rotation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                if (!mode2D) {
                    transform.position += speed * -transform.rotation;
                } else {
                    transform.position.y -= speed;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                transform.position +=
                    speed * glm::normalize(glm::cross(transform.rotation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                transform.position += speed * Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                transform.position += speed * -Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                speed = config.editorCamera.shiftSpeed;
            } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ==
                       GLFW_RELEASE) {
                speed = 0.1f;
            }

            if (!mode2D) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
                    GLFW_PRESS) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

                    if (firstClick) {
                        glfwSetCursorPos(window, (width / 2), (height / 2));
                        firstClick = false;
                    }

                    double mouseX;
                    double mouseY;
                    glfwGetCursorPos(window, &mouseX, &mouseY);

                    rotX =
                        sensitivity * (float)(mouseY - (height / 2)) / height;
                    rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
                    //                    std::cout << rotX << " " << rotY <<
                    //                    std::endl;

                    glm::vec3 newOrientation = glm::rotate(
                        transform.rotation, glm::radians(-rotX),
                        glm::normalize(glm::cross(transform.rotation, Up)));

                    if (abs(glm::angle(newOrientation, Up) -
                            glm::radians(90.0f)) <= glm::radians(85.0f)) {
                        transform.rotation = newOrientation;
                    }

                    transform.rotation = glm::rotate(transform.rotation,
                                                     glm::radians(-rotY), Up);

                    glfwSetCursorPos(window, (width / 2), (height / 2));
                } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
                           GLFW_RELEASE) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    // glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y
                    // + (height / 2));
                    firstClick = true;
                }
                UpdateComponent(transform);
            }
            UpdateComponent(transform);
        }
    }

    void Camera::ControllerInput(GLFWwindow *window) {
        if (EnttComp) {
            auto &transform =
                Scene::m_Registry.get<Experimental::Transform>(entity);

            if (Input::Controller::GetLeftAnalogY() < -0.1f) {
                std::cout << Input::Controller::GetLeftAnalogY() << std::endl;
                transform.position +=
                    -Input::Controller::GetLeftAnalogY() * transform.rotation;
            } else if (Input::Controller::GetLeftAnalogY() > 0.1f) {
                transform.position +=
                    Input::Controller::GetLeftAnalogY() * -transform.rotation;
            }

            if (Input::Controller::GetLeftAnalogX() > 0.1f) {
                transform.position +=
                    Input::Controller::GetLeftAnalogX() *
                    glm::normalize(glm::cross(transform.rotation, Up));
            } else if (Input::Controller::GetLeftAnalogX() < -0.1f) {
                transform.position +=
                    -Input::Controller::GetLeftAnalogX() *
                    -glm::normalize(glm::cross(transform.rotation, Up));
            }

            if (Input::Controller::IsButtonPressed(KEY_CONTROLLER_L3)) {
                transform.position += speed * Up;
            } else if (Input::Controller::IsButtonPressed(KEY_CONTROLLER_R3)) {
                transform.position += speed * -Up;
            }

            if (!mode2D) {
                if (firstClick) {
                    glfwSetCursorPos(window, (width / 2), (height / 2));
                    firstClick = false;
                }

                double mouseX;
                double mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                rotX = Input::Controller::GetRightAnalogY();
                rotY = Input::Controller::GetRightAnalogX();

                if (rotX < 0.1f && rotX > -0.2f) {
                    rotX = 0.0f;
                }

                if (rotY < 0.1f && rotY > -0.2f) {
                    rotY = 0.0f;
                }

                glm::vec3 newOrientation = glm::rotate(
                    transform.rotation, glm::radians(-rotX),
                    glm::normalize(glm::cross(transform.rotation, Up)));

                // if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f))
                // <= glm::radians(85.0f))
                // {
                transform.rotation = newOrientation;
                // }

                transform.rotation =
                    glm::rotate(transform.rotation, glm::radians(-rotY), Up);
            }
        } else {
            auto transform = GetComponent<TransformComponent>();
            // front
            if (Input::Controller::GetLeftAnalogY() < -0.1f) {
                std::cout << Input::Controller::GetLeftAnalogY() << std::endl;
                transform.position +=
                    -Input::Controller::GetLeftAnalogY() * transform.rotation;
            } else if (Input::Controller::GetLeftAnalogY() > 0.1f) {
                transform.position +=
                    Input::Controller::GetLeftAnalogY() * -transform.rotation;
            }

            if (Input::Controller::GetLeftAnalogX() > 0.1f) {
                transform.position +=
                    Input::Controller::GetLeftAnalogX() *
                    glm::normalize(glm::cross(transform.rotation, Up));
            } else if (Input::Controller::GetLeftAnalogX() < -0.1f) {
                transform.position +=
                    -Input::Controller::GetLeftAnalogX() *
                    -glm::normalize(glm::cross(transform.rotation, Up));
            }

            if (Input::Controller::IsButtonPressed(KEY_CONTROLLER_L3)) {
                transform.position += speed * Up;
            } else if (Input::Controller::IsButtonPressed(KEY_CONTROLLER_R3)) {
                transform.position += speed * -Up;
            }

            if (!mode2D) {
                if (firstClick) {
                    glfwSetCursorPos(window, (width / 2), (height / 2));
                    firstClick = false;
                }

                double mouseX;
                double mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                rotX = Input::Controller::GetRightAnalogY();
                rotY = Input::Controller::GetRightAnalogX();

                if (rotX < 0.1f && rotX > -0.1f) {
                    rotX = 0.0f;
                }

                if (rotY < 0.1f && rotY > -0.1f) {
                    rotY = 0.0f;
                }
                //                    std::cout << rotX << " " << rotY <<
                //                    std::endl;

                glm::vec3 newOrientation = glm::rotate(
                    transform.rotation, glm::radians(-rotX),
                    glm::normalize(glm::cross(transform.rotation, Up)));

                if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <=
                    glm::radians(85.0f)) {
                    transform.rotation = newOrientation;
                }

                transform.rotation =
                    glm::rotate(transform.rotation, glm::radians(-rotY), Up);
                UpdateComponent(transform);
            }
            UpdateComponent(transform);
        }
    }
} // namespace HyperAPI