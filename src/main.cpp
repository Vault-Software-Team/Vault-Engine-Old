#include <cstdio>
#include <random>
#include <memory>
#include <regex>
#include "Audio/SoundDevice.hpp"
#include "Audio/SoundSource.hpp"
#include "Components/CsharpScriptManager.hpp"
#include "ImGuiColorTextEdit/TextEditor.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Renderer/AudioEngine.hpp"
#include "Scripting/CXX/CppScripting.hpp"
#include "Rusty/hyperlog.hpp"
#include "rusty_vault.hpp"
#include "Renderer/Timestep.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "lib/InputEvents.hpp"
#include "icons/icons.h"
#include "imgui/imgui.h"
#include "lib/api.hpp"
#include "lib/scene.hpp"
#include "libs.hpp"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "mono/metadata/image.h"
#include "mono/metadata/loader.h"
#include "mono/metadata/object-forward.h"
#include "vendor/json/json.hpp"
#include "lib/csharp.hpp"

#ifndef _WIN32
#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#else
#include <direct.h>
#endif

using namespace HyperAPI;
using namespace HyperAPI::Experimental;

static float m_grid_size = 100;
static bool drawGrid = false;
static int m_GuizmoMode = -1;
static int m_GuizmoWorld = -1;
static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
static float boundsSnap[] = {0.1f, 0.1f, 0.1f};
static bool boundSizing = false;
static bool boundSizingSnap = false;

class CollisionListener : public b2ContactListener {
public:
    void BeginContact(b2Contact *contact) override {
        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();
        // getuser data

        b2BodyUserData &bodyUserDataA = fixtureA->GetBody()->GetUserData();
        b2BodyUserData &bodyUserDataB = fixtureB->GetBody()->GetUserData();

        auto *gameObjectA = (GameObject *)bodyUserDataA.pointer;
        auto *gameObjectB = (GameObject *)bodyUserDataB.pointer;

        if (gameObjectA->HasComponent<NativeScriptManager>()) {
            auto &scriptManager =
                gameObjectA->GetComponent<NativeScriptManager>();
            for (auto script : scriptManager.m_StaticScripts) {
                script->Collision2D(gameObjectB);
            }
        }

        if (gameObjectB->HasComponent<NativeScriptManager>()) {
            auto &scriptManager =
                gameObjectB->GetComponent<NativeScriptManager>();
            for (auto script : scriptManager.m_StaticScripts) {
                script->Collision2D(gameObjectA);
            }
        }

        // TODO: C# Events (Collision2D)

        if (gameObjectB->HasComponent<NativeScriptManager>()) {
            auto &scriptManager =
                gameObjectB->GetComponent<NativeScriptManager>();
            for (auto script : scriptManager.m_StaticScripts) {
                script->Collision2D(gameObjectA);
            }
        }

        if (gameObjectA->HasComponent<m_LuaScriptComponent>()) {
            auto &scriptManager =
                gameObjectA->GetComponent<m_LuaScriptComponent>();
            for (auto script : scriptManager.scripts) {
                script.Collision2D(gameObjectB);
            }
        }

        if (gameObjectB->HasComponent<m_LuaScriptComponent>()) {
            auto &scriptManager =
                gameObjectB->GetComponent<m_LuaScriptComponent>();
            for (auto script : scriptManager.scripts) {
                script.Collision2D(gameObjectA);
            }
        }

        if (gameObjectA->HasComponent<CsharpScriptManager>()) {
            auto &scriptManager = gameObjectA->GetComponent<CsharpScriptManager>();

            for (auto klass : scriptManager.selectedScripts) {
                MonoObject *exception = nullptr;
                MonoScriptClass *behaviour =
                    CsharpScriptEngine::instances[klass.first];
                MonoMethod *method = behaviour->GetMethod("OnCollisionEnter2D", 1);
                if (!method)
                    continue;

                void *params = mono_string_new(CsharpVariables::appDomain, gameObjectB->ID.c_str());
                mono_runtime_invoke(method, behaviour->f_GetObject(), &params, &exception);
            }
        }

        if (gameObjectB->HasComponent<CsharpScriptManager>()) {
            auto &scriptManager = gameObjectB->GetComponent<CsharpScriptManager>();

            for (auto klass : scriptManager.selectedScripts) {
                MonoObject *exception = nullptr;
                MonoScriptClass *behaviour =
                    CsharpScriptEngine::instances[klass.first];
                MonoMethod *method = behaviour->GetMethod("OnCollisionEnter2D", 1);
                if (!method)
                    continue;

                void *params = mono_string_new(CsharpVariables::appDomain, gameObjectA->ID.c_str());
                mono_runtime_invoke(method, behaviour->f_GetObject(), &params, &exception);
            }
        }
    }

    void EndContact(b2Contact *contact) override {
        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();
        // getuser data
        b2BodyUserData &bodyUserDataA = fixtureA->GetBody()->GetUserData();
        b2BodyUserData &bodyUserDataB = fixtureB->GetBody()->GetUserData();

        auto *gameObjectA = (GameObject *)bodyUserDataA.pointer;
        auto *gameObjectB = (GameObject *)bodyUserDataB.pointer;

        if (gameObjectA->HasComponent<NativeScriptManager>()) {
            auto &scriptManager =
                gameObjectA->GetComponent<NativeScriptManager>();
            for (auto script : scriptManager.m_StaticScripts) {
                script->CollisionExit2D(gameObjectB);
            }
        }

        if (gameObjectB->HasComponent<NativeScriptManager>()) {
            auto &scriptManager =
                gameObjectB->GetComponent<NativeScriptManager>();
            for (auto script : scriptManager.m_StaticScripts) {
                script->CollisionExit2D(gameObjectA);
            }
        }

        if (gameObjectA->HasComponent<m_LuaScriptComponent>()) {
            auto &scriptManager =
                gameObjectA->GetComponent<m_LuaScriptComponent>();
            for (auto script : scriptManager.scripts) {
                script.CollisionExit2D(gameObjectB);
            }
        }

        if (gameObjectB->HasComponent<m_LuaScriptComponent>()) {
            auto &scriptManager =
                gameObjectB->GetComponent<m_LuaScriptComponent>();
            for (auto script : scriptManager.scripts) {
                script.CollisionExit2D(gameObjectA);
            }
        }

        if (gameObjectA->HasComponent<CsharpScriptManager>()) {
            auto &scriptManager = gameObjectA->GetComponent<CsharpScriptManager>();

            for (auto klass : scriptManager.selectedScripts) {
                MonoObject *exception = nullptr;
                MonoScriptClass *behaviour =
                    CsharpScriptEngine::instances[klass.first];
                MonoMethod *method = behaviour->GetMethod("OnCollisionExit2D", 1);
                if (!method)
                    continue;

                void *params = mono_string_new(CsharpVariables::appDomain, gameObjectB->ID.c_str());
                mono_runtime_invoke(method, behaviour->f_GetObject(), &params, &exception);
            }
        }

        if (gameObjectB->HasComponent<CsharpScriptManager>()) {
            auto &scriptManager = gameObjectB->GetComponent<CsharpScriptManager>();

            for (auto klass : scriptManager.selectedScripts) {
                MonoObject *exception = nullptr;
                MonoScriptClass *behaviour =
                    CsharpScriptEngine::instances[klass.first];
                MonoMethod *method = behaviour->GetMethod("OnCollisionExit2D", 1);
                if (!method)
                    continue;

                void *params = mono_string_new(CsharpVariables::appDomain, gameObjectA->ID.c_str());
                mono_runtime_invoke(method, behaviour->f_GetObject(), &params, &exception);
            }
        }
    }
};

CollisionListener *listener = new CollisionListener();

struct AddComponentList {
    int selected = 0;
    int length = 1;
    bool showed = false;
    const char *items[1] = {"Lua Scripts Component"};
};

char newName[256];

AddComponentList AddComponentConfig;

std::string originalName = "";
bool LoadState = false;

bool ends_with(std::string const &value, std::string const &ending) {
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string m_originalName = "";
char originalNameBuffer[50] = "";

TextEditor editor;
std::string currentFilePath = "";

bool SortEntries(fs::directory_entry a, fs::directory_entry b) {
    return a.path().filename().string() < b.path().filename().string();
}

fs::path relative(fs::path p, fs::path base) {
    // 1. convert p and base to absolute paths
    p = fs::absolute(p);
    base = fs::absolute(base);

    // 2. find first mismatch and shared root path
    auto mismatched =
        std::mismatch(p.begin(), p.end(), base.begin(), base.end());

    // 3. if no mismatch return "."
    if (mismatched.first == p.end() && mismatched.second == base.end())
        return ".";

    auto it_p = mismatched.first;
    auto it_base = mismatched.second;

    fs::path ret;

    // 4. iterate abase to the shared root and append "../"
    for (; it_base != base.end(); ++it_base)
        ret /= "..";

    // 5. iterate from the shared root to the p and append its parts
    for (; it_p != p.end(); ++it_p)
        ret /= *it_p;

    return ret;
}

#ifndef PROJECT_MENU
fs::path currentDirectory = fs::path("assets");

auto langDef = TextEditor::LanguageDefinition::Lua();
auto glslDef = TextEditor::LanguageDefinition::GLSL();
void DirIter(const std::string &path) {
    // alphabetical sort
    auto iter = fs::directory_iterator(currentDirectory);
    std::vector<fs::directory_entry> entries;
    for (auto &p : iter) {
        entries.push_back(p);
    }
    std::sort(entries.begin(), entries.end(), SortEntries);

    std::vector<fs::directory_entry> folders;
    std::vector<fs::directory_entry> files;
    std::vector<fs::directory_entry> paths;

    for (auto &p : entries) {
        if (fs::is_directory(p)) {
            folders.push_back(p);
        } else {
            files.push_back(p);
        }
    }
    std::sort(files.begin(), files.end(), SortEntries);

    folders.insert(folders.end(), files.begin(), files.end());

    // test

    float padding = 16;
    float buttonSize = 100;
    float cellSize = buttonSize + padding;

    float width = ImGui::GetContentRegionAvail().x;
    int count = (int)(width / cellSize);

    if (count == 0)
        count = 5;

    ImGui::Columns(count, 0, false);

    // select second fa-solid-900 font that is twice big
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
    for (const auto &entry : folders) {
        std::string path = entry.path().string();
        fs::path relativePath = relative(entry.path(), currentDirectory);

        if (fs::is_directory(entry)) {
            // move the button text down
            // itme spacing
            ImVec2 cursorPos = ImGui::GetCursorPos();
            // grey color
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            bool item = ImGui::Button(
                std::string("##" + relativePath.filename().string()).c_str(),
                ImVec2(buttonSize, buttonSize));
            ImGui::PopStyleColor(3);

            ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + 25));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::Button(ICON_FA_FOLDER, ImVec2(buttonSize, buttonSize - 25));

            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1)) {
                m_originalName = path;
                strcpy(newName, entry.path().filename().string().c_str());
                ImGui::OpenPopup("File Options");
            }

            ImGui::PopStyleColor();
            ImGui::Text(relativePath.string().c_str());

            if (item) {
                currentDirectory /= entry.path().filename();
            }
        } else {
            ImVec2 cursorPos = ImGui::GetCursorPos();
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            bool item = ImGui::Button(
                std::string("##" + relativePath.filename().string()).c_str(),
                ImVec2(buttonSize, buttonSize));

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                HyperAPI::dirPayloadData = fs::absolute(entry.path()).string();
                ImGui::SetDragDropPayload("file", dirPayloadData.c_str(),
                                          strlen(dirPayloadData.c_str()));
                ImGui::Text(entry.path().filename().string().c_str());
                ImGui::EndDragDropSource();
            }

            ImGui::PopStyleColor(3);

            ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + 30));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            if (ends_with(entry.path().string(), ".lua")) {
                ImGui::Button(ICON_FA_CODE,
                              ImVec2(buttonSize, buttonSize - 30));
                if (item) {
                    std::ifstream file(entry.path().string());
                    std::string str((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
                    editor.SetText(str);
                    editor.SetLanguageDefinition(langDef);
                    currentFilePath = entry.path().string();
                }
            } else if (ends_with(entry.path().string(), ".cpp") || ends_with(entry.path().string(), ".cc") || ends_with(entry.path().string(), ".cxx") || ends_with(entry.path().string(), ".hpp")) {
                ImGui::Button(ICON_FA_CODE,
                              ImVec2(buttonSize, buttonSize - 30));
                if (item) {
                    std::ifstream file(entry.path().string());
                    std::string str((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
                    editor.SetText(str);
                    editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
                    currentFilePath = entry.path().string();
                }
            } else if (ends_with(entry.path().string(), ".dll") || ends_with(entry.path().string(), ".so")) {
                ImGui::Button(ICON_FA_GEARS,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".o")) {
                ImGui::Button(ICON_FA_WRENCH,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".glsl")) {
                ImGui::Button(ICON_FA_PAINT_ROLLER,
                              ImVec2(buttonSize, buttonSize - 30));
                if (item) {
                    std::ifstream file(entry.path().string());
                    std::string str((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
                    editor.SetText(str);
                    editor.SetLanguageDefinition(glslDef);
                    currentFilePath = entry.path().string();
                }
            } else if (ends_with(entry.path().string(), ".png") ||
                       ends_with(entry.path().string(), ".jpg") ||
                       ends_with(entry.path().string(), ".jpeg")) {
                ImGui::Button(ICON_FA_IMAGE,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".ogg") ||
                       ends_with(entry.path().string(), ".mp3") ||
                       ends_with(entry.path().string(), ".wav")) {
                ImGui::Button(ICON_FA_FILE_AUDIO,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".ttf") ||
                       ends_with(entry.path().string(), ".otf")) {
                ImGui::Button(ICON_FA_FONT,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".vault")) {
                ImGui::Button(ICON_FA_CUBES,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".json")) {
                ImGui::Button(ICON_FA_FILE_CODE,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".prefab")) {
                ImGui::Button(ICON_FA_CUBE,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".material")) {
                ImGui::Button(ICON_FA_PAINTBRUSH,
                              ImVec2(buttonSize, buttonSize - 30));
            } else if (ends_with(entry.path().string(), ".vault.model")) {
                ImGui::Button(ICON_FA_CUBES,
                              ImVec2(buttonSize, buttonSize - 30));
            } else {
                ImGui::Button(ICON_FA_FILE,
                              ImVec2(buttonSize, buttonSize - 30));
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1)) {
                m_originalName = path;
                strcpy(newName, entry.path().filename().string().c_str());
                ImGui::OpenPopup("File Options");
            }

            ImGui::PopStyleColor();
            ImGui::Text(relativePath.string().c_str());

            if (item) {
            }
        }

        ImGui::NextColumn();
    }
    ImGui::PopFont();
    ImGui::Columns(1);

    //    return;
    //
    //    for (const auto &entry : folders) {
    //        bool item = ImGui::TreeNodeEx(
    //                (std::string(ICON_FA_FOLDER) + " " +
    //                entry.path().filename().string()).c_str(),
    //                ImGuiTreeNodeFlags_SpanAvailWidth);
    //        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1)) {
    //            m_originalName = entry.path().string();
    //            ImGui::OpenPopup("File Options");
    //        }
    //
    //        if (ImGui::BeginDragDropTarget()) {
    //            if (const ImGuiPayload *payload =
    //            ImGui::AcceptDragDropPayload("file")) {
    //                // fs::rename(file, newFile);move the file into the folder
    //                std::string file = (char *) payload->Data;
    //                file.erase(0, cwd.length() + 1);
    //                std::string newFile = entry.path().string() + "/" +
    //                file.substr(file.find_last_of("/") + 1); try {
    //                    fs::rename(file, newFile);
    //                } catch (const std::exception &e) {
    //                }
    //            }
    //            ImGui::EndDragDropTarget();
    //        }
    //        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
    //            dirPayloadData = entry.path().string();
    //            ImGui::SetDragDropPayload("file",
    //            entry.path().string().c_str(), entry.path().string().size());
    //            ImGui::Text(entry.path().filename().string().c_str());
    //            ImGui::EndDragDropSource();
    //        }
    //
    //        if (item) {
    //            DirIter(entry.path().string());
    //            ImGui::TreePop();
    //        }
    //    }
    //    for (const auto &entry : files) {
    //        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 25);
    //        if (ends_with(entry.path().string(), ".lua")) {
    //            ImGui::Selectable((std::string(ICON_FA_CODE) + " " +
    //            entry.path().filename().string()).c_str()); if
    //            (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
    //                std::ifstream file(entry.path().string());
    //                std::string str((std::istreambuf_iterator<char>(file)),
    //                std::istreambuf_iterator<char>()); editor.SetText(str);
    //                currentFilePath = entry.path().string();
    //            }
    //        } else if (ends_with(entry.path().string(), ".png") ||
    //        ends_with(entry.path().string(), ".jpg") ||
    //                   ends_with(entry.path().string(), ".jpeg")) {
    //            ImGui::Selectable((std::string(ICON_FA_IMAGE) + " " +
    //            entry.path().filename().string()).c_str());
    //        } else if (ends_with(entry.path().string(), ".ogg") ||
    //        ends_with(entry.path().string(), ".mp3") ||
    //                   ends_with(entry.path().string(), ".wav")) {
    //            ImGui::Selectable((std::string(ICON_FA_FILE_AUDIO) + " " +
    //            entry.path().filename().string()).c_str());
    //        } else if (ends_with(entry.path().string(), ".ttf") ||
    //        ends_with(entry.path().string(), ".otf")) {
    //            ImGui::Selectable((std::string(ICON_FA_FONT) + " " +
    //            entry.path().filename().string()).c_str());
    //        } else if (ends_with(entry.path().string(), ".vault")) {
    //            ImGui::Selectable((std::string(ICON_FA_CUBES) + " " +
    //            entry.path().filename().string()).c_str());
    //        } else if (ends_with(entry.path().string(), ".json")) {
    //            ImGui::Selectable((std::string(ICON_FA_FILE_CODE) + " " +
    //            entry.path().filename().string()).c_str());
    //        } else if (ends_with(entry.path().string(), ".prefab")) {
    //            ImGui::Selectable((std::string(ICON_FA_CUBE) + " " +
    //            entry.path().filename().string()).c_str());
    //        } else if (ends_with(entry.path().string(), ".material")) {
    //            ImGui::Selectable((std::string(ICON_FA_PAINTBRUSH) + " " +
    //            entry.path().filename().string()).c_str());
    //        } else {
    //            ImGui::Selectable((std::string(ICON_FA_FILE) + " " +
    //            entry.path().filename().string()).c_str());
    //        }
    //
    //        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1)) {
    //            m_originalName = entry.path().string();
    //            ImGui::OpenPopup("File Options");
    //        }
    //
    //        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
    //            dirPayloadData = entry.path().string();
    //            ImGui::SetDragDropPayload("file", dirPayloadData.c_str(),
    //            strlen(dirPayloadData.c_str()));
    //            ImGui::Text(entry.path().filename().string().c_str());
    //            ImGui::EndDragDropSource();
    //        }
    //    }
}

void ApplyMaterial(nlohmann::json JSON, Material &material, int i) {
    material.baseColor = glm::vec4(JSON[i]["material"]["baseColor"]["r"],
                                   JSON[i]["material"]["baseColor"]["g"],
                                   JSON[i]["material"]["baseColor"]["b"], 1);

    material.roughness = JSON[i]["material"]["roughness"];
    material.metallic = JSON[i]["material"]["metallic"];
    material.texUVs = Vector2(JSON[i]["material"]["texUV"]["x"],
                              JSON[i]["material"]["texUV"]["y"]);

    if (JSON[i]["material"]["diffuse"] != "nullptr") {
        std::string diffusePath = JSON[i]["material"]["diffuse"];
        material.diffuse =
            new Texture((char *)diffusePath.c_str(), 0, "texture_diffuse");
    }

    if (JSON[i]["material"]["specular"] != "nullptr") {
        std::string specularPath = JSON[i]["material"]["specular"];
        material.specular =
            new Texture((char *)specularPath.c_str(), 1, "texture_specular");
    }

    if (JSON[i]["material"]["normal"] != "nullptr") {
        std::string normalPath = JSON[i]["material"]["normal"];
        material.normal =
            new Texture((char *)normalPath.c_str(), 2, "texture_normal");
    }
}

namespace InspecType {
    enum Type { None,
                Material };
}

struct InspectorMaterial {
    std::string diffuse = "None";
    std::string specular = "None";
    std::string normal = "None";
    std::string height = "None";
    float metallic = 0;
    float roughness = 0;
    Vector4 baseColor = Vector4(1, 1, 1, 1);
    Vector2 texUVs = Vector2(0, 0);
};

bool editingText = false;

void ShortcutManager(bool &openConfig) {
    if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) &&
        ImGui::IsKeyPressed(GLFW_KEY_G)) {
        drawGrid = !drawGrid;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_LeftAlt) && ImGui::IsKeyPressed(GLFW_KEY_T)) {
        m_GuizmoMode = ImGuizmo::OPERATION::TRANSLATE;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_LeftAlt) && ImGui::IsKeyPressed(GLFW_KEY_R)) {
        m_GuizmoMode = ImGuizmo::OPERATION::ROTATE;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_LeftAlt) && ImGui::IsKeyPressed(GLFW_KEY_S)) {
        m_GuizmoMode = ImGuizmo::OPERATION::SCALE;
    }

    if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) &&
        ImGui::IsKeyPressed(GLFW_KEY_S) &&
        !ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        if (!editingText) {
            if (Scene::currentScenePath == "") {
                ImGuiFileDialog::Instance()->OpenDialog(
                    "SaveSceneDialog", ICON_FA_FLOPPY_DISK " Save Scene",
                    ".vault", ".");
            } else {
                nlohmann::json S_SJ;
                Scene::SaveScene(Scene::currentScenePath, S_SJ);
            }
        } else if (editingText) {
            std::ofstream file(currentFilePath);
            file << editor.GetText();
            file.close();
        }
    }

    if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) &&
        ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) &&
        ImGui::IsKeyPressed(GLFW_KEY_S)) {
        ImGuiFileDialog::Instance()->OpenDialog(
            "SaveSceneDialog", ICON_FA_FLOPPY_DISK " Save Scene", ".vault",
            ".");
    }

    if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) &&
        ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) &&
        ImGui::IsKeyPressed(GLFW_KEY_C)) {
        openConfig = true;
    }

    if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) &&
        ImGui::IsKeyPressed(GLFW_KEY_B) && ImGui::IsKeyPressed(GLFW_KEY_L)) {
        ImGuiFileDialog::Instance()->OpenDialog(
            "BuildLinuxDialog", "Build for Linux", nullptr, ".");
    }

    if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) &&
        ImGui::IsKeyPressed(GLFW_KEY_B) && ImGui::IsKeyPressed(GLFW_KEY_W)) {
        ImGuiFileDialog::Instance()->OpenDialog(
            "BuildWindowsDialog", "Build for Windows", nullptr, ".");
    }

    // copying the Scene::m_Object
    if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) &&
        ImGui::IsKeyPressed(GLFW_KEY_D)) {
        // copy Scene::m_Object entt entity into a new one
        if (Scene::m_Object == nullptr)
            return;
        nlohmann::json JSON;
        Scene::SaveJSONPrefab(JSON, Scene::m_Object);
        Scene::LoadJSONPrefab(JSON);
        JSON.clear();
    }
}

