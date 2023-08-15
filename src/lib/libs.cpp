#include "libs.hpp"
#include "box2d/b2_world_callbacks.h"

namespace uuid {
    DLL_API std::random_device rd;
    DLL_API std::mt19937 gen(rd());
    DLL_API std::uniform_int_distribution<> dis(0, 15);
    DLL_API std::uniform_int_distribution<> dis2(8, 11);

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
    DLL_API float runTime = 0;
    DLL_API std::string cwd = "";
    DLL_API std::string dirPayloadData = "";
    DLL_API bool isRunning = false;
    DLL_API bool isStopped = true;
    DLL_API glm::vec3 mousePosWorld, mousePosCamWorld;
    DLL_API float sceneMouseX, sceneMouseY;
    DLL_API b2ContactListener *b2_listener;
    DLL_API std::string currentHyperLog = "";
    DLL_API Config config = {
        "Vault Engine",
        "assets/scenes/main.vault",
        0.2,
    };
} // namespace HyperAPI