#include "libs.hpp"
#include "box2d/b2_world_callbacks.h"

namespace uuid {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        }
        return ss.str();
    }
} // namespace uuid

namespace HyperAPI {
    std::string cwd = "";
    std::string dirPayloadData = "";
    bool isRunning = false;
    bool isStopped = true;
    glm::vec3 mousePosWorld, mousePosCamWorld;
    float sceneMouseX, sceneMouseY;
    b2ContactListener *b2_listener;
    Config config = {
        "Vault Engine",
        "assets/scenes/main.vault",
        0.2,
    };
} // namespace HyperAPI