time_t timestamp = time(0);

void PostProcessingEffects(Shader &shader, const Camera *sceneCamera) {
    // if(Scene::mainCamera == sceneCamera) return;
    // if(!config.postProcessing.enabled) return;

    // for(auto &shader : shaders) {
    //     shader->Bind();
    //     shader->SetUniform1i("globalBloom",
    //     config.postProcessing.bloom.enabled);
    //     shader->SetUniform1f("bloomThreshold",
    //     config.postProcessing.bloom.threshold);
    // }
    shader.Bind();
    shader.SetUniform1i("globalBloom", config.postProcessing.bloom.enabled);
    shader.SetUniform1f("bloomThreshold",
                        config.postProcessing.bloom.threshold);
}

#ifndef _WIN32 || GAME_BUILD

void UpdatePresence(const std::string &details = "",
                    const std::string &state = "",
                    const std::string &largeImageKey = "logo",
                    const std::string &largeImageText = "",
                    const std::string &smallImageKey = "",
                    const std::string &smallImageText = "") {
    char buffer[256];
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.largeImageKey = largeImageKey.c_str();
    discordPresence.largeImageText = largeImageText.c_str();
    discordPresence.smallImageKey = smallImageKey.c_str();
    discordPresence.smallImageText = smallImageText.c_str();
    discordPresence.state = state.c_str();
    discordPresence.details = details.c_str();
    discordPresence.startTimestamp = timestamp;
    Discord_UpdatePresence(&discordPresence);
}

#endif

using namespace CppScripting;

#ifdef RUSTY_BUILD
int cpp_play_audio(char *audio_file) {
    std::cout << audio_file << std::endl;
    AudioEngine::PlaySound(audio_file);
}
#endif

#ifdef _WIN32
#include <windows.h>
#endif
typedef CppScripting::Script *(*f_create_object)();

int main(int argc, char **argv) {
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        CsharpVariables::oldCwd = cwd;
    }

#ifndef _WIN32
    unsetenv("TERM");
    // setenv("MONO_LOG_LEVEL", "debug", 0);
    // setenv("MONO_LOG_MASK", "dll", 0);
#endif

    if (argc > 1) {
#ifdef _WIN32
        std::string path = argv[1];
        _chdir(argv[1]);
#else
        chdir(argv[1]);
#endif

        // cwd
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        std::cout << "Current working dir: " << cwd << std::endl;
    }

    char m_cwd[1024];
    getcwd(m_cwd, sizeof(m_cwd));
    std::cout << m_cwd << std::endl;
    std::cout << CsharpVariables::oldCwd << std::endl;
    if (CsharpVariables::oldCwd != std::string(m_cwd)) {
        if (fs::exists("cs-assembly/API")) {
            fs::remove_all("cs-assembly/API");
        }
        fs::copy(CsharpVariables::oldCwd + "/cs-assembly/API", "cs-assembly/API", fs::copy_options::recursive);
    }

    CsharpScriptEngine::InitMono();

    if (!fs::exists("cs-assembly"))
        fs::create_directory("cs-assembly");

    filewatch::FileWatch<std::string> watch("./cs-assembly", [&](const std::string &filename, const filewatch::Event change_type) {
        HYPER_LOG(filename);
        system("cd cs-assembly && dotnet build");
    });

#ifndef _WIN32 || GAME_BUILD
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = [](const DiscordUser *request) {
        std::cout << "Discord: Ready" << std::endl;
    };

    handlers.errored = [](int errorCode, const char *message) {
        std::cout << "Discord: Error " << errorCode << ": " << message
                  << std::endl;
    };

    handlers.disconnected = [](int errorCode, const char *message) {
        std::cout << "Discord: Disconnected " << errorCode << ": " << message
                  << std::endl;
    };

    handlers.joinGame = [](const char *joinSecret) {
        std::cout << "Discord: Join Game " << joinSecret << std::endl;
    };

    handlers.spectateGame = [](const char *spectateSecret) {
        std::cout << "Discord: Spectate Game " << spectateSecret << std::endl;
    };

    Discord_Initialize("1025522890688442400", &handlers, 1, nullptr);

    UpdatePresence("In Editor", "Making a game");
#endif

    Scene::layers["Default"] = true;
    // ScriptEngine::Init();

    char CWD[1024];
#ifdef _WIN32
    _getcwd(CWD, sizeof(CWD));
#else
    getcwd(CWD, sizeof(CWD));
#endif
    cwd = std::string(CWD);

    // check if game.config exists
    std::ifstream file("assets/game.config");
    bool mainSceneFound = false;
    if (file.is_open()) {
        nlohmann::json JSON = nlohmann::json::parse(file);

        config.width = JSON["width"];
        config.height = JSON["height"];
        strcpy(config.name, ((std::string)JSON["name"]).c_str());
        config.ambientLight = JSON["ambientLight"];
        config.exposure =
            JSON.contains("exposure") ? (float)JSON["exposure"] : 1.0f;
        config.mainScene = JSON["mainScene"];
        config.resizable = JSON["resizable"];
        config.fullscreenOnLaunch = JSON["fullscreen_on_launch"];
        strcpy(config.linuxCompiler,
               ((std::string)JSON["linux_compiler"]).c_str());
        strcpy(config.windowsCompiler,
               ((std::string)JSON["windows_compiler"]).c_str());
        for (auto &layer : JSON["layers"]) {
            Scene::layers[(std::string)layer] = true;
        }
        mainSceneFound = true;

        if (JSON.contains("post_processing")) {
            config.postProcessing.enabled = JSON["post_processing"]["enabled"];
            config.postProcessing.bloom.enabled =
                JSON["post_processing"]["bloom"]["enabled"];
            config.postProcessing.bloom.threshold =
                JSON["post_processing"]["bloom"]["threshold"];

            config.postProcessing.vignette.intensity =
                JSON["post_processing"]["vignette"]["intensity"];
            config.postProcessing.vignette.smoothness =
                JSON["post_processing"]["vignette"]["smoothness"];

            config.postProcessing.chromaticAberration.intensity =
                JSON["post_processing"]["chromatic_aberration"]["intensity"];
        }
    } else {
        nlohmann::json j = {
            {"linux_compiler", config.linuxCompiler},
            {"windows_compiler", config.windowsCompiler},
            {"name", config.name},
            {"ambientLight", config.ambientLight},
            {"exposure", config.exposure},
            {"mainScene", config.mainScene},
            {"width", config.width},
            {"height", config.height},
            {"resizable", config.resizable},
            {"fullscreen_on_launch", config.fullscreenOnLaunch},
            {"layers", {"Default"}},
            {"post_processing",
             {{"enabled", false},
              {"bloom", {{"enabled", false}, {"threshold", 0.5f}}},
              {"vignette",
               {
                   {"enabled", config.postProcessing.enabled},
                   {"intensity", config.postProcessing.vignette.intensity},
                   {"smoothness", config.postProcessing.vignette.smoothness},
               }},
              {"bloom",
               {
                   {"enabled", config.postProcessing.enabled},
                   {"threshold", config.postProcessing.bloom.threshold},
               }},
              {"chromatic_aberration",
               {
                   {"intensity",
                    config.postProcessing.chromaticAberration.intensity},
               }}}}};

        std::ofstream o("assets/game.config");
        o << std::setw(4) << j << std::endl;
    }

#ifdef GAME_BUILD
    Hyper::Application app(
        1280, 720, config.name, config.fullscreenOnLaunch, config.resizable,
        false, [&]() {
            // get io
            auto &io = ImGui::GetIO();
            io.ConfigWindowsMoveFromTitleBarOnly = true;

            // io.Fonts->AddFontDefault();
            ImGui::StyleColorsDark();

            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Regular.ttf",
                                         18.f);
            static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA,
                                                   0};
            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", 16.0f,
                                         &icons_config, icons_ranges);
        });

    app.renderOnScreen = true;
#else
    Hyper::Application app(
        1280, 720, "Vault Engine", false, true, false, [&]() {
            // get io
            auto &io = ImGui::GetIO();
            io.ConfigWindowsMoveFromTitleBarOnly = true;
            // io.Fonts->AddFontDefault();
            ImGui::StyleColorsDark();

            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

            io.FontDefault = io.Fonts->AddFontFromFileTTF(
                "assets/fonts/OpenSans-Semibold.ttf", 18.f);
            static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA,
                                                   0};
            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", 16.0f,
                                         &icons_config, icons_ranges);

            // set default font
        });
    auto *fontCascadia = ImGui::GetIO().Fonts->AddFontFromFileTTF(
        "assets/fonts/CascadiaMono.ttf", 16.0f);

    ImGui::GetIO().Fonts->AddFontFromFileTTF(
        "assets/fonts/OpenSans-Semibold.ttf", 18.f);
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    ImGui::GetIO().Fonts->AddFontFromFileTTF(
        "assets/fonts/fa-solid-900.ttf", 55.0f, &icons_config, icons_ranges);
#endif

    Input::window = app.renderer->window;
    // glfw enable sticky mouse buttons
    Shader shader("shaders/default.glsl");
    Shader workerShader("shaders/worker.glsl");
    Shader outlineShader("shaders/outline.glsl");
    // Shader batchShader("shaders/batch.glsl");
    // Shader gridShader("shaders/grid.glsl");

    shader.Bind();
    shader.SetUniform1f("ambient", 0.2);

    Skybox skybox("assets/skybox/right.jpg", "assets/skybox/left.jpg",
                  "assets/skybox/top.jpg", "assets/skybox/bottom.jpg",
                  "assets/skybox/front.jpg", "assets/skybox/back.jpg");
    auto *camera = new Camera(false, app.width, app.height, Vector3(0, 3, 15));
    camera->cam_far = 5000;

#ifndef GAME_BUILD
    Scene::mainCamera = camera;
#endif
    bool focusedOnScene = false;
    bool hoveredScene = false;

    Vector2 winSize = Vector2(0, 0);
    Vector2 winPos = Vector2(0, 0);

    ScriptEngine::window = app.renderer->window;

    bool openConfig = false;
    bool openDetails = false;
    bool openInspector = true;
    bool openLayers = false;
    bool openShaders = false;
    char layerName[32] = "New Layer";

    int inspectorType = InspecType::None;

    ImGuiFileDialog::Instance()->SetFileStyle(
        IGFD_FileStyleByTypeFile, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(
        IGFD_FileStyleByTypeDir, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FOLDER);

    InspectorMaterial m_InspectorMaterial;

    Font::InitFT();

    json M_JS;
    LoadScripts();
    Scene::LoadScene(config.mainScene, M_JS);

#ifdef GAME_BUILD
    if (Scene::mainCamera == nullptr) {
        Scene::mainCamera = camera;
    }
#else
    Scene::mainCamera = camera;
#endif

    Transform transform;
    transform.position = glm::vec3(2, 0, 2);
    transform.rotation = glm::vec3(15, 0, 0);
    transform.scale = glm::vec3(1, 5, 1);
    transform.Update();

    ImVec2 mousePos = ImVec2(0, 0);
    ImVec2 windowPos = ImVec2(0, 0);
    ImVec2 windowSize = ImVec2(0, 0);

    int frameCount = 0;
    float timeElapsed = 0.0f;
    int fps = 0;
    std::string fpsText;

    bool usingImGuizmo = false;

#ifndef GAME_BUILD
    std::vector<Vertex> cubeVertices = {
        // front
        {Vector3(-0.5f, -0.5f, 0.5f), Vector3(0), Vector3(0, 0, 1),
         Vector2(0, 0)},
        {Vector3(0.5f, -0.5f, 0.5f), Vector3(0), Vector3(0, 0, 1),
         Vector2(1, 0)},
        {Vector3(0.5f, 0.5f, 0.5f), Vector3(0), Vector3(0, 0, 1),
         Vector2(1, 1)},
        {Vector3(-0.5f, 0.5f, 0.5f), Vector3(0), Vector3(0, 0, 1),
         Vector2(0, 1)},
        // back
        {Vector3(-0.5f, -0.5f, -0.5f), Vector3(0), Vector3(0, 0, -1),
         Vector2(1, 0)},
        {Vector3(0.5f, -0.5f, -0.5f), Vector3(0), Vector3(0, 0, -1),
         Vector2(0, 0)},
        {Vector3(0.5f, 0.5f, -0.5f), Vector3(0), Vector3(0, 0, -1),
         Vector2(0, 1)},
        {Vector3(-0.5f, 0.5f, -0.5f), Vector3(0), Vector3(0, 0, -1),
         Vector2(1, 1)},
    };

    // indices
    std::vector<uint32_t> cubeIndices = {0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1,
                                         7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4,
                                         4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3};

    // flat plane vertices
    std::vector<Vertex> planeVertices = {
        {Vector3(-0.5f, 0.0f, 0.5f), Vector3(0), Vector3(0, 1, 0),
         Vector2(0, 0)},
        {Vector3(0.5f, 0.0f, 0.5f), Vector3(0), Vector3(0, 1, 0),
         Vector2(1, 0)},
        {Vector3(0.5f, 0.0f, -0.5f), Vector3(0), Vector3(0, 1, 0),
         Vector2(1, 1)},
        {Vector3(-0.5f, 0.0f, -0.5f), Vector3(0), Vector3(0, 1, 0),
         Vector2(0, 1)},
    };

    // indices
    std::vector<uint32_t> planeIndices = {0, 1, 2, 2, 3, 0};

    Material material(Vector4(0, 4, 0.2, 1));
    Mesh CubeCollider(cubeVertices, cubeIndices, material);
    Mesh mesh_BoxCollider2D(planeVertices, planeIndices, material);
    bool drawBoxCollider2D = false;
    glm::vec3 bc2dPos = glm::vec3(0, 0, 0);
    glm::vec3 bc2dScale = glm::vec3(1, 1, 1);
    float bc2dRotation = 0.0f;

    std::vector<Vertex> sprite_vertices = {
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f)}};

    std::vector<uint32_t> sprite_indices = {0, 1, 2, 0, 2, 3};

#ifdef _WIN32
    auto *dirLightIcon_texture = new Texture(
        "assets\\icons\\directional_light.png", 0, "texture_diffuse");
    auto *pointLightIcon_texture =
        new Texture("assets\\icons\\point_light.png", 0, "texture_diffuse");
    auto *spotLightIcon_texture =
        new Texture("assets\\icons\\spot_light.png", 0, "texture_diffuse");
    auto *cameraIcon_texture =
        new Texture("assets\\icons\\camera.png", 0, "texture_diffuse");
    auto *engineLogo = new Texture("build\\logo2.png", 0, "texture_diffuse");
    auto *audioIcon_texture = new Texture("assets\\icons\\audio_icon.png", 0, "texture_diffuse");
#else
    auto *dirLightIcon_texture =
        new Texture("assets/icons/directional_light.png", 0, "texture_diffuse");
    auto *pointLightIcon_texture =
        new Texture("assets/icons/point_light.png", 0, "texture_diffuse");
    auto *spotLightIcon_texture =
        new Texture("assets/icons/spot_light.png", 0, "texture_diffuse");
    auto *cameraIcon_texture =
        new Texture("assets/icons/camera.png", 0, "texture_diffuse");
    auto *engineLogo = new Texture("build/logo2.png", 0, "texture_diffuse");
    auto *audioIcon_texture = new Texture("assets/icons/audio_icon.png", 0, "texture_diffuse");
#endif

    Material dirLightIconMaterial(Vector4(1, 1, 1, 1));
    dirLightIconMaterial.diffuse = dirLightIcon_texture;
    Mesh dirLightIconMesh(sprite_vertices, sprite_indices,
                          dirLightIconMaterial);

    Material pointLightIconMaterial(Vector4(1, 1, 1, 1));
    pointLightIconMaterial.diffuse = pointLightIcon_texture;
    Mesh pointLightIconMesh(sprite_vertices, sprite_indices,
                            pointLightIconMaterial);

    Material spotLightIconMaterial(Vector4(1, 1, 1, 1));
    spotLightIconMaterial.diffuse = spotLightIcon_texture;
    Mesh spotLightIconMesh(sprite_vertices, sprite_indices,
                           spotLightIconMaterial);

    Material cameraIconMaterial(Vector4(1, 1, 1, 1));
    cameraIconMaterial.diffuse = cameraIcon_texture;
    Mesh cameraIconMesh(sprite_vertices, sprite_indices, cameraIconMaterial);

    Material audioIconMaterial(Vector4(1, 1, 1, 1));
    audioIconMaterial.diffuse = audioIcon_texture;
    Mesh audioMesh(sprite_vertices, sprite_indices, audioIconMaterial);

#endif

#ifdef GAME_BUILD
    StartWorld(listener);
    HyperAPI::isRunning = true;
    HyperAPI::isStopped = false;
#endif

    Scene::SceneType sceneType = Scene::MAIN_SCENE;

    glslDef.mName = "GLSL";
    glslDef.mCommentStart = "/*";
    glslDef.mCommentEnd = "*/";
    glslDef.mSingleLineComment = "//";
    glslDef.mCaseSensitive = true;
    glslDef.mAutoIndentation = true;

    TextEditor::Identifier glslId;
    glslId.mDeclaration = "#shader vertex";
    glslDef.mIdentifiers.insert(std::make_pair("Vertex Shader", glslId));
    glslId.mDeclaration = "#shader fragment";
    glslDef.mIdentifiers.insert(std::make_pair("Fragment Shader", glslId));
    glslId.mDeclaration = "#shader geometry";
    glslDef.mIdentifiers.insert(std::make_pair("Geometry Shader", glslId));

    langDef.mName = "Lua";
    langDef.mCommentStart = "--";
    langDef.mCommentEnd = "";
    langDef.mSingleLineComment = "--";
    langDef.mCaseSensitive = true;
    langDef.mAutoIndentation = true;
    // set function
    TextEditor::Identifier id;
    id.mDeclaration = "Log";
    langDef.mIdentifiers.insert(std::make_pair("Log", id));
    id.mDeclaration = "Warning";
    langDef.mIdentifiers.insert(std::make_pair("Warning", id));
    id.mDeclaration = "Error";
    langDef.mIdentifiers.insert(std::make_pair("Error", id));
    id.mDeclaration = "GetComponent";
    langDef.mIdentifiers.insert(std::make_pair("GetComponent", id));
    id.mDeclaration = "UpdateComponent";
    langDef.mIdentifiers.insert(std::make_pair("UpdateComponent", id));
    id.mDeclaration = "HasComponent";
    langDef.mIdentifiers.insert(std::make_pair("HasComponent", id));
    id.mDeclaration = "FindGameObjectByName";
    langDef.mIdentifiers.insert(std::make_pair("FindGameObjectByName", id));
    id.mDeclaration = "FindGameObjectByTag";
    langDef.mIdentifiers.insert(std::make_pair("FindGameObjectByTag", id));
    id.mDeclaration = "FindGameObjectsByName";
    langDef.mIdentifiers.insert(std::make_pair("FindGameObjectsByName", id));
    id.mDeclaration = "FindGameObjectsByTag";
    langDef.mIdentifiers.insert(std::make_pair("FindGameObjectsByTag", id));
    id.mDeclaration = "PlayAudio";
    langDef.mIdentifiers.insert(std::make_pair("PlayAudio", id));
    id.mDeclaration = "StopAudio";
    langDef.mIdentifiers.insert(std::make_pair("StopAudio", id));
    id.mDeclaration = "PlayMusic";
    langDef.mIdentifiers.insert(std::make_pair("PlayMusic", id));
    id.mDeclaration = "StopMusic";
    langDef.mIdentifiers.insert(std::make_pair("StopMusic", id));
    id.mDeclaration = "ToDegrees";
    langDef.mIdentifiers.insert(std::make_pair("ToDegrees", id));
    id.mDeclaration = "ToRadians";
    langDef.mIdentifiers.insert(std::make_pair("ToRadians", id));

    editor.SetTabSize(4);
    editor.SetLanguageDefinition(langDef);
    editor.SetPalette(TextEditor::GetDarkPalette());
    editor.SetShowWhitespaces(false);

