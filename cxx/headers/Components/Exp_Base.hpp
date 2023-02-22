#pragma once
#include <libs.hpp>

namespace HyperAPI::Experimental {
    class ComponentEntity {
    public:
        std::string parentID = "NO_PARENT";
        std::string name = "GameObject";
        std::string ID;
        std::string tag = "Untagged";
        bool prefab = false;

        entt::entity entity = Scene::m_Registry.create();

        ComponentEntity() = default;

        template <typename T, typename... Args>
        T &AddComponent(Args &&...args) {
            if (!HasComponent<T>() && Scene::m_Registry.valid(entity)) {
                T &component = Scene::m_Registry.emplace<T>(
                    entity, std::forward<Args>(args)...);

                auto &comp = GetComponent<T>();
                comp.entity = entity;
                comp.ID = ID;
                comp.Init();

                return comp;
            }
        }

        template <typename T> T &GetComponent() {
            if (HasComponent<T>()) {
                return Scene::m_Registry.get<T>(entity);
            } else {
                T comp;
                return comp;
            }
        }

        template <typename T> bool HasComponent() {
            if (Scene::m_Registry.valid(entity)) {
                return Scene::m_Registry.has<T>(entity);
            }

            return false;
        }

        template <typename T> void RemoveComponent() {
            Scene::m_Registry.remove<T>(entity);
        }
    };

    struct BaseComponent {
        entt::entity entity = entt::null;
        std::string ID = "";
        bool hasGUI = true;

        virtual void Init() {}
        virtual void GUI() {}
        virtual void DeleteComp() {}
    };

    void DrawVec3Control(const std::string &label, Vector3 &values,
                         float resetValue = 0.0f, float columnWidth = 100.0f);
    void DrawVec2Control(const std::string &label, Vector2 &values,
                         float resetValue = 0.0f, float columnWidth = 100.0f);

    extern bool bulletPhysicsStarted;
    extern nlohmann::json stateScene;
    void StartWorld(b2ContactListener *listener);
    void DeleteWorld();
} // namespace HyperAPI::Experimental