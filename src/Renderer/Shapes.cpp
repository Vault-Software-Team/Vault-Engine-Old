#include "Shapes.hpp"

namespace HyperAPI {
    Capsule::Capsule(Vector4 color)
        : Model("assets/models/default/capsule/capsule.obj", false, color) {}

    Cube::Cube(Vector4 color)
        : Model("assets/models/default/cube/cube.obj", false, color) {}

    Plane::Plane(Vector4 color) {
        std::vector<HyperAPI::Vertex> vertices = {
            {glm::vec3(-0.5, 0, 0.5), glm::vec3(0.3, 0.3, 0.3),
             glm::vec3(0, 1, 0), glm::vec2(0, 0)},
            {glm::vec3(-0.5, 0, -0.5), glm::vec3(0.3, 0.3, 0.3),
             glm::vec3(0, 1, 0), glm::vec2(0, 1)},
            {glm::vec3(0.5, 0, -0.5), glm::vec3(0.3, 0.3, 0.3),
             glm::vec3(0, 1, 0), glm::vec2(1, 1)},
            {glm::vec3(0.5, 0, 0.5), glm::vec3(0.3, 0.3, 0.3),
             glm::vec3(0, 1, 0), glm::vec2(1, 0)}};

        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        this->color = color;
        Material material(color, {});
        m_Mesh = new Mesh(vertices, indices, material, false);
    }

    void Plane::Draw(Shader &shader, Camera &camera) {
        m_Mesh->Draw(shader, camera);
    }

    Sphere::Sphere(Vector4 color)
        : Model("assets/models/default/sphere/sphere.obj", false, color) {}

    Cylinder::Cylinder(Vector4 color)
        : Model("assets/models/default/cylinder/cylinder.obj", false, color) {}

    Cone::Cone(Vector4 color)
        : Model("assets/models/default/cone/cone.obj", false, color) {}

    Torus::Torus(Vector4 color)
        : Model("assets/models/default/torus/torus.obj", false, color) {}
} // namespace HyperAPI