#ifndef GAME_BUILD
    // SoundDevice *alDevice = SoundDevice::get();
    // uint32_t audio = SoundBuffer::get()->AddSoundEffect("assets/wrld.mp3");

    // SoundSource speaker;
    // speaker.Play(audio);

    // glm::vec3 list, aud;

    std::function<void(uint32_t & PPT, uint32_t & PPFBO, uint32_t & gui_gui)>
        GUI_EXP = [&](uint32_t &PPT, uint32_t &PPFBO, uint32_t &gui_gui) {
            ShortcutManager(openConfig);

            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Compile C# Assemblies")) {
                        CsharpScriptEngine::CompileAssemblies();
                    }

                    if (ImGui::MenuItem("Reload C# Assemblies")) {
                        bool wasRunning = false;
                        if (HyperAPI::isRunning) {
                            wasRunning = true;
                            HyperAPI::isRunning = false;
                            DeleteWorld();
                        }

                        CsharpScriptEngine::ReloadAssembly();
                        HYPER_LOG("Reloaded C# Assemblies")

                        if (wasRunning) {
                            if (HyperAPI::isStopped) {
                                stateScene = nlohmann::json::array();
                                Scene::SaveScene("", stateScene);
                            }
                            StartWorld(listener);

                            HyperAPI::isRunning = true;
                            HyperAPI::isStopped = false;

                            for (auto &camera : Scene::cameras) {
                                if (camera->mainCamera) {
                                    Scene::mainCamera = camera;
                                    break;
                                }
                            }
                        }
                    }

                    if (ImGui::MenuItem("Create C# Project")) {
                        CsharpScriptEngine::CreateCsharpProject();
                    }

                    if (ImGui::MenuItem("Compile C++ Scripts (Linux)")) {
                        CompileLinuxScripts();
                    }

                    if (ImGui::MenuItem("Compile C++ Scripts (Windows)")) {
                        CompileWindowsScripts();
                    }

                    if (ImGui::MenuItem("Save Scene", "CTRL+S")) {
                        if (Scene::currentScenePath == "") {
                            ImGuiFileDialog::Instance()->OpenDialog(
                                "SaveSceneDialog",
                                ICON_FA_FLOPPY_DISK " Save Scene", ".vault",
                                ".");
                        } else {
                            json S_SJ;
                            Scene::SaveScene(Scene::currentScenePath, S_SJ);
                        }
                    }

                    if (ImGui::MenuItem("Save Scene As", "CTRL+SHIFT+S")) {
                        ImGuiFileDialog::Instance()->OpenDialog(
                            "SaveSceneDialog",
                            ICON_FA_FLOPPY_DISK " Save Scene", ".vault", ".");
                    }

                    if (ImGui::MenuItem("Config", "CTRL+SHIFT+C")) {
                        openConfig = true;
                    }

                    if (ImGui::MenuItem("Build for Linux", "CTRL+B+L")) {
                        ImGuiFileDialog::Instance()->OpenDialog(
                            "BuildLinuxDialog", "Build for Linux", nullptr,
                            ".");
                    }

                    if (ImGui::MenuItem("Build for Windows", "CTRL+B+W")) {
                        ImGuiFileDialog::Instance()->OpenDialog(
                            "BuildWindowsDialog", "Build for Windows", nullptr,
                            ".");
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Tools")) {
                    if (ImGui::MenuItem("Inspector")) {
                        openInspector = true;
                    }

                    if (ImGui::MenuItem("Layers")) {
                        openLayers = true;
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Info")) {
                    if (ImGui::MenuItem("Details")) {
                        openDetails = true;
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Editor")) {
                    if (ImGui::BeginMenu("Themes")) {
                        if (ImGui::MenuItem("Default")) {
                            auto &colors = ImGui::GetStyle().Colors;
                            ImGui::StyleColorsDark();
                            colors[ImGuiCol_WindowBg] =
                                ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

                            colors[ImGuiCol_Header] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_HeaderHovered] =
                                ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
                            colors[ImGuiCol_HeaderActive] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                            colors[ImGuiCol_Button] =
                                ImVec4(0.6f, 0.2f, 0.2f, 1.0f);
                            colors[ImGuiCol_ButtonHovered] =
                                ImVec4(1, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_ButtonActive] =
                                ImVec4(1, 0.305f, 0.3f, 1.0f);

                            colors[ImGuiCol_FrameBg] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_FrameBgHovered] =
                                ImVec4(0.3, 0.305f, 0.3f, 1.0f);
                            colors[ImGuiCol_FrameBgActive] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                            colors[ImGuiCol_Tab] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_TabHovered] =
                                ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
                            colors[ImGuiCol_TabActive] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);
                            colors[ImGuiCol_TabUnfocused] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_TabUnfocusedActive] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                            colors[ImGuiCol_TitleBg] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_TitleBgActive] =
                                ImVec4(0.25f, 0.255f, 0.25f, 1.0f);
                            colors[ImGuiCol_TitleBgCollapsed] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                            colors[ImGuiCol_ResizeGrip] =
                                ImVec4(1, 0.15, 0.15, 1);
                            colors[ImGuiCol_ResizeGripActive] =
                                ImVec4(1, 0.30, 0.30, 1);
                            colors[ImGuiCol_ResizeGripHovered] =
                                ImVec4(1, 0.20, 0.20, 1);
                            colors[ImGuiCol_NavWindowingHighlight] =
                                ImVec4(1, 0.15, 0.15, 1);
                            // text color
                            colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);

                            colors[ImGuiCol_DockingPreview] =
                                ImVec4(1, 0.15, 0.15, 1);
                        }

                        if (ImGui::MenuItem("Blue")) {
                            ImGui::StyleColorsDark();
                        }

                        if (ImGui::MenuItem("Green")) {
                            auto &colors = ImGui::GetStyle().Colors;
                            ImGui::StyleColorsDark();
                            colors[ImGuiCol_WindowBg] =
                                ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

                            colors[ImGuiCol_Header] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_HeaderHovered] =
                                ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
                            colors[ImGuiCol_HeaderActive] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                            colors[ImGuiCol_Button] =
                                ImVec4(0.2f, 0.6f, 0.2f, 1.0f);
                            colors[ImGuiCol_ButtonHovered] =
                                ImVec4(0.2, 1.0f, 0.2f, 1.0f);
                            colors[ImGuiCol_ButtonActive] =
                                ImVec4(0.305, 1.0f, 0.3f, 1.0f);

                            colors[ImGuiCol_FrameBg] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_FrameBgHovered] =
                                ImVec4(0.3, 0.305f, 0.3f, 1.0f);
                            colors[ImGuiCol_FrameBgActive] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                            colors[ImGuiCol_Tab] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_TabHovered] =
                                ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
                            colors[ImGuiCol_TabActive] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);
                            colors[ImGuiCol_TabUnfocused] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_TabUnfocusedActive] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                            colors[ImGuiCol_TitleBg] =
                                ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                            colors[ImGuiCol_TitleBgActive] =
                                ImVec4(0.25f, 0.255f, 0.25f, 1.0f);
                            colors[ImGuiCol_TitleBgCollapsed] =
                                ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                            colors[ImGuiCol_ResizeGrip] =
                                ImVec4(0.15, 1, 0.15, 1);
                            colors[ImGuiCol_ResizeGripActive] =
                                ImVec4(0.30, 1, 0.30, 1);
                            colors[ImGuiCol_ResizeGripHovered] =
                                ImVec4(0.20, 1, 0.20, 1);
                            colors[ImGuiCol_NavWindowingHighlight] =
                                ImVec4(0.15, 1, 0.15, 1);
                            colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);

                            colors[ImGuiCol_DockingPreview] =
                                ImVec4(0.15, 1, 0.15, 1);
                        }

                        if (ImGui::MenuItem("Light")) {
                            ImGui::StyleColorsLight();
                        }

                        if (ImGui::MenuItem("Classic")) {
                            ImGui::StyleColorsClassic();
                        }

                        ImGui::EndMenu();
                    }

                    if (ImGui::MenuItem("Wireframe")) {
                        if (!app.renderer->wireframe) {
                            app.renderer->wireframe = true;
                        } else {
                            app.renderer->wireframe = false;
                        }
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::DockSpaceOverViewport(
                ImGui::GetMainViewport(),
                ImGuiDockNodeFlags_PassthruCentralNode);

            if (openLayers) {
                if (ImGui::Begin(ICON_FA_LAYER_GROUP " Layers", &openLayers)) {
                    if (ImGui::TreeNode("New Layer")) {
                        ImGui::InputText("Name", layerName, 32);
                        if (ImGui::Button("Create")) {
                            std::ofstream file("assets/game.config");
                            Scene::layers[layerName] = true;
                            strcpy(layerName, "New Layer");
                            std::vector<std::string> layerStarters;
                            for (auto &layer : Scene::layers) {
                                layerStarters.push_back(layer.first);
                            }

                            nlohmann::json j = {
                                {"linux_compiler", config.linuxCompiler},
                                {"windows_compiler", config.windowsCompiler},
                                {"name", config.name},
                                {"ambientLight", config.ambientLight},
                                {"exposure", config.exposure},
                                {"mainScene", config.mainScene},
                                {"width", config.width},
                                {"height", config.height},
                                {"resizable", config.resizable},
                                {"fullscreen_on_launch",
                                 config.fullscreenOnLaunch},
                                {"layers", layerStarters},
                                {"post_processing",
                                 {{"enabled", false},
                                  {"bloom",
                                   {{"enabled", false}, {"threshold", 0.5f}}},
                                  {"vignette",
                                   {
                                       {"enabled",
                                        config.postProcessing.enabled},
                                       {"intensity", config.postProcessing
                                                         .vignette.intensity},
                                       {"smoothness", config.postProcessing
                                                          .vignette.smoothness},
                                   }},
                                  {"bloom",
                                   {
                                       {"enabled",
                                        config.postProcessing.enabled},
                                       {"threshold",
                                        config.postProcessing.bloom.threshold},
                                   }},
                                  {"chromatic_aberration",
                                   {
                                       {"intensity",
                                        config.postProcessing
                                            .chromaticAberration.intensity},
                                   }}}}};

                            file << j.dump(4);
                            file.close();
                        }

                        ImGui::TreePop();
                    }
                    ImGui::NewLine();

                    for (auto layer : Scene::layers) {
                        ImGui::PushID(typeid(layer).hash_code());
                        ImGui::Text(((std::string(ICON_FA_PHOTO_FILM)) +
                                     std::string(layer.first))
                                        .c_str());
                        ImGui::PopID();
                    }
                }
                ImGui::End();
            }

            if (openInspector) {
                static std::string matPath;

                if (ImGui::Begin(ICON_FA_MAGNIFYING_GLASS " Inspector",
                                 &openInspector)) {
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload *payload =
                                ImGui::AcceptDragDropPayload("file")) {
                            // const char* path = (const char*)payload->Data;
                            dirPayloadData.erase(0, cwd.length() + 1);

                            if (ends_with(dirPayloadData, ".material")) {
                                inspectorType = InspecType::Material;
                                std::ifstream file(dirPayloadData);
                                matPath = dirPayloadData;
                                nlohmann::json JSON =
                                    nlohmann::json::parse(file);

                                m_InspectorMaterial.diffuse =
                                    JSON["diffuse"] == "nullptr"
                                        ? "None"
                                        : JSON["diffuse"];
                                m_InspectorMaterial.specular =
                                    JSON["specular"] == "nullptr"
                                        ? "None"
                                        : JSON["specular"];
                                m_InspectorMaterial.normal =
                                    JSON["normal"] == "nullptr"
                                        ? "None"
                                        : JSON["normal"];
                                if (JSON.contains("height")) {
                                    m_InspectorMaterial.height =
                                        JSON["height"] == "nullptr"
                                            ? "None"
                                            : JSON["height"];
                                }
                                m_InspectorMaterial.roughness =
                                    JSON["roughness"];
                                m_InspectorMaterial.metallic = JSON["metallic"];
                                m_InspectorMaterial.texUVs.x =
                                    JSON["texUV"]["x"];
                                m_InspectorMaterial.texUVs.y =
                                    JSON["texUV"]["y"];
                                m_InspectorMaterial.baseColor.x =
                                    JSON["baseColor"]["r"];
                                m_InspectorMaterial.baseColor.y =
                                    JSON["baseColor"]["g"];
                                m_InspectorMaterial.baseColor.z =
                                    JSON["baseColor"]["b"];

                                file.close();
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }

                    switch (inspectorType) {
                    case InspecType::Material: {
                        if (ImGui::TreeNode("Diffuse")) {
                            if (ImGui::BeginDragDropTarget()) {
                                if (const ImGuiPayload *payload =
                                        ImGui::AcceptDragDropPayload("file")) {
                                    dirPayloadData.erase(0, cwd.length() + 1);
                                    m_InspectorMaterial.diffuse =
                                        dirPayloadData;
                                }

                                ImGui::EndDragDropTarget();
                            }

                            ImGui::Text(m_InspectorMaterial.diffuse.c_str());
                            if (ImGui::Button(ICON_FA_TRASH
                                              " Remove Texture")) {
                                m_InspectorMaterial.diffuse = "None";
                            }

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Specular")) {
                            if (ImGui::BeginDragDropTarget()) {
                                if (const ImGuiPayload *payload =
                                        ImGui::AcceptDragDropPayload("file")) {
                                    dirPayloadData.erase(0, cwd.length() + 1);
                                    m_InspectorMaterial.specular =
                                        dirPayloadData;
                                }

                                ImGui::EndDragDropTarget();
                            }

                            ImGui::Text(m_InspectorMaterial.specular.c_str());
                            if (ImGui::Button(ICON_FA_TRASH
                                              " Remove Texture")) {
                                m_InspectorMaterial.specular = "None";
                            }

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Normal")) {
                            if (ImGui::BeginDragDropTarget()) {
                                if (const ImGuiPayload *payload =
                                        ImGui::AcceptDragDropPayload("file")) {
                                    dirPayloadData.erase(0, cwd.length() + 1);
                                    m_InspectorMaterial.normal = dirPayloadData;
                                }

                                ImGui::EndDragDropTarget();
                            }

                            ImGui::Text(m_InspectorMaterial.normal.c_str());
                            if (ImGui::Button(ICON_FA_TRASH
                                              " Remove Texture")) {
                                m_InspectorMaterial.normal = "None";
                            }

                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Emission")) {
                            if (ImGui::BeginDragDropTarget()) {
                                if (const ImGuiPayload *payload =
                                        ImGui::AcceptDragDropPayload("file")) {
                                    dirPayloadData.erase(0, cwd.length() + 1);
                                    m_InspectorMaterial.height = dirPayloadData;
                                }

                                ImGui::EndDragDropTarget();
                            }

                            ImGui::Text(m_InspectorMaterial.height.c_str());
                            if (ImGui::Button(ICON_FA_TRASH
                                              " Remove Texture")) {
                                m_InspectorMaterial.height = "None";
                            }

                            ImGui::TreePop();
                        }

                        ImGui::DragFloat2("UV Scale",
                                          &m_InspectorMaterial.texUVs.x, 0.01f);
                        ImGui::DragFloat("Roughness",
                                         &m_InspectorMaterial.roughness, 0.01f,
                                         0.0f, 1.0f);
                        ImGui::DragFloat("Metallic",
                                         &m_InspectorMaterial.metallic, 0.01f,
                                         0.0f, 1.0f);
                        ImGui::ColorEdit4("Color",
                                          &m_InspectorMaterial.baseColor.x);

                        if (ImGui::Button(ICON_FA_FLOPPY_DISK
                                          " Save Material")) {
                            std::ofstream file(matPath);
                            nlohmann::json j = {
                                {"diffuse",
                                 m_InspectorMaterial.diffuse == "None"
                                     ? "nullptr"
                                     : m_InspectorMaterial.diffuse},
                                {"specular",
                                 m_InspectorMaterial.specular == "None"
                                     ? "nullptr"
                                     : m_InspectorMaterial.specular},
                                {"normal", m_InspectorMaterial.normal == "None"
                                               ? "nullptr"
                                               : m_InspectorMaterial.normal},
                                {"height", m_InspectorMaterial.height == "None"
                                               ? "nullptr"
                                               : m_InspectorMaterial.height},
                                {"roughness", m_InspectorMaterial.roughness},
                                {"metallic", m_InspectorMaterial.metallic},
                                {"baseColor",
                                 {
                                     {"r", m_InspectorMaterial.baseColor.x},
                                     {"g", m_InspectorMaterial.baseColor.y},
                                     {"b", m_InspectorMaterial.baseColor.z},
                                     {"a", m_InspectorMaterial.baseColor.w},
                                 }},
                                {"texUV",
                                 {{"x", m_InspectorMaterial.texUVs.x},
                                  {"y", m_InspectorMaterial.texUVs.y}}}};

                            file << j.dump(4);
                        }
                        ImGui::NewLine();

                        if (ImGuiFileDialog::Instance()->Display(
                                "SaveMaterialDialog")) {
                            // action if OK
                            if (ImGuiFileDialog::Instance()->IsOk()) {
                                std::string filePathName =
                                    ImGuiFileDialog::Instance()
                                        ->GetFilePathName();
                                // remove cwd from filePathName
                                filePathName.erase(0, cwd.length() + 1);
                                std::string filePath =
                                    ImGuiFileDialog::Instance()
                                        ->GetCurrentPath();

                                std::ofstream file(filePathName);
                                nlohmann::json j = {
                                    {"diffuse",
                                     m_InspectorMaterial.diffuse == "None"
                                         ? "nullptr"
                                         : m_InspectorMaterial.diffuse},
                                    {"specular",
                                     m_InspectorMaterial.specular == "None"
                                         ? "nullptr"
                                         : m_InspectorMaterial.specular},
                                    {"normal",
                                     m_InspectorMaterial.normal == "None"
                                         ? "nullptr"
                                         : m_InspectorMaterial.normal},
                                    {"roughness",
                                     m_InspectorMaterial.roughness},
                                    {"metallic", m_InspectorMaterial.metallic},
                                    {"baseColor",
                                     {
                                         {"r", m_InspectorMaterial.baseColor.x},
                                         {"g", m_InspectorMaterial.baseColor.y},
                                         {"b", m_InspectorMaterial.baseColor.z},
                                         {"a", m_InspectorMaterial.baseColor.w},
                                     }},
                                    {"texUV",
                                     {{"x", m_InspectorMaterial.texUVs.x},
                                      {"y", m_InspectorMaterial.texUVs.y}}}};

                                file << j.dump(4);
                            }

                            ImGuiFileDialog::Instance()->Close();
                        }

                        break;
                    }
                    }

                    if (inspectorType == InspecType::None) {
                        ImGui::TextWrapped("To inspect an object, drag it into "
                                           "the inspector window's title bar.");
                    }
                }
                ImGui::End();
            }

            if (ImGuiFileDialog::Instance()->Display("SaveSceneDialog")) {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string filePathName =
                        ImGuiFileDialog::Instance()->GetFilePathName();
                    // remove cwd from filePathName
                    filePathName.erase(0, cwd.length() + 1);
                    std::string filePath =
                        ImGuiFileDialog::Instance()->GetCurrentPath();

                    json S_SJ;
                    Scene::SaveScene(filePathName, S_SJ);
                }

                ImGuiFileDialog::Instance()->Close();
            }

            if (openConfig) {
                ImGui::OpenPopup("Edit Config");
                openConfig = false;
            }

            ImGui::SetNextWindowSize(ImVec2(500, 0));
            if (ImGui::BeginPopup("Edit Config")) {
#ifndef _WIN32 || GAME_BUILD
                UpdatePresence("In Editor", "Editing Configurations");
#endif
                ImGui::InputText("Linux Compiler", config.linuxCompiler, 500);
                ImGui::InputText("Windows Compiler", config.windowsCompiler,
                                 500);

                ImGui::InputText("Game Name", config.name, 500);
                ImGui::DragFloat("Ambient Lightning", &config.ambientLight,
                                 0.01f, 0, 1);
                ImGui::DragFloat("Exposure", &config.exposure, 0.01f, 0);
                ImGui::Checkbox("Fullscreen On Launch",
                                &config.fullscreenOnLaunch);
                ImGui::Checkbox("Resizable", &config.resizable);
                ImGui::DragInt("Width", &config.width, 1, 0, 1920);
                ImGui::DragInt("Height", &config.height, 1, 0, 1080);
                // ImGui::DragFloat("Grid Size", &m_grid_size, 5, 1);
                // ImGui::Checkbox("Draw Grid", &drawGrid);

                if (ImGui::TreeNode("Post Processing")) {
                    ImGui::Checkbox("Enabled", &config.postProcessing.enabled);

                    if (ImGui::TreeNode("Chromatic Aberration")) {
                        ImGui::DragFloat("Strength",
                                         &config.postProcessing
                                              .chromaticAberration.intensity,
                                         0.001f, 0);
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Bloom")) {
                        ImGui::Checkbox("Enabled",
                                        &config.postProcessing.bloom.enabled);
                        ImGui::DragFloat("Threshold",
                                         &config.postProcessing.bloom.threshold,
                                         0.001f, 0);
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Vignette")) {
                        ImGui::DragFloat(
                            "Strength",
                            &config.postProcessing.vignette.intensity, 0.01f,
                            0);
                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }

                if (ImGui::Button("Main Scene", ImVec2(500, 0))) {
                    ImGuiFileDialog::Instance()->OpenDialog(
                        "ChooseMainScene", "Choose Main Scene", ".vault", ".");
#ifndef _WIN32 || GAME_BUILD
                    UpdatePresence("In Editor", "Making a game");
#endif
                    // ImGui::CloseCurrentPopup();
                }
                ImGui::Separator();
                if (ImGui::Button("Save", ImVec2(500, 0))) {
                    std::vector<std::string> layerStarters;
                    for (auto &layer : Scene::layers) {
                        layerStarters.push_back(layer.first);
                    }

                    nlohmann::json j = {
                        {"linux_compiler", config.linuxCompiler},
                        {"windows_compiler", config.windowsCompiler},
                        {"name", config.name},
                        {"ambientLight", config.ambientLight},
                        {"exposure", config.exposure},
                        {"mainScene", config.mainScene},
                        {"resizable", config.resizable},
                        {"fullscreen_on_launch", config.fullscreenOnLaunch},
                        {"width", config.width},
                        {"height", config.height},
                        {"layers", layerStarters},
                        {"post_processing",
                         {{"enabled", false},
                          {"bloom", {{"enabled", false}, {"threshold", 0.5f}}},
                          {"vignette",
                           {
                               {"enabled", config.postProcessing.enabled},
                               {"intensity",
                                config.postProcessing.vignette.intensity},
                               {"smoothness",
                                config.postProcessing.vignette.smoothness},
                           }},
                          {"bloom",
                           {
                               {"enabled", config.postProcessing.enabled},
                               {"threshold",
                                config.postProcessing.bloom.threshold},
                           }},
                          {"chromatic_aberration",
                           {
                               {"intensity",
                                config.postProcessing.chromaticAberration
                                    .intensity},
                           }}}}};

                    std::ofstream o("assets/game.config");
                    o << std::setw(4) << j << std::endl;

#ifndef _WIN32 || GAME_BUILD
                    UpdatePresence("In Editor", "Making a game");
#endif

                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGuiFileDialog::Instance()->Display("ChooseMainScene")) {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string filePathName =
                        ImGuiFileDialog::Instance()->GetFilePathName();
                    // remove cwd from filePathName
                    filePathName.erase(0, cwd.length() + 1);
                    std::string filePath =
                        ImGuiFileDialog::Instance()->GetCurrentPath();

                    config.mainScene = filePathName;
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            // docking with mutli-viewport
            if (openDetails) {
                if (ImGui::Begin("Details", &openDetails)) {
                    ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
                    ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
                    ImGui::Text("Version: %s", glGetString(GL_VERSION));
                    ImGui::Text("Shading Language Version: %s",
                                glGetString(GL_SHADING_LANGUAGE_VERSION));
                    ImGui::Text("%s", fpsText.c_str());
                    auto vram = app.renderer->GetVRamUsage();
                    ImGui::Text("VRAM: %d MB / %d MB", vram.first, vram.second);

#ifndef _WIN32 || GAME_BUILD
                    UpdatePresence("In Editor", "Checking Details");
#endif
                    ImGui::End();
                }
            }

            if (ImGui::Begin(ICON_FA_CODE " Text Editor")) {
                if (ImGui::IsWindowHovered()) {
                    editingText = true;
                }

                ImGui::PushFont(fontCascadia);
                editor.Render("Text Editor");
                ImGui::PopFont();
            } else {
                editingText = false;
            }
            ImGui::End();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            if (ImGui::Begin(ICON_FA_GAMEPAD " Scene", nullptr,
                             ImGuiWindowFlags_NoScrollbar |
                                 ImGuiWindowFlags_NoScrollWithMouse)) {

                ImVec2 m_size = ImGui::GetWindowSize();
                float viewManipulateRight = ImGui::GetWindowPos().x + m_size.x;
                float viewManipulateTop = ImGui::GetWindowPos().y + m_size.y;
                static glm::mat4 m_cube = glm::mat4(1.0f);
                // ImGuizmo::ViewManipulate(
                //     &Scene::mainCamera->view[0][0],
                //     8.0f, ImVec2(viewManipulateRight - 128, viewManipulateTop - 128),
                //     ImVec2(128, 128),
                //     0x10101010);

                bool complete =
                    Scene::DropTargetMat(Scene::DRAG_SCENE, nullptr, nullptr);
                ImVec2 w_s = ImGui::GetWindowSize();
                winSize = Vector2(w_s.x, w_s.y);
                sceneType = Scene::MAIN_SCENE;

                //                    ImGui::BeginChild("View");
                if (sceneType == Scene::MAIN_SCENE) {
                    mousePos = ImGui::GetMousePos();
                    windowPos = ImGui::GetWindowPos();
                    windowSize = ImGui::GetWindowSize();
                    mousePos.x -= windowPos.x;
                    mousePos.y -= windowPos.y;
                    mousePos.y = windowSize.y - mousePos.y;
                    app.sceneMouseX = mousePos.x;
                    app.sceneMouseY = mousePos.y;

                    app.width = windowSize.x;
                    app.height = windowSize.y;

                    ImVec2 w_p = ImGui::GetWindowPos();
                    Input::winPos = Vector3(w_p.x, w_p.y, 0);

                    glActiveTexture(GL_TEXTURE15);
                    glBindTexture(GL_TEXTURE_2D, PPT);

                    ImGui::Image((void *)PPT, ImVec2(w_s.x, w_s.y),
                                 ImVec2(0, 1), ImVec2(1, 0));
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0)) {
                        focusedOnScene = true;
                    } else if (!ImGui::IsMouseDragging(0)) {
                        focusedOnScene = false;
                    }

                    if (ImGui::IsItemHovered()) {
                        hoveredScene = true;
                    } else if (!ImGui::IsMouseDragging(0)) {
                        hoveredScene = false;
                    }
                }

                auto [viewWidth, viewHeight] = ImGui::GetWindowSize();
                auto [viewX, viewY] = ImGui::GetWindowPos();
                // m_Viewbounds is winPos
                auto [mouseX, mouseY] = ImGui::GetMousePos();
                mouseX -= viewX;
                mouseY -= viewY;
                mouseY = (int)viewHeight - mouseY;
                int m_mouseX = (int)mouseX;
                int m_mouseY = (int)mouseY;
                sceneMouseX = m_mouseX;
                sceneMouseY = m_mouseY;

                app.sceneMouseX = m_mouseX;
                app.sceneMouseY = m_mouseY;

                ImGuizmo::SetOrthographic(Scene::mainCamera->mode2D);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(viewX, viewY, viewWidth, viewHeight);

                auto selectedObject = Scene::m_Object;
                // ImGuizmo::ViewManipulate(glm::value_ptr(Scene::mainCamera->view), )
                //                    m_GuizmoMode = ImGuizmo::OPERATION::ROTATE
                // if (Scene::mainCamera != camera) m_GuizmoMode = -1;
                if (m_GuizmoMode == -1)
                    m_GuizmoMode = ImGuizmo::OPERATION::TRANSLATE;
                if (selectedObject && m_GuizmoMode != -1) {
                    // check if ImGuizmo is hovered

                    glm::mat4 view = Scene::mainCamera->view;
                    glm::mat4 projection = Scene::mainCamera->projection;

                    auto &transform = selectedObject->GetComponent<Transform>();
                    transform.Update();
                    glm::mat4 transformMat = transform.transform;
                    glm::vec3 originalRot = transform.rotation;
                    glm::vec3 originalScal = transform.scale;

                    bool snap = Input::IsKeyPressed(KEY_LEFT_CONTROL);
                    float snapValue = 0.5f;

                    if (m_GuizmoMode == ImGuizmo::OPERATION::ROTATE)
                        snapValue = 45.0f;

                    float snapValues[3] = {snapValue, snapValue, snapValue};

                    ImGuizmo::Manipulate(
                        glm::value_ptr(view), glm::value_ptr(projection),
                        (ImGuizmo::OPERATION)m_GuizmoMode, (ImGuizmo::MODE)m_GuizmoWorld,
                        glm::value_ptr(transformMat), nullptr,
                        snap ? snapValues : nullptr,
                        m_GuizmoMode == ImGuizmo::BOUNDS ? bounds : nullptr,
                        snap ? boundsSnap : nullptr);

                    if (ImGuizmo::IsOver()) {
                        usingImGuizmo = true;
                    } else {
                        usingImGuizmo = false;
                    }

                    if (ImGuizmo::IsUsing()) {

                        glm::vec3 translation, rotation, scale;
                        DecomposeTransform(transformMat, translation, rotation,
                                           scale);

                        //                            transform.rotation =
                        //                            rotation; transform.scale
                        //                            = scale;
                        switch (m_GuizmoMode) {
                        case ImGuizmo::OPERATION::TRANSLATE: {
                            transform.position = translation;

                            if (HyperAPI::isRunning) {
                                if (selectedObject
                                        ->HasComponent<Rigidbody2D>()) {
                                    auto &rigidbody =
                                        selectedObject
                                            ->GetComponent<Rigidbody2D>();

                                    b2Body *body = (b2Body *)rigidbody.body;
                                    body->SetTransform(
                                        b2Vec2(translation.x, translation.y),
                                        body->GetAngle());
                                }

                                if (selectedObject
                                        ->HasComponent<Rigidbody3D>()) {
                                    auto &rigidbody =
                                        selectedObject
                                            ->GetComponent<Rigidbody3D>();

                                    btRigidBody *body =
                                        (btRigidBody *)rigidbody.body;
                                    body->getWorldTransform().setOrigin(
                                        btVector3(translation.x, translation.y,
                                                  translation.z));
                                    body->getWorldTransform().setRotation(
                                        btQuaternion(rotation.x, rotation.y,
                                                     rotation.z));
                                }
                            }
                            break;
                        }
                        case ImGuizmo::OPERATION::ROTATE: {
                            glm::vec3 deltaRot = rotation - originalRot;
                            transform.rotation += deltaRot;

                            if (HyperAPI::isRunning) {
                                if (selectedObject
                                        ->HasComponent<Rigidbody2D>()) {
                                    auto &rigidbody =
                                        selectedObject
                                            ->GetComponent<Rigidbody2D>();

                                    b2Body *body = (b2Body *)rigidbody.body;
                                    body->SetTransform(
                                        body->GetPosition(),
                                        body->GetAngle() +
                                            glm::radians(deltaRot.z));
                                }

                                if (selectedObject
                                        ->HasComponent<Rigidbody3D>()) {
                                    auto &rigidbody =
                                        selectedObject
                                            ->GetComponent<Rigidbody3D>();

                                    btRigidBody *body =
                                        (btRigidBody *)rigidbody.body;
                                    btQuaternion rot =
                                        body->getWorldTransform().getRotation();
                                    rot.setEulerZYX(glm::radians(deltaRot.z),
                                                    glm::radians(deltaRot.y),
                                                    glm::radians(deltaRot.x));
                                    body->getWorldTransform().setRotation(rot);
                                }
                            }
                            break;
                        }
                        case ImGuizmo::OPERATION::SCALE: {
                            transform.scale =
                                Vector3(scale.x * 2, scale.y * 2, scale.z * 2);
                            break;
                        }
                        case ImGuizmo::OPERATION::BOUNDS: {
                            transform.position = translation;
                            transform.scale =
                                Vector3(scale.x * 2, scale.y * 2, scale.z * 2);
                            break;
                        }
                        }
                    }
                }

                if (Scene::LoadingScene) {
                    Scene::m_Object = nullptr;
                }

                if (complete) {
                    Scene::mainCamera = nullptr;
                }

                if (Scene::mainCamera == camera && drawGrid) {
                    ImGuizmo::DrawGrid(glm::value_ptr(Scene::mainCamera->view), glm::value_ptr(Scene::mainCamera->projection), glm::value_ptr(glm::mat4(1.0f)), m_grid_size);
                }

                ImGui::SetCursorPosY(28);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

                ImGui::PushStyleColor(
                    ImGuiCol_ChildBg,
                    (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f, 0.6f));
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.5f);
                ImGui::BeginChild("##Gizmo", ImVec2(97 + 34, 32));
                ImVec2 gizmoSize = ImGui::GetWindowSize();
                ImVec2 centerCalc = ImGui::GetCursorPos();
                centerCalc.x += gizmoSize.x / 2;
                centerCalc.y += gizmoSize.y / 2;

                float objOffset = 24;
                float offset = 34;
                float blockSize = 32;

                ImGui::SetCursorPos(
                    ImVec2(7, centerCalc.y - 12));
                if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT)) {
                    m_GuizmoMode = ImGuizmo::OPERATION::TRANSLATE;
                }
                if (ImGui::IsItemHovered()) {
                    focusedOnScene = false;
                    hoveredScene = false;
                }
                ImGui::SameLine();
                ImGui::SetCursorPos(
                    ImVec2(7 + (blockSize), centerCalc.y - 12));
                if (ImGui::Button(ICON_FA_ARROWS_ROTATE)) {
                    m_GuizmoMode = ImGuizmo::OPERATION::ROTATE;
                }

                ImGui::SameLine();
                ImGui::SetCursorPos(
                    ImVec2(7 + (blockSize * 2), centerCalc.y - 12));
                if (ImGui::Button(ICON_FA_MAXIMIZE)) {
                    m_GuizmoMode = ImGuizmo::OPERATION::SCALE;
                }

                ImGui::SameLine();
                ImGui::SetCursorPos(
                    ImVec2(7 + (blockSize * 3), centerCalc.y - 12));
                if (ImGui::Button(ICON_FA_SQUARE)) {
                    m_GuizmoMode = ImGuizmo::OPERATION::BOUNDS;
                }

                if (ImGui::IsItemHovered()) {
                    focusedOnScene = false;
                    hoveredScene = false;
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();

                {
                    ImGui::SetCursorPosY((28 * 2) + 10);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

                    ImGui::PushStyleColor(
                        ImGuiCol_ChildBg,
                        (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f, 0.6f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.5f);
                    ImGui::BeginChild("##Gizmo_Mode", ImVec2(97 / 1.4, 32));
                    ImVec2 gizmoSize = ImGui::GetWindowSize();
                    ImVec2 centerCalc = ImGui::GetCursorPos();
                    centerCalc.x += gizmoSize.x / 2;
                    centerCalc.y += gizmoSize.y / 2;

                    float offset = 30;

                    ImGui::SetCursorPos(
                        ImVec2(centerCalc.x + 2 - offset, centerCalc.y - 12));
                    if (ImGui::Button(ICON_FA_GLOBE)) {
                        m_GuizmoWorld = ImGuizmo::MODE::WORLD;
                    }
                    if (ImGui::IsItemHovered()) {
                        focusedOnScene = false;
                        hoveredScene = false;
                    }
                    ImGui::SameLine();
                    ImGui::SetCursorPos(
                        ImVec2(centerCalc.x - 26 + offset, centerCalc.y - 12));
                    if (ImGui::Button(ICON_FA_SQUARE)) {
                        m_GuizmoWorld = ImGuizmo::MODE::LOCAL;
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();
                }

                ImGui::SameLine();

                if (!HyperAPI::isRunning) {
                    ImGui::SetCursorPosX(
                        (HyperAPI::isStopped
                             ? (ImGui::GetWindowSize().x / 2) - (32 / 2)
                             : (ImGui::GetWindowSize().x / 2) - (32 / 2) -
                                   (32 / 2) - 5));
                    ImGui::SetCursorPosY(28);

                    ImVec4 buttonColor =
                        ImGui::GetStyle().Colors[ImGuiCol_Button];
                    ImGui::PushStyleColor(ImGuiCol_Button,
                                          ImVec4(buttonColor.x, buttonColor.y,
                                                 buttonColor.z, 0.7f));
                    if (ImGui::Button(ICON_FA_PLAY, ImVec2(32, 32))) {
                        // CsharpScriptEngine::ReloadAssembly();
                        if (HyperAPI::isStopped) {
                            stateScene = nlohmann::json::array();
                            Scene::SaveScene("", stateScene);
                        }
                        StartWorld(listener);

                        HyperAPI::isRunning = true;
                        HyperAPI::isStopped = false;

                        for (auto &camera : Scene::cameras) {
                            if (camera->mainCamera) {
                                Scene::mainCamera = camera;
                                break;
                            }
                        }
                    }
                    ImGui::PopStyleColor();

                    if (ImGui::IsItemHovered()) {
                        focusedOnScene = false;
                        hoveredScene = false;
                    }
                } else if (!HyperAPI::isStopped) {
                    ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) -
                                         (32 / 2) - (32 / 2) - 5);
                    ImGui::SetCursorPosY(28);

                    ImVec4 buttonColor =
                        ImGui::GetStyle().Colors[ImGuiCol_Button];
                    ImGui::PushStyleColor(ImGuiCol_Button,
                                          ImVec4(buttonColor.x, buttonColor.y,
                                                 buttonColor.z, 0.7f));
                    if (ImGui::Button(ICON_FA_PAUSE, ImVec2(32, 32))) {
                        HyperAPI::isRunning = false;
                        DeleteWorld();

                        Scene::mainCamera = camera;
                    }
                    ImGui::PopStyleColor();

                    if (ImGui::IsItemHovered()) {
                        focusedOnScene = false;
                        hoveredScene = false;
                    }
                }

                if (!HyperAPI::isStopped) {
                    ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) -
                                         (32 / 2) + (32 / 2) + 5);
                    ImGui::SetCursorPosY(28);

                    ImVec4 buttonColor =
                        ImGui::GetStyle().Colors[ImGuiCol_Button];
                    ImGui::PushStyleColor(ImGuiCol_Button,
                                          ImVec4(buttonColor.x, buttonColor.y,
                                                 buttonColor.z, 0.7f));
                    if (ImGui::Button(ICON_FA_STOP, ImVec2(32, 32))) {
                        if (HyperAPI::isRunning) {
                            DeleteWorld();
                        }
                        HyperAPI::isRunning = false;
                        HyperAPI::isStopped = true;
                        Scene::m_Object = nullptr;
                        // nlohmann json to string
                        std::string stateJSON = stateScene.dump(4);
                        if (stateJSON != "[]") {
                            Scene::LoadScene("", stateScene);
                        }
                        stateScene.clear();
                        Scene::mainCamera = camera;
                    }
                    ImGui::PopStyleColor();

                    if (ImGui::IsItemHovered()) {
                        focusedOnScene = false;
                        hoveredScene = false;
                    }
                }

                ImGui::SameLine();

                if (camera->mode2D) {
                    ImGui::SetCursorPosY(28);
                    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 85 - 10);
                    ImVec4 buttonColor =
                        ImGui::GetStyle().Colors[ImGuiCol_Button];
                    ImGui::PushStyleColor(ImGuiCol_Button,
                                          ImVec4(buttonColor.x, buttonColor.y,
                                                 buttonColor.z, 0.7f));
                    if (ImGui::Button(ICON_FA_CAMERA " 3D View",
                                      ImVec2(85, 32))) {
                        camera->mode2D = false;
                    }
                    ImGui::PopStyleColor();
                    if (ImGui::IsItemHovered()) {
                        focusedOnScene = false;
                        hoveredScene = false;
                    }
                } else {
                    ImGui::SetCursorPosY(28);
                    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 85 - 10);
                    ImVec4 buttonColor =
                        ImGui::GetStyle().Colors[ImGuiCol_Button];
                    ImGui::PushStyleColor(ImGuiCol_Button,
                                          ImVec4(buttonColor.x, buttonColor.y,
                                                 buttonColor.z, 0.7f));
                    if (ImGui::Button(ICON_FA_CAMERA " 2D View",
                                      ImVec2(85, 32))) {
                        camera->mode2D = true;
                    }
                    ImGui::PopStyleColor();

                    if (ImGui::IsItemHovered()) {
                        focusedOnScene = false;
                        hoveredScene = false;
                    }
                }

                ImGui::SetCursorPos(ImVec2(0, 0));
                ImGui::Text("FPS: %d", fps);
            }
            ImGui::End();
            ImGui::PopStyleVar();

            if (ImGui::Begin(ICON_FA_CUBES " Hierarchy")) {
                if (ImGui::IsWindowHovered()) {
                    if (ImGui::IsMouseClicked(1)) {
                        ImGui::OpenPopup("Context Menu");
                    }
                }

                if (ImGui::BeginPopup("Context Menu")) {
                    if (ImGui::Button(ICON_FA_PLUS " Add GameObject", ImVec2(200, 25))) {
                        GameObject *go = new GameObject();
                        go->AddComponent<Transform>();
                        Scene::m_GameObjects.push_back(go);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("game_object")) {
                        for (auto &gameObject : Scene::m_GameObjects) {
                            if (gameObject->ID == HyperAPI::dirPayloadData) {
                                if (gameObject->HasComponent<Transform>()) {
                                    auto &transform = gameObject->GetComponent<Transform>();

                                    transform.parentTransform = nullptr;
                                }

                                gameObject->parentID = "NO_PARENT";
                                break;
                            }
                        }
                    }

                    ImGui::EndDragDropTarget();
                }
                Scene::DropTargetMat(Scene::DRAG_MODEL, nullptr, nullptr);
                ImVec2 win_size = ImGui::GetWindowSize();

                // if (ImGui::Button(ICON_FA_FOLDER " Add Folder", ImVec2(win_size.x - 15, 25))) {
                //     GameObject *go = new GameObject();
                //     go->AddComponent<Transform>();
                //     Scene::m_GameObjects.push_back(go);
                // }
                // ImGui::Columns(3);
                // // ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 16, ImGui::GetCursorPos().y));
                // ImGui::Text("Label");
                // ImGui::NextColumn();
                // ImGui::Text("Type");
                // ImGui::NextColumn();
                // ImGui::Text("Active");
                // ImGui::Columns();

                // ImGui::Columns(3);
                for (int i = 0; i < Scene::m_GameObjects.size(); i++) {
                    if (Scene::m_GameObjects[i]->parentID != "NO_PARENT") {
                        bool parentFound;
                        for (auto &gameObject : Scene::m_GameObjects) {
                            if (gameObject->ID ==
                                Scene::m_GameObjects[i]->parentID) {
                                parentFound = true;
                                break;
                            } else {
                                parentFound = false;
                            }
                        }

                        if (!parentFound) {
                            Scene::m_GameObjects[i]->parentID = "NO_PARENT";

                            if (Scene::m_GameObjects[i]
                                    ->HasComponent<Transform>()) {
                                auto &transform =
                                    Scene::m_GameObjects[i]
                                        ->GetComponent<Transform>();
                                transform.parentTransform = nullptr;
                            }
                        }
                        continue;
                    }
                    Scene::m_GameObjects[i]->GUI();
                }
                // ImGui::NextColumn();
                // for (auto *go : Scene::m_GameObjects) {
                //     if (go->type == "Prefab") {
                //         ImVec4 col(0.1294, 0.58039, 1, 1);
                //         ImGui::TextColored(col, "%s", go->type.c_str());
                //     } else {
                //         ImGui::Text("%s", go->type.c_str());
                //     }
                // }
                // ImGui::NextColumn();
                // for (auto *go : Scene::m_GameObjects) {
                //     if (go->enabled) {
                //         ImGui::Text("True");
                //     } else {
                //         ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "False");
                //     }
                // }
                // ImGui::Columns();

                if (ImGui::IsWindowHovered() &&
                    ImGui::IsMouseDoubleClicked(0)) {
                    Scene::m_Object = nullptr;
                }
            }
            ImGui::End();
            if (ImGui::Begin(ICON_FA_SHARE_NODES " Components")) {
                if (Scene::m_Object != nullptr && !Scene::LoadingScene) {
                    if (ImGui::IsWindowFocused()) {
#ifndef _WIN32 || GAME_BUILD
                        UpdatePresence("In Editor",
                                       "Editing " + Scene::m_Object->name);
#endif
                    }
                    ImGui::InputText("Name", Scene::name, 500);
                    ImGui::InputText("Tag", Scene::tag, 500);
                    ImGui::Checkbox("Active", &Scene::m_Object->enabled);
                    // layer items
                    std::vector<const char *> layerItems;
                    for (auto &layer : Scene::layers) {
                        layerItems.push_back(layer.first.c_str());
                    }

                    if (ImGui::BeginCombo("Layer",
                                          Scene::m_Object->layer.c_str())) {
                        for (int i = 0; i < layerItems.size(); i++) {
                            bool isSelected = (Scene::layer == layerItems[i]);
                            if (ImGui::Selectable(layerItems[i], isSelected)) {
                                Scene::m_Object->layer = layerItems[i];
                                strcpy(Scene::layer, layerItems[i]);
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    Scene::m_Object->tag = Scene::tag;
                    Scene::m_Object->name = Scene::name;
                    Scene::m_Object->layer = Scene::layer;

                    if (Scene::m_Object->HasComponent<Transform>()) {
                        auto &comp = Scene::m_Object->GetComponent<Transform>();
                        if (comp.hasGUI)
                            comp.GUI();

                        if (Scene::m_Object->HasComponent<Rigidbody2D>() &&
                            HyperAPI::isRunning) {
                            auto &rigidbody =
                                Scene::m_Object->GetComponent<Rigidbody2D>();
                            b2Body *body = (b2Body *)rigidbody.body;
                            body->SetTransform(
                                b2Vec2(comp.position.x, comp.position.y),
                                body->GetAngle());
                        }

                        if (Scene::m_Object->HasComponent<Rigidbody3D>() &&
                            HyperAPI::isRunning) {
                            auto &rigidbody =
                                Scene::m_Object->GetComponent<Rigidbody3D>();
                            btRigidBody *body = (btRigidBody *)rigidbody.body;
                            body->getWorldTransform().setOrigin(
                                btVector3(comp.position.x, comp.position.y,
                                          comp.position.z));
                        }
                    }

                    if (Scene::m_Object->HasComponent<CameraComponent>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<CameraComponent>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<MeshRenderer>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<MeshRenderer>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<m_LuaScriptComponent>()) {
                        auto &comp = Scene::m_Object
                                         ->GetComponent<m_LuaScriptComponent>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<CppScriptManager>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<CppScriptManager>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<CsharpScriptManager>()) {
                        auto &comp = Scene::m_Object
                                         ->GetComponent<CsharpScriptManager>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<Audio3D>()) {
                        auto &comp = Scene::m_Object->GetComponent<Audio3D>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<AudioListener>()) {
                        auto &comp = Scene::m_Object->GetComponent<AudioListener>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<c_PointLight>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<c_PointLight>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<c_Light2D>()) {
                        auto &comp = Scene::m_Object->GetComponent<c_Light2D>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<c_SpotLight>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<c_SpotLight>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<c_DirectionalLight>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<c_DirectionalLight>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<SpriteRenderer>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<SpriteRenderer>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<SpriteAnimation>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<SpriteAnimation>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<Text3D>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<Text3D>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object
                            ->HasComponent<c_SpritesheetAnimation>()) {
                        auto &comp =
                            Scene::m_Object
                                ->GetComponent<c_SpritesheetAnimation>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<SpritesheetRenderer>()) {
                        auto &comp = Scene::m_Object
                                         ->GetComponent<SpritesheetRenderer>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<BoxCollider2D>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<BoxCollider2D>();
                        if (comp.hasGUI) {
                            comp.GUI();
                            if (ImGui::IsWindowFocused()) {
                                drawBoxCollider2D = true;
                            }
                            auto &transform =
                                Scene::m_Object->GetComponent<Transform>();
                            bc2dPos = transform.position;
                            bc2dRotation = transform.rotation.z;
                            bc2dScale = Vector3(comp.size.x, comp.size.y, 1);
                        }
                    } else {
                        drawBoxCollider2D = false;
                    }

                    if (Scene::m_Object->HasComponent<Rigidbody2D>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<Rigidbody2D>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<Rigidbody3D>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<Rigidbody3D>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<FixedJoint3D>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<FixedJoint3D>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<BoxCollider3D>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<BoxCollider3D>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<MeshCollider3D>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<MeshCollider3D>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<Bloom>()) {
                        auto &comp = Scene::m_Object->GetComponent<Bloom>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    if (Scene::m_Object->HasComponent<PathfindingAI>()) {
                        auto &comp =
                            Scene::m_Object->GetComponent<PathfindingAI>();
                        if (comp.hasGUI)
                            comp.GUI();
                    }

                    ImGui::Separator();

                    ImVec2 win_size = ImGui::GetWindowSize();
                    if (!HyperAPI::isRunning) {
                        if (ImGui::Button(ICON_FA_PLUS " Add Component",
                                          ImVec2(win_size.x - 15, 0))) {
                            ImGui::OpenPopup("Add Component");
                        }
                    }
                }

                if (ImGui::BeginPopup("Add Component")) {
                    if (ImGui::Button("Transform", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<Transform>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Mesh Renderer", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<MeshRenderer>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Lua Scripts", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<m_LuaScriptComponent>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("C++ Scripts", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<CppScriptManager>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("C# Scripts", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<CsharpScriptManager>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Camera", ImVec2(200, 0))) {
                        // CameraComponent has one argument of type entt::entity
                        Scene::m_Object->AddComponent<CameraComponent>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("3D Audio", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<Audio3D>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Audio Listener", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<AudioListener>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Point Light", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<c_PointLight>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Spot Light", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<c_SpotLight>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Directional Light", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<c_DirectionalLight>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("2D Light", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<c_Light2D>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Sprite Renderer", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<SpriteRenderer>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("3D Text", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<Text3D>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Sprite Animation", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<SpriteAnimation>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Spritesheet Renderer", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<SpritesheetRenderer>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Spritesheet Animation",
                                      ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<c_SpritesheetAnimation>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Rigidbody 2D", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<Rigidbody2D>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Box Collider 2D", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<BoxCollider2D>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Rigidbody 3D", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<Rigidbody3D>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Fixed Joint 3D", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<FixedJoint3D>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Box Collider 3D", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<BoxCollider3D>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Mesh Collider 3D", ImVec2(200, 0))) {
                        if (Scene::m_Object->HasComponent<MeshRenderer>()) {
                            Scene::m_Object->AddComponent<MeshCollider3D>();
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    if (ImGui::Button("Bloom", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<Bloom>();
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Path Finding AI", ImVec2(200, 0))) {
                        Scene::m_Object->AddComponent<PathfindingAI>();
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::End();

            if (ImGui::Begin(ICON_FA_FOLDER " Assets", nullptr)) {
                Scene::DropTargetMat(Scene::DRAG_GAMEOBJECT, nullptr);
                // create new file

                if (ImGui::BeginPopup("New File")) {
                    if (ImGui::Button(ICON_FA_CODE " Lua Script",
                                      ImVec2(200, 0))) {
#ifdef _WIN32
                        fs::path p = fs::path(currentDirectory.string() +
                                              "\\New Script.lua");
#else
                        fs::path p = fs::path(currentDirectory.string() +
                                              "/new_script.lua");
#endif

                        if (!fs::exists(p)) {
                            std::ofstream file(p);
                            file << "new_script = {}\n\n";
                            file << "function new_script:OnStart()\n";
                            file << "end\n\n";
                            file << "function new_script:OnUpdate()\n";
                            file << "end";

                            file.close();
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Button(ICON_FA_CODE " C++ Script",
                                      ImVec2(200, 0))) {
#ifdef _WIN32
                        fs::path cpp = fs::path(currentDirectory.string() +
                                                "\\NewScript.cpp");
                        fs::path hpp = fs::path(currentDirectory.string() +
                                                "\\NewScript.hpp");
#else
                        fs::path cpp = fs::path(currentDirectory.string() +
                                                "/NewScript.cpp");
                        fs::path hpp = fs::path(currentDirectory.string() +
                                                "/NewScript.hpp");
#endif

                        if (!fs::exists(hpp)) {
                            std::ofstream file(hpp);
                            std::string str = R"(#pragma once
#include <api.hpp>

using namespace HyperAPI;
using namespace HyperAPI::Experimental;

#ifdef _WIN32
#ifdef BUILD_DLL
#define VAULT_API __declspec(dllexport)
#else
#define VAULT_API __declspec(dllimport)
#endif
#else
#define VAULT_API
#endif

extern "C" {
class NewScript : CppScripting::Script {
public:
    NewScript() = default;
    ~NewScript() = default;

    void Start() override;
    void Update() override;
};

VAULT_API NewScript *create_object();
}
                            )";

                            file << str;
                            file.close();
                        }
                        if (!fs::exists(cpp)) {
                            std::ofstream file(cpp);
                            std::string str = R"(#include "NewScript.hpp"
NewScript *create_object() {
    return new NewScript;
}

void NewScript::Start() {}
void NewScript::Update() {}
                            )";

                            file << str;
                            file.close();
                        }
                        ImGui::CloseCurrentPopup();
                    }

                    // folder
                    if (ImGui::Button(ICON_FA_FOLDER " Folder",
                                      ImVec2(200, 0))) {
#ifdef _WIN32
                        fs::path p = fs::path(currentDirectory.string() +
                                              "\\New Folder");
#else
                        fs::path p =
                            fs::path(currentDirectory.string() + "/New Folder");
#endif

                        if (!fs::exists(p)) {
                            fs::create_directory(p);
                        }
                        ImGui::CloseCurrentPopup();
                    }

                    // file
                    if (ImGui::Button(ICON_FA_FILE " File", ImVec2(200, 0))) {
#ifdef _WIN32
                        fs::path p =
                            fs::path(currentDirectory.string() + "\\New File");
#else
                        fs::path p =
                            fs::path(currentDirectory.string() + "/New File");
#endif
                        if (!fs::exists(p)) {
                            std::ofstream file(p);
                            file.close();
                        }
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button(ICON_FA_PAINTBRUSH " Shader",
                                      ImVec2(200, 0))) {
#ifdef _WIN32
                        fs::path p = fs::path(currentDirectory.string() +
                                              "\\new_shader.glsl");
#else
                        fs::path p = fs::path(currentDirectory.string() +
                                              "/new_shader.glsl");
#endif

                        if (!fs::exists(p)) {
                            std::ofstream file(p);
                            std::string shader = R"(
                                    #shader vertex
                                    #version 330 core
                                    layout(location = 0) in vec3 position;
                                    layout(location = 1) in vec3 color;
                                    layout(location = 2) in vec3 aNormal;
                                    layout(location = 3) in vec2 g_texCoords;
                                    layout(location = 4) in ivec4 boneIds;
                                    layout(location = 5) in vec4 weights;
                                    layout(location = 6) in vec3 tangent;
                                    layout(location = 7) in vec3 bitangent;

                                    uniform mat4 camera;
                                    uniform mat4 translation;
                                    uniform mat4 rotation;
                                    uniform mat4 scale;
                                    uniform mat4 model;
                                    uniform mat4 lightSpaceMatrix;
                                    uniform vec3 cameraPosition;
                                    uniform vec2 texUvOffset;

                                    out DATA {
                                        vec2 texCoords;
                                        vec3 Color;
                                        vec3 Normal;
                                        vec3 currentPosition;
                                        vec3 reflectedVector;
                                        vec4 fragPosLight;
                                        mat4 projection;
                                        mat4 model;
                                        vec3 T;
                                        vec3 B;
                                        vec3 N;
                                    } data_out;

                                    const int MAX_BONES = 100;
                                    const int MAX_BONE_INFLUENCE = 4;
                                    uniform mat4 finalBonesMatrices[MAX_BONES];

                                    void main() {
                                        vec4 totalPosition = vec4(0);

                                        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
                                        {
                                            if(boneIds[i] == -1) {
                                                continue;
                                            }

                                            if(boneIds[i] >=MAX_BONES)
                                            {
                                                totalPosition = vec4(position, 1.0f);
                                                break;
                                            }

                                            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(position, 1.0f);
                                            totalPosition += localPosition * weights[i];
                                            vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
                                        }
                                        if(totalPosition == vec4(0))
                                        {
                                            totalPosition = vec4(position, 1.0f);
                                        }

                                        vec4 worldPosition = model * translation * rotation * scale * totalPosition;
                                        data_out.currentPosition = vec3(model * translation * rotation * scale * totalPosition);
                                        data_out.projection = camera;
                                        gl_Position = vec4(data_out.currentPosition, 1.0);
                                        data_out.model = model * translation * rotation * scale;

                                        vec2 finalCoords = g_texCoords;

                                        if(finalCoords.x > 0) {
                                            finalCoords.x = finalCoords.x + texUvOffset.x;
                                        }

                                        if(finalCoords.y > 0) {
                                            finalCoords.y = finalCoords.y + texUvOffset.y;
                                        }

                                        data_out.texCoords = finalCoords;
                                        data_out.Color = color;
                                        // make normal apply rotation
                                        data_out.Normal = mat3(transpose(inverse(model))) * aNormal;
                                        // Normal = aNormal;

                                        data_out.fragPosLight = lightSpaceMatrix * vec4(data_out.currentPosition, 1.0);

                                        vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);
                                        data_out.reflectedVector = reflect(viewVector, data_out.Normal);

                                        vec3 T   = normalize(mat3(model) * tangent);
                                        vec3 B   = normalize(mat3(model) * bitangent);
                                        vec3 N   = normalize(mat3(model) * data_out.Normal);

                                        data_out.T = T;
                                        data_out.B = B;
                                        data_out.N = N;
                                    }
                                    #shader fragment
                                    #version 330 core

                                    layout(location = 0) out vec4 FragColor;
                                    layout(location = 1) out vec4 BloomColor;
                                    layout(location = 2) out uint EntityID;

                                    in vec2 texCoords;
                                    in vec3 Color;
                                    in vec3 Normal;
                                    in vec3 currentPosition;
                                    in vec3 reflectedVector;
                                    in vec4 fragPosLight;
                                    in mat3 m_TBN;

                                    struct PointLight {
                                        vec3 lightPos;
                                        vec3 color;
                                        float intensity;
                                    };

                                    struct SpotLight {
                                        vec3 lightPos;
                                        vec3 color;
                                        vec3 angle;
                                    };

                                    struct DirectionalLight {
                                        vec3 lightPos;
                                        vec3 color;
                                        float intensity;
                                    };

                                    struct Light2D {
                                        vec2 lightPos;
                                        vec3 color;
                                        float range;
                                    };

                                    uniform float ambient;
                                    #define MAX_LIGHTS 60
                                    uniform PointLight pointLights[MAX_LIGHTS];
                                    uniform Light2D light2ds[MAX_LIGHTS];
                                    uniform SpotLight spotLights[MAX_LIGHTS];
                                    uniform DirectionalLight dirLights[MAX_LIGHTS];

                                    uniform int isTex;
                                    uniform samplerCube cubeMap;
                                    uniform vec3 cameraPosition;

                                    //Material properties
                                    uniform sampler2D texture_diffuse0;
                                    uniform sampler2D texture_specular0;
                                    uniform sampler2D texture_normal0;
                                    uniform sampler2D texture_height0;
                                    uniform sampler2D texture_emission0;
                                    uniform vec4 baseColor;
                                    uniform vec3 u_BloomColor;
                                    uniform float metallic;
                                    uniform float roughness;

                                    //texture setters
                                    uniform int hasNormalMap;
                                    uniform int hasHeightMap;

                                    //global textures
                                    uniform sampler2D shadowMap;

                                    vec4 reflectedColor = texture(cubeMap, reflectedVector);
                                    float specularTexture = texture(texture_specular0, texCoords).r;

                                    float near = 0.1;
                                    float far = 100.0;

                                    uniform uint u_EntityID;

                                    // Post Processing uniforms
                                    uniform int globalBloom;
                                    uniform float bloomThreshold;

                                    uniform float DeltaTime;

                                    void main() {
                                        FragColor = texture(texture_diffuse0, texCoords);
                                        EntityID = u_EntityID;
                                    }

                                    #shader geometry
                                    #version 330 core
                                    layout(triangles) in;
                                    layout(triangle_strip, max_vertices = 3) out;

                                    out vec2 texCoords;
                                    out vec3 Color;
                                    out vec3 Normal;
                                    out vec3 currentPosition;
                                    out vec3 reflectedVector;
                                    out vec4 fragPosLight;
                                    out mat3 m_TBN;

                                    in DATA {
                                        vec2 texCoords;
                                        vec3 Color;
                                        vec3 Normal;
                                        vec3 currentPosition;
                                        vec3 reflectedVector;
                                        vec4 fragPosLight;
                                        mat4 projection;
                                        mat4 model;
                                        vec3 T;
                                        vec3 B;
                                        vec3 N;
                                    } data_in[];

                                    void main() {
                                        gl_Position = data_in[0].projection * gl_in[0].gl_Position;
                                        Normal = data_in[0].Normal;
                                        Color = data_in[0].Color;
                                        currentPosition = gl_in[0].gl_Position.xyz;
                                        reflectedVector = data_in[0].reflectedVector;
                                        fragPosLight = data_in[0].fragPosLight;
                                        texCoords = data_in[0].texCoords;
                                        m_TBN = TBN;
                                        EmitVertex();

                                        gl_Position = data_in[1].projection * gl_in[1].gl_Position;
                                        Normal = data_in[1].Normal;
                                        Color = data_in[1].Color;
                                        currentPosition = gl_in[1].gl_Position.xyz;
                                        reflectedVector = data_in[1].reflectedVector;
                                        fragPosLight = data_in[1].fragPosLight;
                                        texCoords = data_in[1].texCoords;
                                        m_TBN = TBN;
                                        EmitVertex();

                                        gl_Position = data_in[2].projection * gl_in[2].gl_Position;
                                        Normal = data_in[2].Normal;
                                        Color = data_in[2].Color;
                                        currentPosition = gl_in[2].gl_Position.xyz;
                                        reflectedVector = data_in[2].reflectedVector;
                                        fragPosLight = data_in[2].fragPosLight;
                                        texCoords = data_in[2].texCoords;
                                        m_TBN = TBN;
                                        EmitVertex();

                                        EndPrimitive();
                                    }
                                )";

                            std::string newShader;
                            std::stringstream ss(shader);
                            std::string line;
                            while (std::getline(ss, line)) {
                                // replace " " with "" using regex
                                newShader +=
                                    std::regex_replace(
                                        line,
                                        std::regex("                           "
                                                   "         "),
                                        "") +
                                    "\n";
                            }
                            file << newShader;

                            file.close();
                        }

                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button(ICON_FA_FILE " Material",
                                      ImVec2(200, 0))) {
#ifdef _WIN32
                        fs::path p = fs::path(currentDirectory.string() +
                                              "\\New Material.material");
#else
                        fs::path p = fs::path(currentDirectory.string() +
                                              "/New Material.material");
#endif

                        if (!fs::exists(p)) {
                            std::ofstream file(p);
                            nlohmann::json j = {
                                {"diffuse", "nullptr"},
                                {"specular", "nullptr"},
                                {"normal", "nullptr"},
                                {"roughness", 0},
                                {"metallic", 0},
                                {"baseColor",
                                 {{"r", 1}, {"g", 1}, {"b", 1}, {"a", 1}}},
                                {"texUV", {{"x", 0}, {"y", 0}}}};

                            file << j.dump(4);
                            file.close();
                        }
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (currentDirectory != fs::path("assets")) {
                    if (ImGui::Button(ICON_FA_ARROW_LEFT)) {
                        currentDirectory = currentDirectory.parent_path();
                    }

                    ImGui::SameLine();
                }

                if (ImGui::Button(ICON_FA_PLUS " New File", ImVec2(0, 0))) {
                    ImGui::OpenPopup("New File");
                }

                ImGui::Separator();
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload *payload =
                            ImGui::AcceptDragDropPayload("file")) {
                        std::string file = (char *)payload->Data;
                        file.erase(0, cwd.length() + 1);
                        std::string newFile =
                            "assets/" + file.substr(file.find_last_of("/") + 1);
                        try {
                            fs::rename(file, newFile);
                        } catch (const std::exception &e) {
                            std::cout << e.what() << std::endl;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                DirIter(cwd + std::string("/assets"));
                if (ImGui::BeginPopup("File Options")) {
                    static bool renameWindow = false;
                    if (!renameWindow) {
                        if (ImGui::Button("Rename", ImVec2(200, 0))) {
                            renameWindow = true;
                        }

                        if (ImGui::Button("Delete", ImVec2(200, 0))) {
                            fs::remove(m_originalName);
                            ImGui::CloseCurrentPopup();
                        }
                    } else {
                        ImGui::InputText("New Name", newName, 256);

                        if (ImGui::Button("Rename")) {
#ifdef _WIN32
                            fs::path p = fs::path(m_originalName);
                            fs::path newP = fs::path(
                                m_originalName.substr(
                                    0, m_originalName.find_last_of("\\")) +
                                "\\" + newName);
                            fs::rename(p, newP);
#else
                            fs::path p = fs::path(m_originalName);
                            fs::path newP = fs::path(
                                m_originalName.substr(
                                    0, m_originalName.find_last_of("/")) +
                                "/" + newName);
                            fs::rename(p, newP);
#endif
                            renameWindow = false;
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::End();

            // scroll to the bottom
            if (ImGui::Begin(ICON_FA_TERMINAL " Console")) {
                if (ImGui::Button(ICON_FA_TRASH " Clear")) {
                    Scene::logs.clear();
                }

                ImGui::Separator();

                ImGui::BeginChild("Logs");

                for (auto &log : Scene::logs) {
                    log.GUI();
                }
                if (HyperAPI::isRunning) {
                    ImGui::SetScrollHereY(0.999f);
                }

                ImGui::EndChild();
            }
            ImGui::End();

            if (ImGuiFileDialog::Instance()->Display("BuildWindowsDialog")) {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string filePathName =
                        ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath =
                        ImGuiFileDialog::Instance()->GetCurrentPath();

                    if (fs::exists(filePathName + "/assets"))
                        fs::remove_all(filePathName + "/assets");

                    if (fs::exists(filePathName + "/imgui.ini"))
                        fs::remove(filePathName + "/imgui.ini");

                    if (fs::exists(filePathName + "/shaders"))
                        fs::remove_all(filePathName + "/shaders");

                    if (fs::exists(filePathName + "/build"))
                        fs::remove_all(filePathName + "/build");

                    if (fs::exists(filePathName + "/cs-assembly"))
                        fs::remove_all(filePathName + "/cs-assembly");

                    if (fs::exists(filePathName + "/lib"))
                        fs::remove_all(filePathName + "/lib");

                    if (fs::exists(filePathName + "/mono"))
                        fs::remove_all(filePathName + "/mono");

                    if (fs::exists(filePathName + "/" + config.name + ".bat"))
                        fs::remove(filePathName + "/" + config.name + ".bat");

                    std::string m_cwd = CsharpVariables::oldCwd;
#ifdef _WIN32
                    fs::copy("./assets", filePathName + "/assets", fs::copy_options::recursive);
                    fs::copy("./shaders", filePathName + "/shaders", fs::copy_options::recursive);
                    fs::copy("./lib", filePathName + "/lib", fs::copy_options::recursive);
                    fs::copy(m_cwd + "/bin/game.exe", filePathName + "/lib/game.exe", fs::copy_options::recursive);
                    fs::copy(m_cwd + "/bin/LaunchGame.bat", filePathName + "/" + config.name + ".bat", fs::copy_options::recursive);
                    fs::copy("./build", filePathName + "/build", fs::copy_options::recursive);
                    fs::copy("./cs-assembly", filePathName + "/cs-assembly", fs::copy_options::recursive);
                    fs::copy("./imgui.ini", filePathName + "/imgui.ini", fs::copy_options::recursive);
#else
                    fs::copy("./assets", filePathName + "/assets", fs::copy_options::recursive);
                    fs::copy("./shaders", filePathName + "/shaders", fs::copy_options::recursive);
                    fs::copy("./bin/dlls", filePathName + "/lib", fs::copy_options::recursive);
                    fs::copy(m_cwd + "/bin/game.exe", filePathName + "/lib/game.exe", fs::copy_options::recursive);
                    fs::copy(m_cwd + "/bin/LaunchGame.bat", filePathName + "/" + config.name + ".bat", fs::copy_options::recursive);
                    fs::copy("./build", filePathName + "/build", fs::copy_options::recursive);
                    fs::copy("./cs-assembly", filePathName + "/cs-assembly", fs::copy_options::recursive);
                    fs::copy(m_cwd + "/mono/lib", filePathName + "/lib/mono", fs::copy_options::recursive);
                    fs::copy("./imgui.ini", filePathName + "/imgui.ini", fs::copy_options::recursive);
#endif

                    for (const auto &iter : std::filesystem::directory_iterator(filePathName + "/cs-assembly")) {
                        if (iter.path().extension() == ".cs") {
                            std::filesystem::remove(iter.path());
                        }
                    }
                    fs::remove_all(filePathName + "/cs-assembly/API");
                }
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGuiFileDialog::Instance()->Display("BuildLinuxDialog")) {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string filePathName =
                        ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath =
                        ImGuiFileDialog::Instance()->GetCurrentPath();

                    if (fs::exists(filePathName + "/shaders"))
                        fs::remove_all(filePathName + "/shaders");

                    if (fs::exists(filePathName + "/assets"))
                        fs::remove_all(filePathName + "/assets");

                    if (fs::exists(filePathName + "/lib"))
                        fs::remove_all(filePathName + "/lib");

                    if (fs::exists(filePathName + "/build"))
                        fs::remove_all(filePathName + "/build");

                    if (fs::exists(filePathName + "/cs-assembly"))
                        fs::remove_all(filePathName + "/cs-assembly");

                    if (fs::exists(filePathName + "/mono"))
                        fs::remove_all(filePathName + "/mono");

                    if (fs::exists(filePathName + "/imgui.ini"))
                        fs::remove(filePathName + "/imgui.ini");

                    if (fs::exists(filePathName + "/" + config.name + ".sh"))
                        fs::remove(filePathName + "/" + config.name + ".sh");

                    std::string m_cwd = CsharpVariables::oldCwd;
                    fs::copy("./assets", filePathName + "/assets", fs::copy_options::recursive);
                    fs::copy("./shaders", filePathName + "/shaders", fs::copy_options::recursive);
                    fs::copy("./lib", filePathName + "/lib", fs::copy_options::recursive);
                    fs::copy(m_cwd + "/bin/game.out", filePathName + "/lib/game.out", fs::copy_options::recursive);
                    fs::copy(m_cwd + "/bin/LaunchGame.sh", filePathName + "/" + config.name + ".sh", fs::copy_options::recursive);
                    fs::copy("./build", filePathName + "/build", fs::copy_options::recursive);
                    fs::copy("./cs-assembly", filePathName + "/cs-assembly", fs::copy_options::recursive);
                    fs::copy(m_cwd + "/mono", filePathName + "/mono", fs::copy_options::recursive);
                    fs::copy("./imgui.ini", filePathName + "/imgui.ini", fs::copy_options::recursive);

                    for (const auto &iter : std::filesystem::directory_iterator(filePathName + "/cs-assembly")) {
                        if (iter.path().extension() == ".cs") {
                            std::filesystem::remove(iter.path());
                        }
                    }
                    fs::remove_all(filePathName + "/cs-assembly/API");
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }
        };
#else
    std::function<void(uint32_t & PPT, uint32_t & PPFBO, uint32_t & gui_gui)>
        GUI_EXP = [&](uint32_t &PPT, uint32_t &PPFBO, uint32_t &gui_gui) {};
#endif

    bool calledOnce = false;
    float deltaTime, lastFrame;

    //    Text::Font font("assets/fonts/OpenSans-Bold.ttf", 48);
    Transform fontTransform;
    struct ScrollData {
        double x = 0;
        double y = 0;
        bool called = false;
        ScrollData() = default;
    };
    ScrollData *scrollData = new ScrollData();
    glfwSetWindowUserPointer(app.renderer->window, scrollData);

    float wheel = 0;

    // infinite grid
#ifndef GAME_BUILD
    Mesh *gridMesh = Plane(Vector4(1, 0, 0, 1)).m_Mesh;
#endif

    float runTime;

    // Transform batch_trans;
    // batch_trans.position = Vector3(0, 0, 0);
    // batch_trans.rotation = Vector3(0, 0, 0);
    // batch_trans.scale = Vector3(1, 1, 1);

    // Plane batch_plane;
    // Batch batch_layer;
    // Material batch_mat;
    // Shader batch_shader("shaders/batch.glsl");
    // batch_layer.AddMesh(batch_plane.m_Mesh->vertices, batch_plane.m_Mesh->indices, &batch_trans);

    // Terrain terrain;

    app.Run(
        [&](uint32_t &shadowMapTex) {
            // glEnable(GL_CULL_FACE);
            // glCullFace(GL_BACK);
            // glFrontFace(GL_CCW);

            // glEnable(GL_DEPTH_TEST);
            // batch_layer.Draw(shader, *Scene::mainCamera, batch_mat);

            // animator.UpdateAnimation(Timestep::deltaTime);
            runTime += Timestep::deltaTime;
            shader.Bind();
            shader.SetUniform1i("shadowMap", GL_TEXTURE7);
            shader.SetUniformMat4("lightSpaceMatrix", Scene::projection);
            shader.SetUniform1f("time", Timestep::deltaTime);

            if (Scene::LoadingScene)
                return;

            // std::vector<Shader*> shaders = {&shader};
            PostProcessingEffects(shader, camera);

            app.exposure = config.exposure;
            app.isGuzimoInUse = usingImGuizmo;

            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            if (HyperAPI::isRunning) {
                for (auto &gameObject : Scene::m_GameObjects) {
                    if (!gameObject->HasComponent<PathfindingAI>())
                        continue;

                    auto &component = gameObject->GetComponent<PathfindingAI>();
                    component.Update(shader, *Scene::mainCamera);
                }

                BulletPhysicsWorld::UpdatePhysics();
                BulletPhysicsWorld::CollisionCallback([&](const std::string
                                                              &idA,
                                                          const std::string
                                                              &idB) {
                    auto gameObjectA = f_GameObject::FindGameObjectByID(idA);
                    auto gameObjectB = f_GameObject::FindGameObjectByID(idB);
                    if (!gameObjectA || !gameObjectB)
                        return;

                    if (gameObjectA->HasComponent<NativeScriptManager>()) {
                        auto &scriptManager =
                            gameObjectA->GetComponent<NativeScriptManager>();
                        for (auto script : scriptManager.m_StaticScripts) {
                            script->Collision3D(gameObjectB);
                        }
                    }

                    if (gameObjectB->HasComponent<NativeScriptManager>()) {
                        auto &scriptManager =
                            gameObjectB->GetComponent<NativeScriptManager>();
                        for (auto script : scriptManager.m_StaticScripts) {
                            script->Collision3D(gameObjectA);
                        }
                    }

                    if (gameObjectA->HasComponent<m_LuaScriptComponent>()) {
                        auto &scriptManager =
                            gameObjectA->GetComponent<m_LuaScriptComponent>();
                        for (auto script : scriptManager.scripts) {
                            script.Collision3D(gameObjectB);
                        }
                    }

                    if (gameObjectB->HasComponent<m_LuaScriptComponent>()) {
                        auto &scriptManager =
                            gameObjectB->GetComponent<m_LuaScriptComponent>();
                        for (auto script : scriptManager.scripts) {
                            script.Collision3D(gameObjectA);
                        }
                    }

                    if (gameObjectA->HasComponent<CsharpScriptManager>()) {
                        auto &scriptManager = gameObjectA->GetComponent<CsharpScriptManager>();

                        for (auto klass : scriptManager.selectedScripts) {
                            MonoObject *exception = nullptr;
                            MonoScriptClass *behaviour =
                                CsharpScriptEngine::instances[klass.first];
                            MonoMethod *method = behaviour->GetMethod("OnCollisionEnter3D", 1);
                            if (!method)
                                continue;

                            void *params = mono_string_new(CsharpVariables::appDomain, gameObjectB->ID.c_str());
                            mono_runtime_invoke(method, behaviour->f_GetObject(), &params, &exception);
                        }
                    }

                    if (gameObjectB->HasComponent<CsharpScriptManager>()) {
                        auto &scriptManager = gameObjectB->GetComponent<CsharpScriptManager>();

                        for (auto klass : scriptManager.selectedScripts) {
                            MonoObject *exception = nullptr;
                            MonoScriptClass *behaviour =
                                CsharpScriptEngine::instances[klass.first];
                            MonoMethod *method = behaviour->GetMethod("OnCollisionEnter3D", 1);
                            if (!method)
                                continue;

                            void *params = mono_string_new(CsharpVariables::appDomain, gameObjectA->ID.c_str());
                            mono_runtime_invoke(method, behaviour->f_GetObject(), &params, &exception);
                        }
                    }
                });

#ifndef _WIN32 || GAME_BUILD
                UpdatePresence("In Editor",
                               "Playtesting " + std::string(config.name));
#endif
            }

            frameCount++;
            timeElapsed += Timestep::deltaTime;
            if (timeElapsed >= 1.0f) {
                fps = frameCount;
                frameCount = 0;
                timeElapsed -= 1.0f;

                fpsText = "FPS: " + std::to_string(fps);
            }

            if (Scene::mainCamera == nullptr) {
                Scene::mainCamera = camera;
            }

            if (!calledOnce) {
                auto &colors = ImGui::GetStyle().Colors;
                colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

                colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                colors[ImGuiCol_HeaderHovered] =
                    ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
                colors[ImGuiCol_HeaderActive] =
                    ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                colors[ImGuiCol_Button] = ImVec4(0.6f, 0.2f, 0.2f, 1.0f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(1, 0.205f, 0.2f, 1.0f);
                colors[ImGuiCol_ButtonActive] = ImVec4(1, 0.305f, 0.3f, 1.0f);

                colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                colors[ImGuiCol_FrameBgHovered] =
                    ImVec4(0.3, 0.305f, 0.3f, 1.0f);
                colors[ImGuiCol_FrameBgActive] =
                    ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                colors[ImGuiCol_Tab] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                colors[ImGuiCol_TabHovered] = ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
                colors[ImGuiCol_TabActive] =
                    ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);
                colors[ImGuiCol_TabUnfocused] =
                    ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                colors[ImGuiCol_TabUnfocusedActive] =
                    ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
                colors[ImGuiCol_TitleBgActive] =
                    ImVec4(0.25f, 0.255f, 0.25f, 1.0f);
                colors[ImGuiCol_TitleBgCollapsed] =
                    ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

                colors[ImGuiCol_ResizeGrip] = ImVec4(1, 0.15, 0.15, 1);
                colors[ImGuiCol_ResizeGripActive] = ImVec4(1, 0.30, 0.30, 1);
                colors[ImGuiCol_ResizeGripHovered] = ImVec4(1, 0.20, 0.20, 1);
                colors[ImGuiCol_NavWindowingHighlight] =
                    ImVec4(1, 0.15, 0.15, 1);

                colors[ImGuiCol_DockingPreview] = ImVec4(1, 0.15, 0.15, 1);

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                                    ImVec2(4.5, 2.5));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.5f);
            }

            calledOnce = true;

            Input::winSize = Vector3(app.width, app.height, 0);

            if (hoveredScene && !usingImGuizmo && camera->mode2D &&
                Scene::mainCamera == camera) {
                camera->Inputs(app.renderer->window, winPos);
                auto transform = camera->GetComponent<TransformComponent>();
                transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);

                if (scrollData->called) {
                    if (scrollData->y < 0) {
                        transform.scale.x -= 0.1f;
                        transform.scale.y -= 0.1f;
                    } else {
                        transform.scale.x += 0.1f;
                        transform.scale.y += 0.1f;
                    }

                    if (transform.scale.x < 0) {
                        transform.scale.x = 0.01;
                        transform.scale.y = 0.01;
                    }

                    scrollData->called = false;
                    scrollData->x = 0;
                    scrollData->y = 0;
                }

                camera->UpdateComponent(transform);

                // scroll wheel
            }

            if (hoveredScene && ImGui::IsMouseDragging(0) && !usingImGuizmo &&
                Scene::mainCamera == camera) {
                camera->Inputs(app.renderer->window, winPos);
                camera->moving = true;
            } else {
                camera->moving = false;
            }

            winSize = Vector2(app.width, app.height);

            for (auto &camera : Scene::cameras) {
                camera->cursorWinW = app.winWidth;
                camera->cursorWinH = app.winHeight;

                camera->updateMatrix(
                    camera->cam_fov, camera->cam_near, camera->cam_far,
                    Vector2(app.winWidth, app.winHeight), winSize);

                if (camera->m_MouseMovement && HyperAPI::isRunning) {
#ifdef GAME_BUILD
                    camera->MouseMovement(winPos);
#else
                    if (hoveredScene) {
                        camera->MouseMovement(winPos);
                    }
#endif
                }
            }
            camera->cursorWinW = winSize.x;
            camera->cursorWinH = winSize.y;
            camera->updateMatrix(camera->cam_fov, camera->cam_near,
                                 camera->cam_far,
                                 Vector2(app.winWidth, app.winHeight), winSize);
            skybox.Draw(*Scene::mainCamera, winSize.x, winSize.y);
            // floor.Draw(shader, *camera);
            shader.Bind();
            shader.SetUniform1f("ambient", config.ambientLight);

            if (HyperAPI::isRunning && Scene::world != nullptr) {
                const int32_t velocityIterations = 6;
                const int32_t positionIterations = 2;
                Scene::world->Step(1.0f / 60.0f, velocityIterations,
                                   positionIterations);

                auto view = Scene::m_Registry.view<Rigidbody2D>();
                for (auto e : view) {
                    GameObject *m_GameObject;
                    for (auto &gameObject : Scene::m_GameObjects) {
                        if (gameObject->entity == e) {
                            m_GameObject = gameObject;
                        }
                    }

                    auto &transform = m_GameObject->GetComponent<Transform>();
                    auto &rigidbody = m_GameObject->GetComponent<Rigidbody2D>();

                    if (rigidbody.body == nullptr) {
                        b2BodyDef bodyDef;
                        bodyDef.userData.pointer =
                            reinterpret_cast<uintptr_t>(m_GameObject);
                        bodyDef.type = rigidbody.type;
                        bodyDef.position.Set(transform.position.x,
                                             transform.position.y);
                        bodyDef.angle = glm::radians(transform.rotation.z);
                        bodyDef.gravityScale = rigidbody.gravityScale;

                        b2Body *body = Scene::world->CreateBody(&bodyDef);
                        body->SetFixedRotation(rigidbody.fixedRotation);
                        rigidbody.body = body;

                        if (m_GameObject->HasComponent<BoxCollider2D>()) {
                            auto &boxCollider2D =
                                m_GameObject->GetComponent<BoxCollider2D>();
                            b2PolygonShape shape;
                            shape.SetAsBox(
                                (((boxCollider2D.size.x) / 2) - 0.02) / 2,
                                (((boxCollider2D.size.y) / 2) - 0.02) / 2);

                            b2FixtureDef fixtureDef;
                            fixtureDef.isSensor = boxCollider2D.trigger;
                            fixtureDef.shape = &shape;
                            fixtureDef.density = boxCollider2D.density;
                            fixtureDef.friction = boxCollider2D.friction;
                            fixtureDef.restitution = boxCollider2D.restitution;
                            fixtureDef.restitutionThreshold =
                                boxCollider2D.restitutionThreshold;
                            boxCollider2D.fixture =
                                body->CreateFixture(&fixtureDef);
                        }
                    }

                    b2Body *body = (b2Body *)rigidbody.body;
                    const auto &position = body->GetPosition();
                    transform.position.x = position.x;
                    transform.position.y = position.y;
                    transform.rotation.z = glm::degrees(body->GetAngle());
                }

                for (auto *gameObject : Scene::m_GameObjects) {
                    if (gameObject->HasComponent<Audio3D>()) {
                        gameObject->GetComponent<Audio3D>().Update();
                    }
                }
            }

            if (bulletPhysicsStarted) {
                auto view = Scene::m_Registry.view<Rigidbody3D>();

                for (auto e : view) {
                    GameObject *m_GameObject;

                    for (auto &obj : Scene::m_GameObjects) {
                        if (obj->entity == e) {
                            m_GameObject = obj;
                        }
                    }

                    auto &rigidbody = m_GameObject->GetComponent<Rigidbody3D>();
                    if (rigidbody.body == nullptr) {
                        auto &rigidbody =
                            m_GameObject->GetComponent<Rigidbody3D>();
                        rigidbody.transform =
                            &m_GameObject->GetComponent<Transform>();

                        if (m_GameObject->HasComponent<BoxCollider3D>()) {
                            auto &collider =
                                m_GameObject->GetComponent<BoxCollider3D>();
                            collider.CreateShape();
                            rigidbody.CreateBody(collider.shape);
                        }

                        if (m_GameObject->HasComponent<MeshCollider3D>()) {
                            auto &collider =
                                m_GameObject->GetComponent<MeshCollider3D>();
                            collider.CreateShape(
                                &m_GameObject->GetComponent<MeshRenderer>());
                            rigidbody.CreateBody(collider.shape);
                        }
                    }

                    rigidbody.transform =
                        &m_GameObject->GetComponent<Transform>();
                    rigidbody.Update();
                }
            }

            for (auto &gameObject : Scene::m_GameObjects) {
                if (!gameObject)
                    continue;

                gameObject->UpdateEnabled();

                if (!gameObject->enabled)
                    continue;
                if (gameObject->prefab)
                    continue;

                if (!gameObject)
                    return;

                gameObject->Update();

                if (gameObject->HasComponent<Transform>()) {
                    gameObject->GetComponent<Transform>().Update();
                }

                // if (gameObject->HasComponent<m_LuaScriptComponent>()) {
                //     gameObject->GetComponent<m_LuaScriptComponent>().Update();
                // }

                if (gameObject->HasComponent<c_PointLight>()) {
                    gameObject->GetComponent<c_PointLight>().Update();
                }

                if (gameObject->HasComponent<c_Light2D>()) {
                    gameObject->GetComponent<c_Light2D>().Update();
                }

                if (gameObject->HasComponent<c_SpotLight>()) {
                    gameObject->GetComponent<c_SpotLight>().Update();
                }

                if (gameObject->HasComponent<c_DirectionalLight>()) {
                    gameObject->GetComponent<c_DirectionalLight>().Update();
                }

                if (gameObject->HasComponent<SpritesheetRenderer>()) {
                    gameObject->GetComponent<SpritesheetRenderer>().Update();
                }

                if (gameObject->HasComponent<SpriteRenderer>()) {
                    gameObject->GetComponent<SpriteRenderer>().Update();
                }

                if (gameObject->HasComponent<MeshRenderer>()) {
                    gameObject->GetComponent<MeshRenderer>().Update();
                }

                if (gameObject->HasComponent<AudioListener>()) {
                    gameObject->GetComponent<AudioListener>().Update();
                }

                if (gameObject->HasComponent<m_LuaScriptComponent>()) {
                    auto &script =
                        gameObject->GetComponent<m_LuaScriptComponent>();
                    if (HyperAPI::isRunning) {
                        script.Update();
                    }
                }

                if (gameObject->HasComponent<CppScriptManager>()) {
                    auto &script = gameObject->GetComponent<CppScriptManager>();
                    if (HyperAPI::isRunning) {
                        script.Update();
                    }
                }

                if (gameObject->HasComponent<CsharpScriptManager>()) {
                    auto &script =
                        gameObject->GetComponent<CsharpScriptManager>();
                    if (HyperAPI::isRunning) {
                        script.Update();
                    }
                }

                if (gameObject->HasComponent<NativeScriptManager>()) {
                    auto &script =
                        gameObject->GetComponent<NativeScriptManager>();
                    if (HyperAPI::isRunning) {
                        script.Update();
                    }
                }

                if (gameObject->HasComponent<BoxCollider2D>()) {
                    auto &component =
                        gameObject->GetComponent<BoxCollider2D>();
                    if (HyperAPI::isRunning) {
                        component.Update();
                    }
                }
            }

            //        font.Render(textShader, *camera, "Hello World!",
            //        fontTransform);

            // Transform Updates
            for (auto &gameObject : Scene::m_GameObjects) {
                if (gameObject->HasComponent<Transform>()) {
                    auto &transform = gameObject->GetComponent<Transform>();
                    transform.Update();
                }
            }

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // disable wireframe mode
            // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            for (auto &layer : Scene::layers) {
                bool notInCameraLayer = true;
                for (auto &camLayer : Scene::mainCamera->layers) {
                    if (Scene::mainCamera == camera)
                        break;
                    if (camLayer == layer.first) {
                        notInCameraLayer = false;
                        break;
                    }
                }

                bool isDepthCamera = false;
                auto view = Scene::m_Registry.view<CameraComponent>();
                for (auto e : view) {
                    auto &cam = Scene::m_Registry.get<CameraComponent>(e);
                    if (cam.camera == Scene::mainCamera && cam.depthCamera) {
                        isDepthCamera = true;
                        break;
                    }
                }

                if (notInCameraLayer && Scene::mainCamera != camera)
                    continue;
                if (isDepthCamera && Scene::mainCamera != camera)
                    continue;

                for (auto &gameObject : Scene::m_GameObjects) {
                    if (!gameObject->enabled)
                        continue;
                    if (gameObject->layer != layer.first)
                        continue;
                    if (gameObject->HasComponent<MeshRenderer>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto meshRenderer =
                            gameObject->GetComponent<MeshRenderer>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        glm::mat4 extra = meshRenderer.extraMatrix;

                        glm::mat4 m_parentTransform = glm::mat4(1.0f);

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                parentTransform.Update();
                                m_parentTransform = parentTransform.transform;
                            }
                        }

                        if (meshRenderer.m_Mesh != nullptr) {
                            shader.Bind();
                            shader.SetUniformMat4("lightSpaceMatrix",
                                                  Scene::projection);

                            if (Scene::m_Object == gameObject) {
                                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                                glStencilMask(0xFF);
                            }

                            meshRenderer.m_Mesh->enttId =
                                (uint32_t)gameObject->entity;
                            glActiveTexture(GL_TEXTURE21);
                            glBindTexture(GL_TEXTURE_CUBE_MAP,
                                          skybox.cubemapTexture);

                            // if (meshRenderer.customShader.shader != nullptr) {
                            //     meshRenderer.customShader.shader->Bind();
                            //     meshRenderer.customShader.shader->SetUniform1f("DeltaTime", runTime);
                            // }

                            if (meshRenderer.meshType == "Plane") {
                                glDisable(GL_CULL_FACE);
                                meshRenderer.m_Mesh->Draw(
                                    meshRenderer.customShader.usingCustomShader
                                        ? *meshRenderer.customShader.shader
                                        : shader,
                                    *Scene::mainCamera,
                                    transform.transform * extra *
                                        m_parentTransform);
                                glEnable(GL_CULL_FACE);
                            } else {
                                meshRenderer.m_Mesh->Draw(
                                    meshRenderer.customShader.usingCustomShader
                                        ? *meshRenderer.customShader.shader
                                        : shader,
                                    *Scene::mainCamera,
                                    transform.transform * extra *
                                        m_parentTransform);
                            }
                        }
                    }

                    if (gameObject->HasComponent<Text3D>()) {
                        auto &renderer = gameObject->GetComponent<Text3D>();
                        renderer.Update();
                        auto &transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        glm::mat4 m_parentTransform = glm::mat4(1.0f);

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                parentTransform.Update();
                                m_parentTransform = parentTransform.transform;
                            }
                        }

                        renderer.font->Draw(*Scene::mainCamera, transform.transform * m_parentTransform, renderer.text, renderer.color, renderer.bloomColor, 0, 0, renderer.scale, renderer.y_offset);
                    }

                    if (gameObject->HasComponent<SpriteRenderer>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto spriteRenderer =
                            gameObject->GetComponent<SpriteRenderer>();
                        auto transform = gameObject->GetComponent<Transform>();
                        glm::mat4 m_parentTransform = glm::mat4(1.0f);

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                parentTransform.Update();
                                m_parentTransform = parentTransform.transform;
                            }
                        }

                        glActiveTexture(GL_TEXTURE21);
                        glBindTexture(GL_TEXTURE_CUBE_MAP,
                                      skybox.cubemapTexture);

                        spriteRenderer.mesh->enttId =
                            (uint32_t)gameObject->entity;
                        glDisable(GL_CULL_FACE);

                        if (spriteRenderer.customShader.shader != nullptr) {
                            spriteRenderer.customShader.shader->Bind();
                            spriteRenderer.customShader.shader->SetUniform1f("DeltaTime", runTime);
                        }

                        spriteRenderer.mesh->Draw(
                            spriteRenderer.customShader.usingCustomShader
                                ? *spriteRenderer.customShader.shader
                                : shader,
                            *Scene::mainCamera,
                            transform.transform * m_parentTransform);
                        glEnable(GL_CULL_FACE);
                    }

                    if (gameObject->HasComponent<SpritesheetRenderer>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto spritesheetRenderer =
                            gameObject->GetComponent<SpritesheetRenderer>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        glm::mat4 m_parentTransform = glm::mat4(1.0f);

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                parentTransform.Update();
                                m_parentTransform = parentTransform.transform;
                            }
                        }

                        glActiveTexture(GL_TEXTURE21);
                        glBindTexture(GL_TEXTURE_CUBE_MAP,
                                      skybox.cubemapTexture);

                        // disable face culling
                        glDisable(GL_CULL_FACE);
                        if (spritesheetRenderer.mesh != nullptr) {
                            spritesheetRenderer.mesh->enttId =
                                (uint32_t)gameObject->entity;

                            if (spritesheetRenderer.customShader.shader != nullptr) {
                                spritesheetRenderer.customShader.shader->Bind();
                                spritesheetRenderer.customShader.shader->SetUniform1f("DeltaTime", runTime);
                            }

                            spritesheetRenderer.mesh->Draw(
                                spritesheetRenderer.customShader
                                        .usingCustomShader
                                    ? *spritesheetRenderer.customShader.shader
                                    : shader,
                                *Scene::mainCamera,
                                transform.transform * m_parentTransform);
                        }
                        glEnable(GL_CULL_FACE);
                    }

                    if (gameObject->HasComponent<SpriteAnimation>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto spriteAnimation =
                            gameObject->GetComponent<SpriteAnimation>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        glm::mat4 m_parentTransform = glm::mat4(1.0f);

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                parentTransform.Update();
                                m_parentTransform = parentTransform.transform;
                            }
                        }

                        spriteAnimation.Play();

                        if (spriteAnimation.currMesh != nullptr) {
                            spriteAnimation.currMesh->enttId =
                                (uint32_t)gameObject->entity;

                            if (spriteAnimation.customShader.shader != nullptr) {
                                spriteAnimation.customShader.shader->Bind();
                                spriteAnimation.customShader.shader->SetUniform1f("DeltaTime", runTime);
                            }

                            spriteAnimation.currMesh->Draw(
                                spriteAnimation.customShader.usingCustomShader
                                    ? *spriteAnimation.customShader.shader
                                    : shader,
                                *Scene::mainCamera,
                                transform.transform * m_parentTransform);
                            // glDisable(GL_BLEND);
                        }
                    }

                    if (gameObject->HasComponent<c_SpritesheetAnimation>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto spritesheetAnimation =
                            gameObject->GetComponent<c_SpritesheetAnimation>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        glm::mat4 m_parentTransform = glm::mat4(1.0f);

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                parentTransform.Update();
                                m_parentTransform = parentTransform.transform;
                            }
                        }

                        spritesheetAnimation.Play();
                        spritesheetAnimation.Update();

                        glActiveTexture(GL_TEXTURE21);
                        glBindTexture(GL_TEXTURE_CUBE_MAP,
                                      skybox.cubemapTexture);

                        glDisable(GL_CULL_FACE);
                        if (spritesheetAnimation.mesh != nullptr) {
                            spritesheetAnimation.mesh->enttId =
                                (uint32_t)gameObject->entity;

                            if (spritesheetAnimation.customShader.shader != nullptr) {
                                spritesheetAnimation.customShader.shader->Bind();
                                spritesheetAnimation.customShader.shader->SetUniform1f("DeltaTime", runTime);
                            }

                            spritesheetAnimation.mesh->Draw(
                                spritesheetAnimation.customShader
                                        .usingCustomShader
                                    ? *spritesheetAnimation.customShader.shader
                                    : shader,
                                *Scene::mainCamera,
                                transform.transform * m_parentTransform);
                            // glDisable(GL_BLEND);
                        }
                        glEnable(GL_CULL_FACE);
                    }
                }
            }
            glDisable(GL_BLEND);
            glDisable(GL_CULL_FACE);

            // Grid
            // clear depth buffer
            // glClear(GL_DEPTH_BUFFER_BIT);
            // if(Scene::mainCamera == camera) {
            //     Transform transform;
            //     transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
            //     transform.scale = glm::vec3(100.0f, 100.0f, 100.0f);
            //     gridMesh->Draw(gridShader, *camera, transform.transform);
            // }
            // // Grid

            for (uint32_t i = 0; i < 32; i++) {
                // unbind all texture
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            auto cameraView = Scene::m_Registry.view<CameraComponent>();
            for (auto entity : cameraView) {
                if (Scene::mainCamera == camera)
                    break;
                GameObject *gameObject;
                for (auto &go : Scene::m_GameObjects) {
                    if (go->entity == entity) {
                        gameObject = go;
                        break;
                    }
                }
                auto &camera = gameObject->GetComponent<CameraComponent>();

                if (!camera.depthCamera)
                    continue;
                glClear(GL_DEPTH_BUFFER_BIT);

                for (auto &layer : Scene::layers) {
                    bool notInCameraLayer = true;
                    for (auto &camLayer : camera.camera->layers) {
                        if (camLayer == layer.first) {
                            notInCameraLayer = false;
                            break;
                        }
                    }
                    if (notInCameraLayer)
                        continue;

                    for (auto &gameObject : Scene::m_GameObjects) {
                        if (!gameObject->enabled)
                            continue;
                        if (gameObject->layer != layer.first)
                            continue;

                        Transform parentTransform;
                        if (gameObject->parentID != "NO_PARENT") {
                            parentTransform = f_GameObject::FindGameObjectByID(
                                                  gameObject->parentID)
                                                  ->GetComponent<Transform>();
                        }
                        parentTransform.Update();

                        if (gameObject->HasComponent<MeshRenderer>()) {
                            auto meshRenderer =
                                gameObject->GetComponent<MeshRenderer>();
                            auto transform =
                                gameObject->GetComponent<Transform>();
                            transform.Update();

                            glm::mat4 extra = meshRenderer.extraMatrix;

                            if (meshRenderer.m_Mesh != nullptr) {
                                shader.Bind();
                                if (Scene::m_Object == gameObject) {
                                    glStencilFunc(GL_ALWAYS, 1, 0xFF);
                                    glStencilMask(0xFF);
                                }

                                meshRenderer.m_Mesh->Draw(
                                    shader, *camera.camera,
                                    transform.transform * extra *
                                        parentTransform.transform);
                            }
                        }

                        if (gameObject->HasComponent<SpriteRenderer>()) {
                            auto spriteRenderer =
                                gameObject->GetComponent<SpriteRenderer>();
                            auto transform =
                                gameObject->GetComponent<Transform>();
                            transform.Update();

                            for (auto &go : Scene::m_GameObjects) {
                                if (go->ID == gameObject->parentID &&
                                    go->HasComponent<Transform>()) {
                                    auto &parentTransform =
                                        go->GetComponent<Transform>();
                                    transform.position +=
                                        parentTransform.position;
                                    transform.rotation +=
                                        parentTransform.rotation;
                                    transform.scale *= parentTransform.scale;
                                }
                            }

                            spriteRenderer.mesh->Draw(shader, *camera.camera,
                                                      transform.transform);
                        }

                        if (gameObject->HasComponent<SpritesheetRenderer>()) {
                            auto spritesheetRenderer =
                                gameObject->GetComponent<SpritesheetRenderer>();
                            auto transform =
                                gameObject->GetComponent<Transform>();
                            transform.Update();

                            for (auto &go : Scene::m_GameObjects) {
                                if (go->ID == gameObject->parentID &&
                                    go->HasComponent<Transform>()) {
                                    auto &parentTransform =
                                        go->GetComponent<Transform>();
                                    transform.position +=
                                        parentTransform.position;
                                    transform.rotation +=
                                        parentTransform.rotation;
                                    transform.scale *= parentTransform.scale;
                                }
                            }

                            if (spritesheetRenderer.mesh != nullptr) {
                                spritesheetRenderer.mesh->Draw(
                                    shader, *camera.camera,
                                    transform.transform *
                                        parentTransform.transform);
                            }
                        }

                        if (gameObject->HasComponent<SpriteAnimation>()) {
                            auto spriteAnimation =
                                gameObject->GetComponent<SpriteAnimation>();
                            auto transform =
                                gameObject->GetComponent<Transform>();
                            transform.Update();

                            spriteAnimation.Play();

                            for (auto &go : Scene::m_GameObjects) {
                                if (go->ID == gameObject->parentID &&
                                    go->HasComponent<Transform>()) {
                                    auto &parentTransform =
                                        go->GetComponent<Transform>();
                                    transform.position +=
                                        parentTransform.position;
                                    transform.rotation +=
                                        parentTransform.rotation;
                                    transform.scale *= parentTransform.scale;
                                }
                            }

                            if (spriteAnimation.currMesh != nullptr) {
                                spriteAnimation.currMesh->Draw(
                                    shader, *camera.camera,
                                    transform.transform *
                                        parentTransform.transform);
                            }
                        }

                        if (gameObject
                                ->HasComponent<c_SpritesheetAnimation>()) {
                            auto spritesheetAnimation =
                                gameObject
                                    ->GetComponent<c_SpritesheetAnimation>();
                            auto transform =
                                gameObject->GetComponent<Transform>();
                            transform.Update();

                            for (auto &go : Scene::m_GameObjects) {
                                if (go->ID == gameObject->parentID &&
                                    go->HasComponent<Transform>()) {
                                    auto &parentTransform =
                                        go->GetComponent<Transform>();
                                    transform.position +=
                                        parentTransform.position;
                                    transform.rotation +=
                                        parentTransform.rotation;
                                    transform.scale *= parentTransform.scale;
                                }
                            }

                            spritesheetAnimation.Play();
                            spritesheetAnimation.Update();
                            if (spritesheetAnimation.mesh != nullptr) {
                                spritesheetAnimation.mesh->Draw(
                                    shader, *camera.camera,
                                    transform.transform *
                                        parentTransform.transform);
                            }
                        }
                    }
                }
            }
#ifndef GAME_BUILD
            if (Scene::mainCamera != camera)
                return;

            for (auto &gameObject : Scene::m_GameObjects) {
                if (!gameObject->enabled)
                    continue;
                if (Scene::m_Object != gameObject)
                    continue;

                if (gameObject->HasComponent<MeshRenderer>()) {
                    if (gameObject->GetComponent<MeshRenderer>().m_Mesh ==
                        nullptr)
                        break;
                    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                    glStencilMask(0x00);
                    glDisable(GL_DEPTH_TEST);

                    auto meshRenderer =
                        gameObject->GetComponent<MeshRenderer>();
                    auto transform = gameObject->GetComponent<Transform>();
                    transform.Update();

                    glm::mat4 extra = meshRenderer.extraMatrix;
                    outlineShader.Bind();
                    outlineShader.SetUniform1f("outlining", 0.08f);
                    if (transform.parentTransform != nullptr) {
                        transform.position +=
                            transform.parentTransform->position;
                        transform.rotation +=
                            transform.parentTransform->rotation;
                        transform.scale *= transform.parentTransform->scale;
                        transform.Update();
                    }

                    meshRenderer.m_Mesh->Draw(outlineShader, *Scene::mainCamera,
                                              transform.transform * extra);

                    glStencilMask(0xFF);
                    glStencilFunc(GL_ALWAYS, 0, 0xFF);
                    glEnable(GL_DEPTH_TEST);
                }
            }

            //        glClear(GL_DEPTH_BUFFER_BIT);
            //        if (drawBoxCollider2D) {
            //            glDepthFunc(GL_LEQUAL);
            //            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //
            //            glm::mat4 model = glm::mat4(1.0f);
            //            model = glm::translate(model, bc2dPos) *
            //                    glm::scale(model, glm::vec3(bc2dScale.x / 2,
            //                    bc2dScale.y / 2, 1.0f)) * glm::rotate(model,
            //                    glm::radians(90.0f), glm::vec3(1.0f, 0.0f,
            //                    0.0f));
            ////            mesh_BoxCollider2D.Draw(workerShader, *camera,
            /// model);
            //
            //            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            //        }

            for (auto &gameObject : Scene::m_GameObjects) {
                if (!gameObject->enabled)
                    continue;

                if (gameObject->HasComponent<c_DirectionalLight>()) {
                    for (auto &game_object : Scene::m_GameObjects) {
                        if (game_object->HasComponent<MeshRenderer>()) {
                            auto &meshRenderer =
                                game_object->GetComponent<MeshRenderer>();
                            if (meshRenderer.m_Mesh)
                                meshRenderer.m_Mesh->material.Unbind(shader);
                        }
                    }

                    auto &transform = gameObject->GetComponent<Transform>();
                    Transform t = transform;
                    t.scale = glm::vec3(-0.5f, 0.5f, 0.5f);

                    auto camTransform =
                        camera->GetComponent<TransformComponent>();
                    t.LookAt(camTransform.position);
                    t.Update();

                    glDisable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                    dirLightIconMesh.enttId = (uint32_t)gameObject->entity;
                    dirLightIconMesh.Draw(workerShader, *camera, t.transform);
                    glEnable(GL_DEPTH_TEST);
                }

                if (gameObject->HasComponent<Audio3D>()) {
                    for (auto &game_object : Scene::m_GameObjects) {
                        if (game_object->HasComponent<MeshRenderer>()) {
                            auto &meshRenderer =
                                game_object->GetComponent<MeshRenderer>();
                            if (meshRenderer.m_Mesh)
                                meshRenderer.m_Mesh->material.Unbind(shader);
                        }
                    }

                    auto &transform = gameObject->GetComponent<Transform>();
                    Transform t = transform;
                    t.scale = glm::vec3(-0.5f, 0.5f, 0.5f);

                    auto camTransform =
                        camera->GetComponent<TransformComponent>();
                    t.LookAt(camTransform.position);
                    t.Update();

                    glDisable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                    audioMesh.enttId = (uint32_t)gameObject->entity;
                    audioMesh.Draw(workerShader, *camera, t.transform);
                    glEnable(GL_DEPTH_TEST);
                }

                if (gameObject->HasComponent<c_PointLight>()) {
                    for (auto &game_object : Scene::m_GameObjects) {
                        if (game_object->HasComponent<MeshRenderer>()) {
                            auto &meshRenderer =
                                game_object->GetComponent<MeshRenderer>();
                            if (meshRenderer.m_Mesh)
                                meshRenderer.m_Mesh->material.Unbind(shader);
                        }
                    }

                    auto &transform = gameObject->GetComponent<Transform>();
                    Transform t = transform;
                    t.scale = glm::vec3(-0.5f, 0.5f, 0.5f);

                    auto camTransform =
                        camera->GetComponent<TransformComponent>();
                    t.LookAt(camTransform.position);
                    t.Update();

                    glDisable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                    pointLightIconMesh.enttId = (uint32_t)gameObject->entity;
                    pointLightIconMesh.Draw(workerShader, *camera, t.transform);
                    glEnable(GL_DEPTH_TEST);
                }

                if (gameObject->HasComponent<c_SpotLight>()) {
                    for (auto &game_object : Scene::m_GameObjects) {
                        if (game_object->HasComponent<MeshRenderer>()) {
                            auto &meshRenderer =
                                game_object->GetComponent<MeshRenderer>();
                            if (meshRenderer.m_Mesh)
                                meshRenderer.m_Mesh->material.Unbind(shader);
                        }
                    }

                    auto &transform = gameObject->GetComponent<Transform>();
                    Transform t = transform;
                    t.scale = glm::vec3(-0.5f, 0.5f, 0.5f);

                    auto camTransform =
                        camera->GetComponent<TransformComponent>();
                    t.LookAt(camTransform.position);
                    t.Update();

                    glDisable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                    spotLightIconMesh.enttId = (uint32_t)gameObject->entity;
                    spotLightIconMesh.Draw(workerShader, *camera, t.transform);
                    glEnable(GL_DEPTH_TEST);
                }

                if (gameObject->HasComponent<CameraComponent>()) {
                    for (auto &game_object : Scene::m_GameObjects) {
                        if (game_object->HasComponent<MeshRenderer>()) {
                            auto &meshRenderer =
                                game_object->GetComponent<MeshRenderer>();
                            if (meshRenderer.m_Mesh)
                                meshRenderer.m_Mesh->material.Unbind(shader);
                        }
                    }

                    auto &transform = gameObject->GetComponent<Transform>();
                    Transform t = transform;
                    t.scale = glm::vec3(-0.5f, 0.5f, 0.5f);

                    auto camTransform =
                        camera->GetComponent<TransformComponent>();
                    t.LookAt(camTransform.position);
                    t.Update();

                    glDisable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                    cameraIconMesh.enttId = (uint32_t)gameObject->entity;
                    cameraIconMesh.Draw(workerShader, *camera, t.transform);
                    glEnable(GL_DEPTH_TEST);
                }
            }
#endif
        },
        GUI_EXP,
        [&](Shader &m_shadowMapShader) {
            for (auto &layer : Scene::layers) {
                bool notInCameraLayer = true;
                for (auto &camLayer : Scene::mainCamera->layers) {
                    if (Scene::mainCamera == camera)
                        break;
                    if (camLayer == layer.first) {
                        notInCameraLayer = false;
                        break;
                    }
                }
                if (notInCameraLayer && Scene::mainCamera != camera)
                    continue;

                for (auto &gameObject : Scene::m_GameObjects) {
                    if (!gameObject->enabled)
                        continue;
                    if (gameObject->layer != layer.first)
                        continue;
                    if (gameObject->HasComponent<MeshRenderer>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto meshRenderer =
                            gameObject->GetComponent<MeshRenderer>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        glm::mat4 extra = meshRenderer.extraMatrix;

                        if (meshRenderer.m_Mesh != nullptr) {
                            m_shadowMapShader.Bind();

                            if (Scene::m_Object == gameObject) {
                                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                                glStencilMask(0xFF);
                            }
                            if (transform.parentTransform != nullptr) {
                                transform.position +=
                                    transform.parentTransform->position;
                                transform.rotation +=
                                    transform.parentTransform->rotation;
                                transform.scale *=
                                    transform.parentTransform->scale;
                                transform.Update();
                            }

                            meshRenderer.m_Mesh->enttId =
                                (uint32_t)gameObject->entity;
                            glActiveTexture(GL_TEXTURE21);
                            glBindTexture(GL_TEXTURE_CUBE_MAP,
                                          skybox.cubemapTexture);

                            if (meshRenderer.meshType == "Plane") {
                                glDisable(GL_CULL_FACE);
                                meshRenderer.m_Mesh->Draw(
                                    m_shadowMapShader, *Scene::mainCamera,
                                    transform.transform * extra);
                                glEnable(GL_CULL_FACE);
                            } else {
                                meshRenderer.m_Mesh->Draw(
                                    m_shadowMapShader, *Scene::mainCamera,
                                    transform.transform * extra);
                            }
                        }
                    }

                    if (gameObject->HasComponent<SpriteRenderer>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto spriteRenderer =
                            gameObject->GetComponent<SpriteRenderer>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                transform.position += parentTransform.position;
                                transform.rotation += parentTransform.rotation;
                                transform.scale *= parentTransform.scale;
                            }
                        }

                        glActiveTexture(GL_TEXTURE21);
                        glBindTexture(GL_TEXTURE_CUBE_MAP,
                                      skybox.cubemapTexture);

                        spriteRenderer.mesh->enttId =
                            (uint32_t)gameObject->entity;
                        glDisable(GL_CULL_FACE);
                        spriteRenderer.mesh->Draw(m_shadowMapShader,
                                                  *Scene::mainCamera,
                                                  transform.transform);
                        glEnable(GL_CULL_FACE);
                    }

                    if (gameObject->HasComponent<SpritesheetRenderer>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto spritesheetRenderer =
                            gameObject->GetComponent<SpritesheetRenderer>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                transform.position += parentTransform.position;
                                transform.rotation += parentTransform.rotation;
                                transform.scale *= parentTransform.scale;
                            }
                        }

                        glActiveTexture(GL_TEXTURE21);
                        glBindTexture(GL_TEXTURE_CUBE_MAP,
                                      skybox.cubemapTexture);

                        // disable face culling
                        glDisable(GL_CULL_FACE);
                        if (spritesheetRenderer.mesh != nullptr) {
                            spritesheetRenderer.mesh->enttId =
                                (uint32_t)gameObject->entity;
                            spritesheetRenderer.mesh->Draw(m_shadowMapShader,
                                                           *Scene::mainCamera,
                                                           transform.transform);
                        }
                        glEnable(GL_CULL_FACE);
                    }

                    if (gameObject->HasComponent<SpriteAnimation>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto spriteAnimation =
                            gameObject->GetComponent<SpriteAnimation>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        spriteAnimation.Play();

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                transform.position += parentTransform.position;
                                transform.rotation += parentTransform.rotation;
                                transform.scale *= parentTransform.scale;
                            }
                        }

                        if (spriteAnimation.currMesh != nullptr) {
                            spriteAnimation.currMesh->enttId =
                                (uint32_t)gameObject->entity;
                            spriteAnimation.currMesh->Draw(m_shadowMapShader,
                                                           *Scene::mainCamera);
                            // glDisable(GL_BLEND);
                        }
                    }

                    if (gameObject->HasComponent<c_SpritesheetAnimation>()) {
                        for (uint32_t i = 0; i < 32; i++) {
                            // unbind all texture
                            glActiveTexture(GL_TEXTURE0 + i);
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }

                        auto spritesheetAnimation =
                            gameObject->GetComponent<c_SpritesheetAnimation>();
                        auto transform = gameObject->GetComponent<Transform>();
                        transform.Update();

                        for (auto &go : Scene::m_GameObjects) {
                            if (go->ID == gameObject->parentID &&
                                go->HasComponent<Transform>()) {
                                auto &parentTransform =
                                    go->GetComponent<Transform>();
                                transform.position += parentTransform.position;
                                transform.rotation += parentTransform.rotation;
                                transform.scale *= parentTransform.scale;
                            }
                        }

                        spritesheetAnimation.Play();
                        spritesheetAnimation.Update();

                        glActiveTexture(GL_TEXTURE21);
                        glBindTexture(GL_TEXTURE_CUBE_MAP,
                                      skybox.cubemapTexture);

                        glDisable(GL_CULL_FACE);
                        if (spritesheetAnimation.mesh != nullptr) {
                            spritesheetAnimation.mesh->enttId =
                                (uint32_t)gameObject->entity;
                            spritesheetAnimation.mesh->Draw(
                                m_shadowMapShader, *Scene::mainCamera,
                                transform.transform);
                            // glDisable(GL_BLEND);
                        }
                        glEnable(GL_CULL_FACE);
                    }
                }
            }
        });

    free(scrollData);
    exit(0);

