#pragma once
#include <libs.hpp>
#include "Mesh.hpp"
#include "Model.hpp"

namespace HyperAPI {
    class Capsule : public Model {
    public:
        Capsule(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Cube : public Model {
    public:
        Cube(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Plane {
    public:
        Mesh *m_Mesh;
        Vector4 color;

        Plane(Vector4 color = Vector4(1, 1, 1, 1));

        void Draw(Shader &shader, Camera &camera);

        template <typename T> void AddComponent(T component) {
            m_Mesh->Components.push_back(std::any_cast<T>(component));
        }

        template <typename T> T GetComponent() {
            for (auto component : m_Mesh->Components) {
                try {
                    if (typeid(T) == typeid(std::any_cast<T>(component))) {
                        return std::any_cast<T>(component);
                    }
                } catch (const std::bad_any_cast &e) {
                }
            }
        }

        template <typename T> bool HasComponent() {
            for (auto component : m_Mesh->Components) {
                if (typeid(T) == typeid(std::any_cast<T>(component))) {
                    return true;
                }
            }
            return false;
        }

        template <typename T> void UpdateComponent(T component) {
            for (auto &comp : m_Mesh->Components) {
                if (typeid(T) == typeid(std::any_cast<T>(comp))) {
                    comp = component;
                }
            }
        }
    };

    class Cylinder : public Model {
    public:
        Cylinder(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Sphere : public Model {
    public:
        Sphere(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Cone : public Model {
    public:
        Cone(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Torus : public Model {
    public:
        Torus(Vector4 color = Vector4(1, 1, 1, 1));
    };
} // namespace HyperAPI