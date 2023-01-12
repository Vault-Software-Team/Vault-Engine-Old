#pragma once
// THIS IS STILL IN WORKS AND **DOES NOT WORK**
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "Transform.hpp"
#include "../Renderer/Shapes.hpp"

namespace HyperAPI::Experimental {
    struct PathfindingAI : public BaseComponent {
    private:
        struct Node {
            bool obstacle = false;
            bool visited = false;
            float globalGoal;
            float localGoal;
            int x;
            int y;
            std::vector<Node *> neighbors;
            Node *parent;
        };

        Node *nodes = nullptr;
        int mapWidth = 5, mapHeight = 5;

        std::shared_ptr<Cube> cube =
            std::make_shared<Cube>(Vector4(1, 0, 0, 1));

    public:
        PathfindingAI() = default;

        void GUI() {
            if (ImGui::TreeNode("Pathfinding AI")) {
                ImGui::DragInt("Map Width", &mapWidth);
                ImGui::DragInt("Map Height", &mapHeight);

                ImGui::TreePop();
            }
        }

        void CreateGrid() {
            nodes = new Node[mapWidth * mapHeight];

            for (int x = 0; x < mapWidth; x++) {
                for (int y = 0; y < mapHeight; y++) {
                    nodes[y * mapWidth + x].x = x;
                    nodes[y * mapWidth + x].y = y;
                    nodes[y * mapWidth + x].obstacle = false;
                    nodes[y * mapWidth + x].visited = false;
                    nodes[y * mapWidth + x].parent = nullptr;
                }
            }
        }

        void DeleteGrid() { delete nodes; }

        void Update(Shader &shader, Camera &camera) {
            float nodeSize = 1;
            float nodeBorder = 0.2;

            for (int x = 0; x < mapWidth; x++) {
                for (int y = 0; y < mapHeight; y++) {
                    Transform trans;
                    trans.position.x = x * nodeSize + nodeBorder;
                    trans.position.y = y * nodeSize + nodeBorder;
                    trans.Update();
                }
            }
        }
    };
} // namespace HyperAPI::Experimental