#ifndef _WIN32 || GAME_BUILD
    Discord_Shutdown();
#endif

    return 0;
}
#else
fs::path path;
int selected = -1;

void DisplayProject(GLFWwindow *window, const std::string &name,
                    const std::string &path, int index) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.055f, 0.051f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.07f, 0.075f, 0.071f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.1f, 0.105f, 0.11f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.03f, 0.5f));
    ImGui::SetCursorPosX(25);
    if (ImGui::Button(std::string(name + "\n" + path).c_str(),
                      ImVec2(ImGui::GetWindowWidth() - 50, 70))) {
#ifdef _WIN32
        // open build.exe
        // get cwd
        char cwd[1024];
        _getcwd(cwd, sizeof(cwd));
        std::string s_Cwd = cwd;

        std::cout << s_Cwd + "\\bin\\win_build.exe \"" + path + "\"" << std::endl;
        std::thread t(
            [&]() { system((".\\bin\\win_build.exe \"" + path + "\"").c_str()); });

        t.detach();
#else
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        std::string s_Cwd = cwd;

        std::thread t([&]() {
            system(std::string("./bin/build.out \"" + path + "\"").c_str());
        });
        t.detach();
#endif
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
        selected = index;
        ImGui::OpenPopup("Project Options");
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
}

int main() {
    char m_cwd[1024];
    getcwd(m_cwd, 1024);
    CsharpVariables::oldCwd = m_cwd;

    Hyper::Application app(
        1280, 720, "Vault Engine", false, true, false, [&]() {
            // get io
            auto &io = ImGui::GetIO();
            io.ConfigWindowsMoveFromTitleBarOnly = true;
            // io.Fonts->AddFontDefault();
            ImGui::StyleColorsDark();

            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            //        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

            io.FontDefault = io.Fonts->AddFontFromFileTTF(
                "assets/fonts/OpenSans-Semibold.ttf", 20.f);
            static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA,
                                                   0};
            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", 20.0f,
                                         &icons_config, icons_ranges);
        });

    GLFWimage images[1];
    images[0].pixels = stbi_load("build/ProjectIcon.png", &images[0].width,
                                 &images[0].height, 0, 4);
    glfwSetWindowIcon(app.renderer->window, 1, images);

    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.125f, 0.121f, 1.0f);

    colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

    colors[ImGuiCol_Button] = ImVec4(0.6f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1, 0.205f, 0.2f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1, 0.305f, 0.3f, 1.0f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3, 0.305f, 0.3f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

    colors[ImGuiCol_Tab] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.255f, 0.25f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

    colors[ImGuiCol_ResizeGrip] = ImVec4(1, 0.15, 0.15, 1);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(1, 0.30, 0.30, 1);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1, 0.20, 0.20, 1);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1, 0.15, 0.15, 1);

    colors[ImGuiCol_DockingPreview] = ImVec4(1, 0.15, 0.15, 1);

    ImGuiFileDialog::Instance()->SetFileStyle(
        IGFD_FileStyleByTypeFile, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(
        IGFD_FileStyleByTypeDir, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FOLDER);

    ImGui::GetStyle().ScrollbarSize = 10.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.5, 2.5));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.5f);

    struct Project {
        std::string name;
        std::string path;
    };

