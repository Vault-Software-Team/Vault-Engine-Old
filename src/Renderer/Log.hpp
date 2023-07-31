#pragma once
#include <dllapi.hpp>
#include <libs.hpp>

namespace HyperAPI {
    enum LOG_TYPE { LOG_INFO,
                    LOG_WARNING,
                    LOG_ERROR };

    struct DLL_API Log {
        std::string message;
        LOG_TYPE type;

        Log(const std::string &message, LOG_TYPE type) {
            this->message = message;
            this->type = type;
            Scene::logs.push_back(*this);
        }

        void GUI() {
            switch (type) {
            case LOG_INFO:
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                                   ICON_FA_MESSAGE);
                // same line
                ImGui::SameLine();
                ImGui::TextWrapped("%s",
                                   (std::string(" - ") + message).c_str());
                break;
            case LOG_WARNING:
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f),
                                   ICON_FA_TRIANGLE_EXCLAMATION);
                // same line
                ImGui::SameLine();
                ImGui::TextWrapped("%s",
                                   (std::string(" - ") + message).c_str());
                break;
            case LOG_ERROR:
                ImGui::TextColored(ImVec4(1.2f, 0.0f, 0.0f, 1.0f),
                                   ICON_FA_CIRCLE_EXCLAMATION);
                // same line
                ImGui::SameLine();
                ImGui::TextWrapped("%s",
                                   (std::string(" - ") + message).c_str());
                break;
            }
        }
    };
} // namespace HyperAPI