#include "scene.hpp"
#include "api.hpp"

bool G_END_WITH(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

namespace HyperAPI {
    namespace Scene {
        void DropTargetMat(DragType type, Mesh *currEntity) {
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
                {
                    // const char* path = (const char*)payload->Data;
                    dirPayloadData.erase(0, cwd.length() + 1);

                    if(
                        G_END_WITH(dirPayloadData, ".png") ||
                        G_END_WITH(dirPayloadData, ".jpg") ||
                        G_END_WITH(dirPayloadData, ".jpeg")
                    ) {
                        switch (type)
                        {
                            case DRAG_DIFFUSE: {
                                if(currEntity->material.diffuse != nullptr) {
                                    glDeleteTextures(1, &currEntity->material.diffuse->ID);
                                }

                                currEntity->material.diffuse = new Texture((char*)dirPayloadData.c_str(), 0, "texture_diffuse");
                                break;
                            }
                            case DRAG_SPECULAR: {
                                if(currEntity->material.specular != nullptr) {
                                    glDeleteTextures(1, &currEntity->material.specular->ID);
                                }
                                currEntity->material.specular = new Texture((char*)dirPayloadData.c_str(), 1, "texture_specular");
                                break;
                            }
                            case DRAG_NORMAL: {
                                if(currEntity->material.normal != nullptr) {
                                    glDeleteTextures(1, &currEntity->material.normal->ID);
                                }
                                currEntity->material.normal = new Texture((char*)dirPayloadData.c_str(), 2, "texture_normal");
                                break;
                            }
                        }
                    }

                    if(
                        G_END_WITH(dirPayloadData, ".obj") ||
                        G_END_WITH(dirPayloadData, ".fbx") ||
                        G_END_WITH(dirPayloadData, ".gltf")
                    ) {
                        switch (type)
                        {
                            case DRAG_MODEL: {
                                Experimental::Model *newEntity = new Experimental::Model((char*)dirPayloadData.c_str(), false);
                                break;
                            }
                        }
                    }

                    // if(
                    //     G_END_WITH(dirPayloadData, ".static") && type == DRAG_SCENE
                    // ) {
                    //     LoadScene(dirPayloadData);
                    // }
                }

                ImGui::EndDragDropTarget();
            }
        }

        entt::registry m_Registry;
        std::vector<Experimental::GameObject*> m_GameObjects = {};

        Experimental::GameObject *m_Object = nullptr;
        char name[499];

        std::vector<Mesh*> entities = {};
        std::vector<Model> models = {};
        Camera *mainCamera;
        std::vector<Camera*> cameras = {};
        std::vector<Log> logs = {};
        glm::mat4 projection = glm::mat4(1.0f);

        std::vector<Mesh*> backup_entities = {};
        std::vector<Model> backup_models = {};
        std::vector<Camera*> backup_cameras = {};

        std::vector<HyperAPI::PointLight*> PointLights = {};
        std::vector<HyperAPI::Light2D*> Lights2D = {};
        std::vector<HyperAPI::SpotLight*> SpotLights = {};
        std::vector<HyperAPI::DirectionalLight*> DirLights = {};
        std::vector<HyperAPI::Mesh*> hyperEntities = {};
    }
}