#ifdef _WIN32
    auto *logo = new Texture("build\\logo2.png", 0, "texture_diffuse");
    path = std::string(getenv("APPDATA"));
    path = path.parent_path() / "Roaming" / "Vault Engine";
    if (!fs::exists(path)) {
        fs::create_directories(path);

        std::ofstream fout(path / "projects.json");
        fout << "[]";
        fout.close();
    }
#else
    auto *logo = new Texture("build/logo2.png", 0, "texture_diffuse");
    // get HOME env
    fs::path home(getenv("HOME"));
    // create path
    path = home / ".vault_engine";
    if (!fs::exists(path))
        fs::create_directories(path);

    if (!fs::exists(path / "projects.json")) {
        std::ofstream fout(path / "projects.json");
        fout << "[]";
        fout.close();
    }
#endif

    std::vector<Project> projects;
    nlohmann::json j_Projects =
        nlohmann::json::parse(std::fstream(path / "projects.json"));
    for (auto &project : j_Projects) {
        projects.push_back({project["name"], project["path"]});
    }

    std::function<void(uint32_t &, uint32_t &, uint32_t &)> GUI =
        [&](uint32_t &one, uint32_t &two, uint32_t &three) {
            // dock it to the left
            glfwGetWindowSize(app.renderer->window, &app.width, &app.height);

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(app.width / 1.5, app.height));
            // flags
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse |
                                     ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                                     ImGuiWindowFlags_NoTitleBar;
            // padding
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(25, 25));
            if (ImGui::Begin(ICON_FA_LAYER_GROUP " Projects", nullptr, flags)) {
                if (ImGui::BeginChild("##icon stuff", ImVec2(0, 150))) {
                    ImGui::Image((void *)logo->tex->ID, ImVec2(150, 150),
                                 ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::EndChild();
                }
                ImGui::NewLine();

                if (ImGui::BeginPopup("Project Options")) {
                    if (ImGui::Button("Delete Project")) {
                        // delete project
                        std::ifstream fin(path / "projects.json");
                        nlohmann::json j = nlohmann::json::parse(fin);
                        projects.erase(projects.begin() + selected);
                        j.erase(j.begin() + selected);

                        std::ofstream fout(path / "projects.json");
                        fout << j.dump(4);
                        fout.close();

                        selected = -1;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                for (auto &project : projects) {
                    DisplayProject(app.renderer->window, project.name,
                                   project.path, &project - &projects[0]);
                }
            }
            ImGui::End();
            ImGui::PopStyleVar();

            // set the next window right next to the first one
            ImGui::SetNextWindowPos(ImVec2(app.width / 1.5, 0));
            ImGui::SetNextWindowSize(
                ImVec2(app.width - (app.width / 1.5), app.height));
            // flags
            ImGuiWindowFlags flags2 = ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoBringToFrontOnFocus |
                                      ImGuiWindowFlags_NoTitleBar;
            if (ImGui::Begin(ICON_FA_LAYER_GROUP " Other", nullptr, flags2)) {
                ImVec2 windowSize = ImGui::GetWindowSize();

                float offset = 25;

                ImGui::SetCursorPosX((offset + 2) / 2);
                if (ImGui::Button("New Project",
                                  ImVec2(windowSize.x - offset, 60))) {
                    ImGui::OpenPopup("Create New Project");
                }

                static char name[256];
                if (ImGui::BeginPopup("Create New Project")) {
                    ImGui::InputText("Project Name", name, 256);

                    if (ImGui::Button("Create Project")) {
                        ImGuiFileDialog::Instance()->OpenDialog(
                            "NewProject",
                            ICON_FA_FOLDER
                            " Choose where to create this project ..",
                            NULL, ".");
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (ImGuiFileDialog::Instance()->Display("NewProject")) {
                    // action if OK
                    if (ImGuiFileDialog::Instance()->IsOk()) {
                        std::string filePathName =
                            ImGuiFileDialog::Instance()->GetFilePathName();
                        std::string filePath =
                            ImGuiFileDialog::Instance()->GetCurrentPath();

                        std::ofstream projFile(filePath +
                                               "/settings.vault.project");

                        nlohmann::json data;
                        data["name"] = name;
                        data["path"] = filePathName;

                        std::ifstream fout(path / "projects.json");
                        nlohmann::json globalProjects =
                            nlohmann::json::parse(fout);
                        fout.close();

                        std::ofstream fout2(path / "projects.json");
                        globalProjects.push_back(data);

                        fout2 << globalProjects.dump(4);
                        fout2.close();

                        projFile << data.dump(4) << std::endl;
                        projFile.close();

                        projects.push_back({name, filePathName});
                        fs::path projectPath = filePathName;

                        // copy assets, shaders, build and imgui.ini to the
                        // project
                        fs::copy(fs::absolute("assets"), projectPath / "assets",
                                 fs::copy_options::recursive);
                        fs::copy(fs::absolute("shaders"),
                                 projectPath / "shaders",
                                 fs::copy_options::recursive);
                        fs::copy_file(fs::absolute("imgui.ini"),
                                      projectPath / "imgui.ini",
                                      fs::copy_options::recursive);
                    }

                    ImGuiFileDialog::Instance()->Close();
                }

                ImGui::SetCursorPosX((offset + 2) / 2);
                if (ImGui::Button("Open Project",
                                  ImVec2(windowSize.x - offset, 60))) {
                    ImGuiFileDialog::Instance()->OpenDialog(
                        "OpenProject",
                        ICON_FA_FOLDER " Choose a project to open ..", NULL,
                        ".");
                }

                if (ImGuiFileDialog::Instance()->Display("OpenProject")) {
                    // action if OK
                    if (ImGuiFileDialog::Instance()->IsOk()) {
                        std::string filePathName =
                            ImGuiFileDialog::Instance()->GetFilePathName();
                        std::string filePath =
                            ImGuiFileDialog::Instance()->GetCurrentPath();

                        if (fs::exists(fs::path(filePathName) /
                                       "settings.vault.project")) {
                            std::ifstream projFile(filePathName +
                                                   "/settings.vault.project");
                            nlohmann::json data =
                                nlohmann::json::parse(projFile);
                            projFile.close();

                            std::ifstream fout(path / "projects.json");
                            nlohmann::json globalProjects =
                                nlohmann::json::parse(fout);
                            fout.close();

                            globalProjects.push_back(data);
                            std::ofstream fout2(path / "projects.json");
                            fout2 << globalProjects.dump(4);
                            fout2.close();

                            projects.push_back({data["name"], data["path"]});
                        }
                    }

                    ImGuiFileDialog::Instance()->Close();
                }
            }
            ImGui::End();
        };

    app.Run([](uint32_t &shadowMap) {}, GUI, [](Shader &shader) {});
}
#endif
