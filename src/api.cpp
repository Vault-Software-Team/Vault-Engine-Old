#include "GLFW/glfw3.h"
#include "assimp/postprocess.h"
#include "imgui/imgui.h"
#include "lib/api.hpp"
#include "lib/scene.hpp"

// C++ Scripting Libraries
#ifndef _WIN32
#include <dlfcn.h>
#endif

#include <experimental/bits/fs_dir.h>
#include <experimental/bits/fs_ops.h>
#include <regex>
#include <sstream>

std::vector<HyperAPI::PointLight *> PointLights;
std::vector<HyperAPI::SpotLight *> SpotLights;
std::vector<HyperAPI::Light2D *> Lights2D;
std::vector<HyperAPI::DirectionalLight *> DirLights;
std::vector<HyperAPI::Mesh *> hyperEntities;

float rectangleVert[] = {
    1, -1, 1, 0, -1, -1, 0, 0, -1, 1, 0, 1,

    1, 1,  1, 1, 1,  -1, 1, 0, -1, 1, 0, 1,
};

float rotation = 0.0f;
double previousTime = glfwGetTime();

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

std::string get_file_contents(const char *file) {
    std::ifstream g_file(file);
    if (!g_file.is_open()) {
        std::cout << "Failed to open shader file: " << file << std::endl;
    }

    std::string content, line;
    while (getline(g_file, line)) {
        content += line + "\n";
    }

    return content;
}

void NewFrame(uint32_t FBO, int width, int height) {

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(pow(0.3f, 2.2f), pow(0.3f, 2.2f), pow(0.3f, 2.2f), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}

void EndFrame(HyperAPI::Shader &framebufferShader, HyperAPI::Renderer &renderer,
              uint32_t rectVAO, uint32_t postProcessingTexture,
              uint32_t postProcessingFBO, const int width, const int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    framebufferShader.Bind();
    framebufferShader.SetUniform1i("screenTexture", 15);
    glBindVertexArray(rectVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDepthFunc(GL_LEQUAL);
    if (renderer.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void EndEndFrame(HyperAPI::Shader &framebufferShader,
                 HyperAPI::Renderer &renderer, uint32_t rectVAO,
                 uint32_t postProcessingTexture, uint32_t postProcessingFBO,
                 uint32_t S_postProcessingTexture, uint32_t S_postProcessingFBO,
                 const int width, const int height, const int mouseX,
                 const int mouseY) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    NewFrame(S_postProcessingFBO, width, height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    framebufferShader.Bind();
    framebufferShader.SetUniform1i("screenTexture", 15);
    glBindVertexArray(rectVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    EndFrame(framebufferShader, renderer, rectVAO, S_postProcessingTexture,
             S_postProcessingFBO, width, height);

    glDepthFunc(GL_LEQUAL);
    if (renderer.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void SC_EndFrame(HyperAPI::Renderer &renderer, uint32_t FBO, uint32_t rectVAO,
                 uint32_t postProcessingTexture, uint32_t postProcessingFBO,
                 const int width, const int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool EndsWith(std::string const &value, std::string const &ending) {
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

namespace HyperAPI {
std::string cwd = "";
std::string dirPayloadData = "";
bool isRunning = false;
bool isStopped = true;
glm::vec3 mousePosWorld, mousePosCamWorld;
float sceneMouseX, sceneMouseY;
Config config = {
    "Vault Engine",
    "assets/scenes/main.vault",
    0.2,
};

bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation,
                        glm::vec3 &rotation, glm::vec3 &scale) {
    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return false;

    // First, isolate perspective.  This is the messiest.
    if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())) {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] =
            static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3], Pdum3;

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
    if (dot(Row[0], Pdum3) < 0)
    {
        for (length_t i = 0; i < 3; i++)
        {
            scale[i] *= static_cast<T>(-1);
            Row[i] *= static_cast<T>(-1);
        }
    }
#endif

    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0) {
        rotation.x = atan2(Row[1][2], Row[2][2]);
        rotation.z = atan2(Row[0][1], Row[0][0]);
    } else {
        rotation.x = atan2(-Row[2][0], Row[1][1]);
        rotation.z = 0;
    }

    return true;
}

namespace AudioEngine {
void PlaySound(const std::string &path, float volume, bool loop, int channel) {
    Mix_Chunk *chunk = Mix_LoadWAV(path.c_str());

    Mix_VolumeChunk(chunk, volume * 128);
    Mix_PlayChannel(channel, chunk, loop ? -1 : 0);
}

void StopSound(int channel) { Mix_HaltChannel(channel); }

void PlayMusic(const std::string &path, float volume, bool loop) {
    // generate chunk
    Mix_Music *music = Mix_LoadMUS(path.c_str());
    if (music == NULL) {
        HYPER_LOG("Failed to load music: " + path)
        return;
    }

    // set volume, the volume scale is 0 - 1
    Mix_VolumeMusic(volume * 128);

    Mix_PlayMusic(music, loop ? -1 : 0);
}

void StopMusic() { Mix_HaltMusic(); }
} // namespace AudioEngine

namespace BulletPhysicsWorld {
btDiscreteDynamicsWorld *dynamicsWorld;
btBroadphaseInterface *broadphase;
btDefaultCollisionConfiguration *collisionConfiguration;
btCollisionDispatcher *dispatcher;
btSequentialImpulseConstraintSolver *solver;
btAlignedObjectArray<btCollisionShape *> collisionShapes;
btAlignedObjectArray<btRigidBody *> rigidBodies;
btAlignedObjectArray<btPairCachingGhostObject *> ghostObjects;
btAlignedObjectArray<btTypedConstraint *> constraints;
btAlignedObjectArray<btCollisionObject *> collisionObjects;

void Delete() {
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

void Init() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver,
                                                collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
}

void UpdatePhysics() { dynamicsWorld->stepSimulation(1.f / 60.f, 10); }

void CollisionCallback(
    std::function<void(const std::string &, const std::string &)>
        HandleEntities) {
    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    // do collision callback on all rigid bodies
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold *contactManifold =
            dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const auto *obA =
            static_cast<const btCollisionObject *>(contactManifold->getBody0());
        const auto *obB =
            static_cast<const btCollisionObject *>(contactManifold->getBody1());

        // get the entity names
        auto *entityA = static_cast<std::string *>(obA->getUserPointer());
        auto *entityB = static_cast<std::string *>(obB->getUserPointer());

        // call the callback
        HandleEntities(*entityA, *entityB);
    }
}
} // namespace BulletPhysicsWorld

namespace Timestep {
float deltaTime = 0;
float lastFrame = 0;
float currentFrame = 0;
} // namespace Timestep

Renderer::Renderer(int width, int height, const char *title, Vector2 g_gravity,
                   uint32_t samples, bool fullscreen, bool resizable,
                   bool wireframe) {
    this->samples = samples;
    this->wireframe = wireframe;

    // mix
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    if (!glfwInit()) {
        HYPER_LOG("Failed to initialize GLFW")
    }
    // //set verisons
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);
    // 3.1 version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // error callback
    glfwSetErrorCallback([](int error, const char *description) {
        HYPER_LOG("GLFW Error (" << error << "): " << description)
    });

    if (fullscreen) {
        // get monitor width and height
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        window = glfwCreateWindow(mode->width, mode->height, title,
                                  glfwGetPrimaryMonitor(), NULL);
    } else {
        window = glfwCreateWindow(width, height, title, NULL, NULL);
    }

    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    // set minimum size of a window;
    glfwSetWindowSizeLimits(window, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    // blur cubemap
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // add icon to window
    GLFWimage images[1];
    images[0].pixels =
        stbi_load("build/logo2.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);

    // glEnable(GL_FRAMEBUFFER_SRGB);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDepthFunc(GL_LESS);
}

std::pair<GLint, GLint> Renderer::GetVRamUsage() {
    GLint totalMemoryInKB = 0;
    glGetIntegerv(0x9048, &totalMemoryInKB);

    GLint curAvailMemoryInKB = 0;
    glGetIntegerv(0x9049, &curAvailMemoryInKB);

    // return in megabytes
    return {(totalMemoryInKB - curAvailMemoryInKB) / 1024,
            totalMemoryInKB / 1024};
}

void Renderer::Render(Camera &camera) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();

    glfwSwapBuffers(window);
}

Shader::Shader(const char *shaderPath, const std::string &shaderContent) {
    path = shaderPath;

    enum ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2 } type;

    std::string vertCode, fragCode, geometryCode, line;
    if (strcmp(shaderPath, "NULL_SHADER") == 0) {
        HYPER_LOG(std::string("Loading shader: ") + typeid(this).name());
        std::stringstream ss(shaderContent);
        while (getline(ss, line)) {
            if (line == "#shader vertex") {
                type = ShaderType::VERTEX;
            } else if (line == "#shader fragment") {
                type = ShaderType::FRAGMENT;
            } else if (line == "#shader geometry") {
                type = ShaderType::GEOMETRY;
            } else {
                switch (type) {
                case ShaderType::VERTEX:
                    vertCode += line + "\n";
                    break;
                case ShaderType::FRAGMENT:
                    fragCode += line + "\n";
                    break;
                case ShaderType::GEOMETRY:
                    geometryCode += line + "\n";
                    break;
                default:
                    break;
                }
            }
        }
    } else {
        HYPER_LOG(std::string("Loading shader: ") + shaderPath);
        std::ifstream shaderFile(shaderPath);
        if (!shaderFile.is_open()) {
            std::cout << "Failed to open shader file" << std::endl;
        }

        while (getline(shaderFile, line)) {
            if (line == "#shader vertex") {
                type = ShaderType::VERTEX;
            } else if (line == "#shader fragment") {
                type = ShaderType::FRAGMENT;
            } else if (line == "#shader geometry") {
                type = ShaderType::GEOMETRY;
            } else {
                switch (type) {
                case ShaderType::VERTEX:
                    vertCode += line + "\n";
                    break;
                case ShaderType::FRAGMENT:
                    fragCode += line + "\n";
                    break;
                case ShaderType::GEOMETRY:
                    geometryCode += line + "\n";
                    break;
                default:
                    break;
                }
            }
        }
    }

    const char *vertShaderCode = vertCode.c_str();
    const char *fragShaderCode = fragCode.c_str();
    const char *geometryShaderCode = geometryCode.c_str();
    uint32_t vertShader, fragShader, geometryShader;
    int success;
    char infoLog[512];

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderCode, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        HYPER_LOG("Failed to compile Vertex Shader")
        std::cout << infoLog << std::endl;
    }

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderCode, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        HYPER_LOG("Failed to compile Fragment Shader")
        std::cout << infoLog << std::endl;
    }

    if (type == ShaderType::GEOMETRY) {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometryShaderCode, NULL);
        glCompileShader(geometryShader);
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
            HYPER_LOG("Failed to compile Geometry Shader")
            std::cout << infoLog << std::endl;
        }
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertShader);
    glAttachShader(ID, fragShader);
    if (type == 2) {
        glAttachShader(ID, geometryShader);
    }
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        HYPER_LOG("Failed to link program")
        std::cout << infoLog << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    if (type == ShaderType::GEOMETRY) {
        glDeleteShader(geometryShader);
    }
}

void Shader::Bind() { glUseProgram(ID); }

void Shader::Unbind() { glUseProgram(0); }

void Shader::SetUniform1f(const char *name, float value) {
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::SetUniform1i(const char *name, int value) {
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetUniform1ui(const char *name, uint32_t value) {
    glUniform1ui(glGetUniformLocation(ID, name), value);
}

void Shader::SetUniform2f(const char *name, float value1, float value2) {
    glUniform2f(glGetUniformLocation(ID, name), value1, value2);
}

void Shader::SetUniform3f(const char *name, float value1, float value2,
                          float value3) {
    glUniform3f(glGetUniformLocation(ID, name), value1, value2, value3);
}

void Shader::SetUniform4f(const char *name, float value1, float value2,
                          float value3, float value4) {
    glUniform4f(glGetUniformLocation(ID, name), value1, value2, value3, value4);
}

void Shader::SetUniformMat4(const char *name, glm::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE,
                       glm::value_ptr(value));
}

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices,
           Material &material, bool empty, bool batched) {

    TransformComponent component;
    component.position = Vector3(0, 0, 0);
    component.scale = Vector3(1, 1, 1);
    this->Components.push_back(component);

    this->vertices = vertices;
    this->indices = indices;
    this->material = material;
    this->ID = uuid::generate_uuid_v4();
    this->empty = empty;

    // calculate TBN
    for (int i = 0; i < indices.size(); i += 3) {
        Vertex &v1 = vertices[indices[i]];
        Vertex &v2 = vertices[indices[i + 1]];
        Vertex &v3 = vertices[indices[i + 2]];
        if (v1.tangent.x != -435.0f && v2.tangent.x != -435.0f &&
            v3.tangent.x != -435.0f)
            continue;

        glm::vec3 edge1 = v2.position - v1.position;
        glm::vec3 edge2 = v3.position - v1.position;
        glm::vec2 deltaUV1 = v2.texUV - v1.texUV;
        glm::vec2 deltaUV2 = v3.texUV - v1.texUV;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        v1.tangent = tangent;
        v2.tangent = tangent;
        v3.tangent = tangent;

        v1.bitangent = bitangent;
        v2.bitangent = bitangent;
        v3.bitangent = bitangent;
    }

    if (empty) {
        return;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    glBindVertexArray(VAO);

    if (!batched) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr,
                     GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
                     indices.data(), GL_STATIC_DRAW);

        // coords
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)0);
        glEnableVertexAttribArray(0);

        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        // normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);

        // texuv
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, texUV));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 4, GL_INT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, m_BoneIDs));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, m_Weights));
        glEnableVertexAttribArray(5);

        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(6);

        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(7);
        // error check
        if (glGetError() != GL_NO_ERROR) {
            HYPER_LOG("Error creating mesh");
        }

        // glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        // (void*)(11 * sizeof(float))); glEnableVertexAttribArray(4);

        // // add mat4
        // glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        // (void*)(12 * sizeof(float))); glEnableVertexAttribArray(5);
        // glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        // (void*)(16 * sizeof(float))); glEnableVertexAttribArray(6);
        // glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        // (void*)(20 * sizeof(float))); glEnableVertexAttribArray(7);
        // glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        // (void*)(24 * sizeof(float))); glEnableVertexAttribArray(8);
        // //divisors
        // glVertexAttribDivisor(5, 1);
        // glVertexAttribDivisor(6, 1);
        // glVertexAttribDivisor(7, 1);
        // glVertexAttribDivisor(8, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr,
                     GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
                     indices.data(), GL_STATIC_DRAW);

        // coords
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)0);
        glEnableVertexAttribArray(0);

        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // texuv
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)(9 * sizeof(float)));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)(11 * sizeof(float)));
        glEnableVertexAttribArray(4);

        // add mat4
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)(12 * sizeof(float)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)(16 * sizeof(float)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)(20 * sizeof(float)));
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)(24 * sizeof(float)));
        glEnableVertexAttribArray(8);
        // divisors
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
        glVertexAttribDivisor(8, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // scriptComponent.componentSystem = this;
}

void Mesh::Draw(Shader &shader, Camera &camera, glm::mat4 matrix,
                glm::vec3 translation, glm::quat rotation, glm::vec3 scale) {
    material.Bind(shader);
    camera.Matrix(shader, "camera");

    previousTime = (float)glfwGetTime();
    if (camera.EnttComp) {
        auto &cameraTransform =
            Scene::m_Registry.get<Experimental::Transform>(camera.entity);
        shader.SetUniform3f("cameraPosition", cameraTransform.position.x,
                            cameraTransform.position.y,
                            cameraTransform.position.z);
    } else {
        TransformComponent cameraTransform =
            camera.GetComponent<TransformComponent>();
        shader.SetUniform3f("cameraPosition", cameraTransform.position.x,
                            cameraTransform.position.y,
                            cameraTransform.position.z);
    }
    shader.SetUniform1i("cubeMap", 20);
    shader.SetUniform1ui("u_EntityID", enttId);

    // scriptComponent.OnUpdate();

    TransformComponent component;
    component.scale = Vector3(1);

    model = glm::translate(glm::mat4(1.0f), component.position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f),
                       Vector3(component.scale.x * 0.5, component.scale.y * 0.5,
                               component.scale.z * 0.5));

    model = matrix * model;

    glm::mat4 trans = glm::mat4(1);
    glm::mat4 rot = glm::mat4(1);
    glm::mat4 sca = glm::mat4(1);

    component.transform =
        glm::translate(glm::mat4(1.0f), component.position) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.x),
                    glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.y),
                    glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.z),
                    glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::scale(glm::mat4(1.0f), component.scale) * matrix;

    model = component.transform;
    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    shader.SetUniformMat4("model", model);
    shader.SetUniformMat4("translation", trans);
    shader.SetUniformMat4("rotation", rot);
    shader.SetUniformMat4("scale", sca);

    for (int i = 0; i < Scene::PointLights.size(); i++) {
        shader.SetUniform3f(
            ("pointLights[" + std::to_string(i) + "].lightPos").c_str(),
            Scene::PointLights[i]->lightPos.x,
            Scene::PointLights[i]->lightPos.y,
            Scene::PointLights[i]->lightPos.z);
        shader.SetUniform3f(
            ("pointLights[" + std::to_string(i) + "].color").c_str(),
            Scene::PointLights[i]->color.x, Scene::PointLights[i]->color.y,
            Scene::PointLights[i]->color.z);
        shader.SetUniform1f(
            ("pointLights[" + std::to_string(i) + "].intensity").c_str(),
            Scene::PointLights[i]->intensity);
    }
    if (Scene::PointLights.size() == 0) {
        for (int i = 0; i < 100; i++) {
            shader.SetUniform3f(
                ("pointLights[" + std::to_string(i) + "].lightPos").c_str(), 0,
                0, 0);
            shader.SetUniform3f(
                ("pointLights[" + std::to_string(i) + "].color").c_str(), 0, 0,
                0);
            shader.SetUniform1f(
                ("pointLights[" + std::to_string(i) + "].intensity").c_str(),
                0);
        }
    }

    for (int i = 0; i < Scene::SpotLights.size(); i++) {
        // Scene::SpotLights[i]->scriptComponent.OnUpdate();
        shader.SetUniform3f(
            ("spotLights[" + std::to_string(i) + "].lightPos").c_str(),
            Scene::SpotLights[i]->lightPos.x, Scene::SpotLights[i]->lightPos.y,
            Scene::SpotLights[i]->lightPos.z);
        shader.SetUniform3f(
            ("spotLights[" + std::to_string(i) + "].color").c_str(),
            Scene::SpotLights[i]->color.x, Scene::SpotLights[i]->color.y,
            Scene::SpotLights[i]->color.z);
        // shader.SetUniform1f(("spotLights[" + std::to_string(i) +
        // "].outerCone").c_str(), Scene::SpotLights[i]->outerCone);
        // shader.SetUniform1f(("spotLights[" + std::to_string(i) +
        // "].innerCone").c_str(), Scene::SpotLights[i]->innerCone);
        shader.SetUniform3f(
            ("spotLights[" + std::to_string(i) + "].angle").c_str(),
            Scene::SpotLights[i]->angle.x, Scene::SpotLights[i]->angle.y,
            Scene::SpotLights[i]->angle.z);
    }
    if (Scene::SpotLights.size() == 0) {
        for (int i = 0; i < 100; i++) {
            shader.SetUniform3f(
                ("spotLights[" + std::to_string(i) + "].lightPos").c_str(), 0,
                0, 0);
            shader.SetUniform3f(
                ("spotLights[" + std::to_string(i) + "].color").c_str(), 0, 0,
                0);
            shader.SetUniform1f(
                ("spotLights[" + std::to_string(i) + "].outerCone").c_str(), 0);
            shader.SetUniform1f(
                ("spotLights[" + std::to_string(i) + "].innerCone").c_str(), 0);
        }
    }

    for (int i = 0; i < Scene::DirLights.size(); i++) {
        // Scene::DirLights[i]->scriptComponent.OnUpdate();
        shader.SetUniform3f(
            ("dirLights[" + std::to_string(i) + "].lightPos").c_str(),
            Scene::DirLights[i]->lightPos.x, Scene::DirLights[i]->lightPos.y,
            Scene::DirLights[i]->lightPos.z);
        shader.SetUniform3f(
            ("dirLights[" + std::to_string(i) + "].color").c_str(),
            Scene::DirLights[i]->color.x, Scene::DirLights[i]->color.y,
            Scene::DirLights[i]->color.z);
        shader.SetUniform1f(
            ("dirLights[" + std::to_string(i) + "].intensity").c_str(),
            Scene::DirLights[i]->intensity);
    }
    if (Scene::DirLights.size() == 0) {
        for (int i = 0; i < 100; i++) {
            shader.SetUniform3f(
                ("dirLights[" + std::to_string(i) + "].lightPos").c_str(), 0, 0,
                0);
            shader.SetUniform3f(
                ("dirLights[" + std::to_string(i) + "].color").c_str(), 0, 0,
                0);
            shader.SetUniform1f(
                ("dirLights[" + std::to_string(i) + "].intensity").c_str(), 0);
        }
    }

    for (int i = 0; i < Scene::Lights2D.size(); i++) {
        shader.SetUniform2f(
            ("light2ds[" + std::to_string(i) + "].lightPos").c_str(),
            Scene::Lights2D[i]->lightPos.x, Scene::Lights2D[i]->lightPos.y);
        shader.SetUniform3f(
            ("light2ds[" + std::to_string(i) + "].color").c_str(),
            Scene::Lights2D[i]->color.x, Scene::Lights2D[i]->color.y,
            Scene::Lights2D[i]->color.z);
        shader.SetUniform1f(
            ("light2ds[" + std::to_string(i) + "].range").c_str(),
            Scene::Lights2D[i]->range);
    }
    if (Scene::Lights2D.size() == 0) {
        for (int i = 0; i < 100; i++) {
            shader.SetUniform2f(
                ("light2ds[" + std::to_string(i) + "].lightPos").c_str(), 0, 0);
            shader.SetUniform3f(
                ("light2ds[" + std::to_string(i) + "].color").c_str(), 0, 0, 0);
            shader.SetUniform1f(
                ("light2ds[" + std::to_string(i) + "].range").c_str(), 0);
        }
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(),
                    vertices.data());
    glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()),
                   GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    material.Unbind(shader);
}

Texture::Texture(const char *texturePath, uint32_t slot,
                 const char *textureType) {
    stbi_set_flip_vertically_on_load(true);
    texType = textureType;
    texStarterPath = texturePath;
    this->slot = slot;
    texPath = std::string(texturePath);
    data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    HYPER_LOG("Texture " + std::to_string(slot) + " loaded from " + texturePath)

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (std::string(textureType) == "texture_normal") {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
    } else if (std::string(textureType) == "texture_height") {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
    } else if (nrChannels >= 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
    else if (nrChannels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
    else if (nrChannels == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RED,
                     GL_UNSIGNED_BYTE, data);
    else
        return;

    // throw std::invalid_argument("Texture format not supported");

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::Texture(unsigned char *m_Data, uint32_t slot, const char *textureType,
                 const char *texturePath) {
    stbi_set_flip_vertically_on_load(true);
    texType = textureType;
    texStarterPath = texturePath;
    this->slot = slot;
    texPath = std::string(texturePath);
    data = stbi_load_from_memory(data, 0, &width, &height, &nrChannels, 0);

    HYPER_LOG("Texture " + std::to_string(slot) + " loaded from " + texturePath)

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (std::string(textureType) == "texture_normal") {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
    } else if (std::string(textureType) == "texture_height") {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
    } else if (nrChannels >= 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
    else if (nrChannels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
    else if (nrChannels == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RED,
                     GL_UNSIGNED_BYTE, data);
    else
        return;

    // throw std::invalid_argument("Texture format not supported");

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(uint32_t slot) {
    if (slot == -1) {
        glActiveTexture(GL_TEXTURE0 + this->slot);
        glBindTexture(GL_TEXTURE_2D, ID);
    } else {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);
    }
}

void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

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
        transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);
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

        if (mode2D) {
            float target_width = 1280;
            float target_height = 720;
            float A = target_width / target_height;
            float V = width / height;

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
        //            glm::vec3 mouseWorld = glm::unProject(glm::vec3(mouseX *
        //            mouseSensitivity, mouseY * mouseSensitivity, 0.0f), view,
        //            projection, glm::vec4(0, 0, width, height));
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
        //            mousePosCam.w; mousePosCamWorld = glm::vec3(mousePosCam.x,
        //            mousePosCam.y, mousePosCam.z); mousePosCamWorld =
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
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE,
                       glm::value_ptr(camMatrix));
    shader.SetUniformMat4("cam_view", view);
    shader.SetUniformMat4("cam_projection", projection);
}

void Camera::ControllerCameraMove(GLFWwindow *window) {
    if (!EnttComp)
        return;

    auto &transform = Scene::m_Registry.get<Experimental::Transform>(entity);

    if (!mode2D) {
        rotX = Input::Controller::GetRightAnalogY();
        rotY = Input::Controller::GetRightAnalogX();

        if (rotX < 0.1f && rotX > -0.2f) {
            rotX = 0.0f;
        }

        if (rotY < 0.1f && rotY > -0.2f) {
            rotY = 0.0f;
        }

        glm::vec3 newOrientation = glm::rotate(
            transform.rotation, glm::radians(-(rotX * controllerSensitivity)),
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

    auto &transform = Scene::m_Registry.get<Experimental::Transform>(entity);
    if (!mode2D) {
        glfwSetInputMode(Input::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
#ifndef GAME_BUILD
        if (glfwGetMouseButton(Input::window, GLFW_MOUSE_BUTTON_LEFT) ==
            GLFW_PRESS) {
            glfwSetInputMode(Input::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            if (firstClick) {
                glfwSetCursorPos(Input::window, (width / 2), (height / 2));
                firstClick = false;
            }

            double mouseX;
            double mouseY;
            glfwGetCursorPos(Input::window, &mouseX, &mouseY);

            rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
            rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

            glm::vec3 newOrientation =
                glm::rotate(transform.rotation, glm::radians(-rotX),
                            glm::normalize(glm::cross(transform.rotation, Up)));

            if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <=
                glm::radians(85.0f)) {
                transform.rotation = newOrientation;
            }

            transform.rotation =
                glm::rotate(transform.rotation, glm::radians(-rotY), Up);

            glfwSetCursorPos(Input::window, (width / 2), (height / 2));
        } else if (glfwGetMouseButton(Input::window, GLFW_MOUSE_BUTTON_LEFT) ==
                   GLFW_RELEASE) {
            glfwSetInputMode(Input::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
            speed = 0.4f;
        } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
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

                rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
                rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

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

                glfwSetCursorPos(window, (width / 2), (height / 2));
            } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
                       GLFW_RELEASE) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                // glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y +
                // (height / 2));
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
            speed = 0.4f;
        } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
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

                rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
                rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
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

                glfwSetCursorPos(window, (width / 2), (height / 2));
            } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
                       GLFW_RELEASE) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                // glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y +
                // (height / 2));
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
            //                    std::cout << rotX << " " << rotY << std::endl;

            glm::vec3 newOrientation =
                glm::rotate(transform.rotation, glm::radians(-rotX),
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

void Model::Draw(Shader &shader, Camera &camera) {
    // scriptComponent.componentSystem = this;
    // scriptComponent.OnUpdate();
    for (uint32_t i = 0; i < meshes.size(); i++) {
        TransformComponent modelTransform = GetComponent<TransformComponent>();
        modelTransform.transform =
            glm::translate(glm::mat4(1.0f), modelTransform.position) *
            glm::rotate(glm::mat4(1.0f),
                        glm::radians(modelTransform.rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f),
                        glm::radians(modelTransform.rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f),
                        glm::radians(modelTransform.rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), Vector3(modelTransform.scale.x * 0.5,
                                                modelTransform.scale.y * 0.5,
                                                modelTransform.scale.z * 0.5));

        UpdateComponent(modelTransform);
        meshes[i]->Draw(shader, camera, transform * modelTransform.transform);
    }
}

void Model::loadModel(std::string path) {
    ID = uuid::generate_uuid_v4();

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        // get name
        std::string name = mesh->mName.C_Str();

        // get matricies
        aiMatrix4x4 aiTransform = node->mTransformation;
        transform = glm::mat4(1.0f);
        transform[0][0] = aiTransform.a1;
        transform[1][0] = aiTransform.b1;
        transform[2][0] = aiTransform.c1;
        transform[3][0] = aiTransform.d1;
        transform[0][1] = aiTransform.a2;
        transform[1][1] = aiTransform.b2;
        transform[2][1] = aiTransform.c2;
        transform[3][1] = aiTransform.d2;
        transform[0][2] = aiTransform.a3;
        transform[1][2] = aiTransform.b3;
        transform[2][2] = aiTransform.c3;
        transform[3][2] = aiTransform.d3;
        transform[0][3] = aiTransform.a4;
        transform[1][3] = aiTransform.b4;
        transform[2][3] = aiTransform.c4;
        transform[3][3] = aiTransform.d4;

        meshes.push_back(processMesh(mesh, scene, name));
    }
    // then do the same for each of its children
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh *Model::processMesh(aiMesh *mesh, const aiScene *scene,
                         const std::string &name) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                                    mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                                  mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) {
            vertex.texUV = glm::vec2(mesh->mTextureCoords[0][i].x,
                                     mesh->mTextureCoords[0][i].y);
        } else {
            vertex.texUV = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // indices
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    Texture *diffuse = nullptr;
    Texture *specular = nullptr;
    Texture *normal = nullptr;

    if (mesh->mMaterialIndex >= 0 && texturesEnabled) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(
            material, aiTextureType_DIFFUSE, "texture_diffuse");
        diffuse = &diffuseMaps[0];
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(
            material, aiTextureType_SPECULAR, "texture_specular");
        specular = &specularMaps[0];
        textures.insert(textures.end(), specularMaps.begin(),
                        specularMaps.end());
    }

    if (texturesEnabled) {
        Material material(Color);
        //            material.diffuse = diffuse;
        //            material.specular = specular;

        Mesh *ent = new Mesh(vertices, indices, material);
        ent->modelMesh = true;
        ent->name = name;
        return ent;
    } else {
        Material material(Color);
        Mesh *ent = new Mesh(vertices, indices, material);
        ent->modelMesh = true;
        ent->name = name;
        return ent;
    }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 std::string typeName) {
    std::vector<Texture> textures;
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        std::string texPath = directory + '/' + str.C_Str();
        for (uint32_t j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].texStarterPath,
                            texPath.c_str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) { // if texture hasn't been loaded already, load it
            std::string texPath = directory + '/' + str.C_Str();
            Texture texture(texPath.c_str(), i, typeName.c_str());
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
            currSlot++;
        }
    }
    return textures;
}

Skybox::Skybox(const std::string &right, const std::string &left,
               const std::string &top, const std::string &bottom,
               const std::string &front, const std::string &back) {
    shader = new Shader("shaders/skybox.glsl");
    facesCubemap.push_back(right);
    facesCubemap.push_back(left);
    facesCubemap.push_back(top);
    facesCubemap.push_back(bottom);
    facesCubemap.push_back(front);
    facesCubemap.push_back(back);

    HYPER_LOG("Skybox created")

    float skyboxVertices[] = {-1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
                              1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
                              -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                              1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f};

    uint32_t skyboxIndices[] = {// Right
                                1, 2, 6, 6, 5, 1,
                                // Left
                                0, 4, 7, 7, 3, 0,
                                // Top
                                4, 5, 6, 6, 7, 4,
                                // Bottom
                                0, 3, 2, 2, 1, 0,
                                // Back
                                0, 1, 5, 5, 4, 0,
                                // Front
                                3, 7, 6, 6, 2, 3};

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (uint32_t i = 0; i < 6; i++) {
        int width, height, nrChannels;

        unsigned char *data =
            stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: "
                      << facesCubemap[i] << std::endl;
            stbi_image_free(data);
        }
    }

    shader->Bind();
    shader->SetUniform1i("skybox", 20);
}

void Skybox::Draw(Camera &camera, int width, int height) {
    glDepthFunc(GL_LEQUAL);

    shader->Bind();
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    if (camera.EnttComp) {
        auto &transform =
            Scene::m_Registry.get<Experimental::Transform>(camera.entity);
        view = glm::mat4(glm::mat3(
            glm::lookAt(transform.position,
                        transform.position + transform.rotation, camera.Up)));
        projection = glm::perspective(
            glm::radians(45.0f), (float)width / (float)height, 0.1f, 5000.0f);
    } else {
        TransformComponent transform =
            camera.GetComponent<TransformComponent>();
        view = glm::mat4(glm::mat3(
            glm::lookAt(transform.position,
                        transform.position + transform.rotation, camera.Up)));
        projection = glm::perspective(
            glm::radians(45.0f), (float)width / (float)height, 0.1f, 5000.0f);
    }

    shader->SetUniformMat4("view", view);
    shader->SetUniformMat4("projection", projection);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE20);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthFunc(GL_LESS);
}

void Renderer::NewFrame() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::Swap(Shader &framebufferShader, uint32_t FBO, uint32_t rectVAO,
                    uint32_t postProcessingTexture,
                    uint32_t postProcessingFBO) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebufferShader.Bind();
    glBindVertexArray(rectVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDepthFunc(GL_LEQUAL);
}

Sprite::Sprite(const char *texPath) {
    // square vertex
    std::vector<Vertex> vertices = {
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f)}};

    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    Material material(Vector4(1, 1, 1, 1),
                      {Texture(texPath, 0, "texture_diffuse")});
    m_Mesh = new Mesh(vertices, indices, material);
}

void Sprite::Draw(Shader &shader, Camera &camera, glm::mat4 trans) {
    m_Mesh->Draw(shader, camera, trans);
}

Spritesheet::Spritesheet(const char *texPath, Material &mat, Vector2 sheetSize,
                         Vector2 spriteSize, Vector2 spriteCoords) {
    // square vertex
    float xCoord = spriteCoords.x + spriteSize.x;
    float yCoord =
        (sheetSize.y - (spriteCoords.y + spriteSize.y)) + spriteSize.y;
    std::vector<Vector2> texCoords = {
        Vector2(spriteCoords.x / sheetSize.x,
                (sheetSize.y - (spriteCoords.y + spriteSize.y)) / sheetSize.y),
        Vector2(xCoord / sheetSize.x,
                (sheetSize.y - (spriteCoords.y + spriteSize.y)) / sheetSize.y),
        Vector2(xCoord / sheetSize.x, yCoord / sheetSize.y),
        Vector2(spriteCoords.x / sheetSize.x, yCoord / sheetSize.y)};

    std::vector<Vertex> vertices = {
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), texCoords[0]},

        Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), texCoords[1]},

        Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), texCoords[2]},

        Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
               glm::vec3(0, 1, 0), texCoords[3]}};

    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    // Material material(Vector4(1,1,1,1), { Texture(texPath, 0,
    // "texture_diffuse") });
    m_Mesh = new Mesh(vertices, indices, mat);
}

void Spritesheet::Draw(Shader &shader, Camera &camera) {
    m_Mesh->Draw(shader, camera);
}

Graphic::Graphic(Vector3 rgb) {
    std::vector<HyperAPI::Vertex> vertices = {
        {glm::vec3(-0.5, -0.5, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
         glm::vec2(0, 0)},
        {glm::vec3(-0.5, 0.5, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
         glm::vec2(0, 1)},
        {glm::vec3(0.5, 0.5, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
         glm::vec2(1, 1)},
        {glm::vec3(0.5, -0.5, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
         glm::vec2(1, 0)},
    };

    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    Material material(Vector4(rgb, 1));
    m_Mesh = new Mesh(vertices, indices, material);
}

void Graphic::Draw(Shader &shader, Camera &camera) {
    m_Mesh->Draw(shader, camera);
}

Capsule::Capsule(Vector4 color)
    : Model("assets/models/default/capsule/capsule.obj", false, color) {}

Cube::Cube(Vector4 color)
    : Model("assets/models/default/cube/cube.obj", false, color) {}

Plane::Plane(Vector4 color) {
    std::vector<HyperAPI::Vertex> vertices = {
        {glm::vec3(-0.5, 0, 0.5), glm::vec3(0.3, 0.3, 0.3), glm::vec3(0, 1, 0),
         glm::vec2(0, 0)},
        {glm::vec3(-0.5, 0, -0.5), glm::vec3(0.3, 0.3, 0.3), glm::vec3(0, 1, 0),
         glm::vec2(0, 1)},
        {glm::vec3(0.5, 0, -0.5), glm::vec3(0.3, 0.3, 0.3), glm::vec3(0, 1, 0),
         glm::vec2(1, 1)},
        {glm::vec3(0.5, 0, 0.5), glm::vec3(0.3, 0.3, 0.3), glm::vec3(0, 1, 0),
         glm::vec2(1, 0)}};

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

Material::Material(Vector4 baseColor, std::vector<Texture> textures,
                   float shininess, float metallic, float roughness) {
    this->baseColor = baseColor;
    this->shininess = shininess;
    this->metallic = metallic;
    this->textures = textures;
    this->roughness = roughness;

    for (auto &tex : textures) {
        if (std::string(tex.texType) == "texture_diffuse") {
            this->diffuse = &tex;
        } else if (std::string(tex.texType) == "texture_specular") {
            this->specular = &tex;
        } else if (std::string(tex.texType) == "texture_normal") {
            this->normal = &tex;
        }
    }

    if (diffuse != nullptr) {
        std::cout << "Diffuse: " << diffuse->texType << std::endl;
    }
}

void Material::Bind(Shader &shader) {
    // uint32_t diffuse = 0;
    // uint32_t specular = 0;
    // uint32_t normal = 0;

    shader.Bind();

    shader.SetUniform4f("baseColor", baseColor.x, baseColor.y, baseColor.z,
                        baseColor.w);
    shader.SetUniform1f("shininess", shininess);
    shader.SetUniform1f("metallic", metallic);
    shader.SetUniform1f("roughness", roughness);
    shader.SetUniform2f("texUvOffset", texUVs.x, texUVs.y);
    shader.SetUniform3f("u_BloomColor", bloomColor.x, bloomColor.y,
                        bloomColor.z);
    const uint32_t noEquippedSlot = 5;

    if (diffuse != nullptr) {
        diffuse->Bind(0);

        shader.SetUniform1i("isTex", 1);
        shader.SetUniform1i("texture_diffuse0", 0);
    } else {
        shader.SetUniform1i("isTex", 0);
    }

    if (specular != nullptr) {
        specular->Bind(1);
        shader.SetUniform1i("texture_specular0", 1);
    } else {
        shader.SetUniform1i("texture_specular0", noEquippedSlot);
    }

    if (normal != nullptr) {
        normal->Bind(2);
        shader.SetUniform1i("texture_normal0", 2);
        shader.SetUniform1i("hasNormalMap", 1);
    } else {
        shader.SetUniform1i("texture_normal0", noEquippedSlot);
        shader.SetUniform1i("hasNormalMap", 0);
    }

    if (height != nullptr) {
        height->Bind(3);
        shader.SetUniform1i("texture_height0", 3);
        shader.SetUniform1i("hasHeightMap", 1);
    } else {
        shader.SetUniform1i("texture_height0", noEquippedSlot);
        shader.SetUniform1i("hasHeightMap", 0);
    }

    if (emission != nullptr) {
        emission->Bind(4);
        shader.SetUniform1i("texture_emission0", 4);
    } else {
        shader.SetUniform1i("texture_emission0", noEquippedSlot);
    }
}

void Material::Unbind(Shader &shader) {
    if (diffuse != nullptr)
        diffuse->Unbind();

    if (specular != nullptr)
        specular->Unbind();

    if (normal != nullptr)
        normal->Unbind();

    shader.Unbind();
}

namespace Experimental {
nlohmann::json stateScene = nlohmann::json::array();
bool bulletPhysicsStarted = false;

void StartWorld(b2ContactListener *listener) {
    InitScripts();

    for (auto &gameObject : Scene::m_GameObjects) {
        if (gameObject->prefab)
            continue;

        if (gameObject->HasComponent<m_LuaScriptComponent>()) {
            gameObject->GetComponent<m_LuaScriptComponent>().Start();
        }

        if (gameObject->HasComponent<NativeScriptManager>()) {
            gameObject->GetComponent<NativeScriptManager>().Start();
        }

        if (gameObject->HasComponent<CppScriptManager>()) {
            gameObject->GetComponent<CppScriptManager>().Start();
        }

        if (gameObject->HasComponent<CsharpScriptManager>()) {
            gameObject->GetComponent<CsharpScriptManager>().Start();
        }
    }

    Scene::world = new b2World({0.0, -5.8f});
    Scene::world->SetContactListener(listener);
    auto view = Scene::m_Registry.view<Rigidbody2D>();
    auto view3D = Scene::m_Registry.view<Rigidbody3D>();

    for (auto e : view) {
        GameObject *gameObject;
        for (auto &go : Scene::m_GameObjects) {
            if (go->entity == e) {
                gameObject = go;
                break;
            }
        }

        auto &transform = gameObject->GetComponent<Transform>();
        auto &rb2d = gameObject->GetComponent<Rigidbody2D>();

        b2BodyDef bodyDef;
        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(gameObject);
        bodyDef.type = rb2d.type;
        bodyDef.position.Set(transform.position.x, transform.position.y);
        bodyDef.angle = glm::radians(transform.rotation.z);
        bodyDef.gravityScale = rb2d.gravityScale;

        b2Body *body = Scene::world->CreateBody(&bodyDef);
        body->SetFixedRotation(rb2d.fixedRotation);
        rb2d.body = body;

        if (gameObject->HasComponent<BoxCollider2D>()) {
            auto &boxCollider2D = gameObject->GetComponent<BoxCollider2D>();
            b2PolygonShape shape;
            shape.SetAsBox((((boxCollider2D.size.x) / 2) - 0.02) / 2,
                           (((boxCollider2D.size.y) / 2) - 0.02) / 2);

            b2FixtureDef fixtureDef;
            fixtureDef.isSensor = boxCollider2D.trigger;
            fixtureDef.shape = &shape;
            fixtureDef.density = boxCollider2D.density;
            fixtureDef.friction = boxCollider2D.friction;
            fixtureDef.restitution = boxCollider2D.restitution;
            fixtureDef.restitutionThreshold =
                boxCollider2D.restitutionThreshold;
            boxCollider2D.fixture = body->CreateFixture(&fixtureDef);
        }
    }

    BulletPhysicsWorld::Init();

    for (auto e : view3D) {
        GameObject *gameObject;
        for (auto &go : Scene::m_GameObjects) {
            if (go->entity == e) {
                gameObject = go;
                break;
            }
        }

        auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
        rigidbody.transform = &gameObject->GetComponent<Transform>();

        if (gameObject->HasComponent<BoxCollider3D>()) {
            auto &collider = gameObject->GetComponent<BoxCollider3D>();
            collider.CreateShape();
            rigidbody.CreateBody(collider.shape);
        }

        if (gameObject->HasComponent<MeshCollider3D>()) {
            auto &collider = gameObject->GetComponent<MeshCollider3D>();
            collider.CreateShape(&gameObject->GetComponent<MeshRenderer>());
            rigidbody.CreateBody(collider.shape);
        }
    }

    auto jointView = Scene::m_Registry.view<FixedJoint3D>();

    for (auto e : jointView) {
        GameObject *gameObject;
        for (auto &go : Scene::m_GameObjects) {
            if (go->entity == e) {
                gameObject = go;
                break;
            }
        }

        auto &joint = gameObject->GetComponent<FixedJoint3D>();
        joint.CreateJoint();
    }

    auto pathfindingView = Scene::m_Registry.view<PathfindingAI>();

    for (auto e : pathfindingView) {
        GameObject *gameObject;
        for (auto &go : Scene::m_GameObjects) {
            if (go->entity == e) {
                gameObject = go;
                break;
            }
        }

        auto &pathfinding = gameObject->GetComponent<PathfindingAI>();
        pathfinding.CreateGrid();
    }

    bulletPhysicsStarted = true;
}

void DeleteWorld() {
    bulletPhysicsStarted = false;

    // halt
    Mix_HaltChannel(-1);
    Mix_HaltMusic();

    for (auto &gameObject : Scene::m_GameObjects) {
        if (gameObject->HasComponent<FixedJoint3D>()) {
            auto &fixedJoint = gameObject->GetComponent<FixedJoint3D>();
            fixedJoint.DeleteJoint();
        }

        if (gameObject->HasComponent<NativeScriptManager>()) {
            auto &script = gameObject->GetComponent<NativeScriptManager>();
            for (auto &script : script.m_StaticScripts) {
                delete script;
            }

            gameObject->RemoveComponent<NativeScriptManager>();
        }

        if (gameObject->HasComponent<Rigidbody3D>()) {
            auto &component = gameObject->GetComponent<Rigidbody3D>();
            component.DeleteBody();
            if (component.ref) {
                delete component.ref;
            }
        }

        if (gameObject->HasComponent<PathfindingAI>()) {
            auto &component = gameObject->GetComponent<PathfindingAI>();
            component.DeleteGrid();
        }
    }

    for (auto instance : CsharpScriptEngine::instances) {
        if (instance.second != nullptr)
            delete instance.second;
    }
    CsharpScriptEngine::instances.clear();

    BulletPhysicsWorld::Delete();
}

void DrawVec3Control(const std::string &label, Vector3 &values,
                     float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 100);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    float lineHeight =
        GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};

    // disable button rounded corners
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    if (ImGui::Button("Z", buttonSize)) {
        values.z = resetValue;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Columns(1);
    ImGui::PopStyleVar(2);
    ImGui::PopID();
}

void DrawVec2Control(const std::string &label, Vector2 &values,
                     float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 100);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    float lineHeight =
        GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};

    // disable button rounded corners
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::Columns(1);
    ImGui::PopStyleVar(2);
    ImGui::PopID();
}

std::vector<m_SpritesheetAnimationData>
GetAnimationsFromXML(const char *texPath, float delay, Vector2 sheetSize,
                     const std::string &xmlFile) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(xmlFile.c_str());

    tinyxml2::XMLElement *root = doc.FirstChildElement("TextureAtlas");
    tinyxml2::XMLElement *subTexture = root->FirstChildElement("SubTexture");

    std::vector<m_SpritesheetAnimationData> animations;

    std::string currAnimation = "";
    m_SpritesheetAnimationData animationData;
    int animIndex = -1;
    // animationData has a Frame, which has size and offset, size is width and
    // height, offset is x and y

    while (subTexture) {
        std::string name = subTexture->Attribute("name");
        std::string x = subTexture->Attribute("x");
        std::string y = subTexture->Attribute("y");
        std::string width = subTexture->Attribute("width");
        std::string height = subTexture->Attribute("height");

        // remove last 4 digits???
        std::string animationName = name.substr(0, name.size() - 4);

        if (currAnimation != animationName) {
            animations.push_back(animationData);

            currAnimation = animationName;
            animationData.loop = true;
            animationData = {};
            animIndex = animations.size() - 1;
            strcpy(animationData.name, currAnimation.c_str());
            animationData.delay = delay;
        }

        m_SpritesheetAnimationData::Frame frame;
        frame.size = Vector2(std::stof(width), std::stof(height));
        frame.offset = Vector2(std::stof(x), std::stof(y));
        animationData.frames.push_back(frame);
        if (animIndex != -1) {
            animations[animIndex] = animationData;
        }

        subTexture = subTexture->NextSiblingElement("SubTexture");
    }

    return animations;
}

void Model::Draw(Shader &shader, Camera &camera,
                 std::vector<glm::mat4> &transforms) {
    Transform &mainTransform = mainGameObject->GetComponent<Transform>();
    mainTransform.Update();

    for (uint32_t i = 0; i < m_gameObjects.size(); i++) {
        MeshRenderer &meshRenderer =
            m_gameObjects[i]->GetComponent<MeshRenderer>();
        meshRenderer.m_Mesh->Draw(shader, camera, mainTransform.transform);
    }
}

void Model::loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(
        path, aiProcess_Triangulate | aiProcess_CalcTangentSpace |
                  aiProcess_PreTransformVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        // get name
        std::string name = mesh->mName.C_Str();

        // get matricies
        aiMatrix4x4 aiTransform = node->mTransformation;
        transform = glm::mat4(1.0f);
        transform[0][0] = aiTransform.a1;
        transform[1][0] = aiTransform.b1;
        transform[2][0] = aiTransform.c1;
        transform[3][0] = aiTransform.d1;
        transform[0][1] = aiTransform.a2;
        transform[1][1] = aiTransform.b2;
        transform[2][1] = aiTransform.c2;
        transform[3][1] = aiTransform.d2;
        transform[0][2] = aiTransform.a3;
        transform[1][2] = aiTransform.b3;
        transform[2][2] = aiTransform.c3;
        transform[3][2] = aiTransform.d3;
        transform[0][3] = aiTransform.a4;
        transform[1][3] = aiTransform.b4;
        transform[2][3] = aiTransform.c4;
        transform[3][3] = aiTransform.d4;

        m_gameObjects.push_back(processMesh(mesh, scene, name));
        auto &meshRenderer = m_gameObjects[i]->GetComponent<MeshRenderer>();
        meshRenderer.extraMatrix = transform;
    }
    // then do the same for each of its children
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

GameObject *Model::processMesh(aiMesh *mesh, const aiScene *scene,
                               const std::string &name) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture *> textures;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        SetVertexBoneDataToDefault(vertex);
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                                    mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                                  mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) {
            vertex.texUV = glm::vec2(mesh->mTextureCoords[0][i].x,
                                     mesh->mTextureCoords[0][i].y);
        } else {
            vertex.texUV = glm::vec2(0.0f, 0.0f);
        }
        vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y,
                                   mesh->mTangents[i].z);
        vertex.bitangent =
            glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y,
                      mesh->mBitangents[i].z);
        vertices.push_back(vertex);
    }
    ExtractBoneWeightForVertices(vertices, mesh, scene);

    // indices
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    Texture *diffuse = nullptr;
    Texture *specular = nullptr;
    Texture *normal = nullptr;

    if (mesh->mMaterialIndex >= 0 && texturesEnabled) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture *> diffuseMaps = loadMaterialTextures(
            material, aiTextureType_DIFFUSE, "texture_diffuse");
        diffuse = diffuseMaps[0];
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture *> specularMaps = loadMaterialTextures(
            material, aiTextureType_SPECULAR, "texture_specular");
        specular = specularMaps[0];
        textures.insert(textures.end(), specularMaps.begin(),
                        specularMaps.end());
    }

    if (texturesEnabled) {
        Material material(Color);
        material.diffuse = diffuse;
        material.specular = specular;

        GameObject *gameObject = new GameObject();
        gameObject->name = name;
        gameObject->parentID = mainGameObject->ID;
        gameObject->AddComponent<Transform>();
        gameObject->AddComponent<MeshRenderer>();

        MeshRenderer &meshRenderer = gameObject->GetComponent<MeshRenderer>();
        meshRenderer.m_Mesh = new Mesh(vertices, indices, material);
        meshRenderer.m_Model = true;
        meshRenderer.meshType = std::string(path);
        Scene::m_GameObjects.push_back(gameObject);

        return gameObject;
    } else {
        Material material(Color);
        material.diffuse = diffuse;
        material.specular = specular;

        GameObject *gameObject = new GameObject();
        gameObject->name = name;
        gameObject->parentID = mainGameObject->ID;
        gameObject->AddComponent<Transform>();
        gameObject->AddComponent<MeshRenderer>();

        MeshRenderer &meshRenderer = gameObject->GetComponent<MeshRenderer>();
        meshRenderer.m_Mesh = new Mesh(vertices, indices, material);
        meshRenderer.m_Model = true;
        meshRenderer.meshType = std::string(path);
        Scene::m_GameObjects.push_back(gameObject);

        return gameObject;
    }
}

std::vector<Texture *> Model::loadMaterialTextures(aiMaterial *mat,
                                                   aiTextureType type,
                                                   std::string typeName) {
    std::vector<Texture *> textures;
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        std::string texPath = directory + '/' + str.C_Str();
        for (uint32_t j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j]->texStarterPath,
                            texPath.c_str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) { // if texture hasn't been loaded already, load it
            std::string texPath = directory + '/' + str.C_Str();
            Texture *texture =
                new Texture(texPath.c_str(), i, typeName.c_str());
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
            currSlot++;
        }
    }
    return textures;
}
} // namespace Experimental

namespace f_GameObject {
Experimental::GameObject *FindGameObjectByName(const std::string &name) {
    for (auto &gameObject : Scene::m_GameObjects) {
        if (gameObject->name == name) {
            return gameObject;
        }
    }

    return nullptr;
}

Experimental::GameObject *FindGameObjectByTag(const std::string &tag) {
    for (auto &gameObject : Scene::m_GameObjects) {
        if (gameObject->tag == tag) {
            return gameObject;
        }
    }

    return nullptr;
}

Experimental::GameObject *FindGameObjectByID(const std::string &id) {
    for (auto &gameObject : Scene::m_GameObjects) {
        if (gameObject->ID == id) {
            return gameObject;
        }
    }

    return nullptr;
}

Experimental::GameObject *FindGameObjectByEntt(const entt::entity &entity) {
    for (auto &gameObject : Scene::m_GameObjects) {
        if (gameObject->entity == entity) {
            return gameObject;
        }
    }

    return nullptr;
}

Experimental::GameObject *InstantiatePrefab(const std::string &path) {
    return Scene::LoadPrefab(path);
}

Experimental::GameObject *
InstantiatePrefab(const std::string &path, Vector3 position = Vector3(0, 0, 0),
                  Vector3 rotation = Vector3(0, 0, 0)) {
    auto gameObject = Scene::LoadPrefab(path);
    gameObject->GetComponent<Experimental::Transform>().position = position;
    gameObject->GetComponent<Experimental::Transform>().rotation = rotation;

    return gameObject;
}
} // namespace f_GameObject

namespace n_Bloom {
bool BloomBuffer::Init(uint32_t windowWidth, uint32_t windowHeight,
                       uint32_t mipChainLength) {
    if (m_Init)
        return true;

    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
    glm::ivec2 mipIntSize((int)windowWidth, (int)windowHeight);
    // Safety check
    if (windowWidth > (uint32_t)INT_MAX || windowHeight > (uint32_t)INT_MAX) {
        std::cerr
            << "Window size conversion overflow - cannot build bloom FBO!\n";
        return false;
    }

    for (uint32_t i = 0; i < mipChainLength; i++) {
        BloomMip mip;

        mipSize *= 0.5f;
        mipIntSize /= 2;
        mip.size = mipSize;
        mip.intSize = mipIntSize;

        glGenTextures(1, &mip.texture);
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        // we are downscaling an HDR color buffer, so we need a float texture
        // format
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x,
                     (int)mipSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_MipChain.emplace_back(mip);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           m_MipChain[0].texture, 0);

    // setup attachments
    uint32_t attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, attachments);

    // check completion status
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        HYPER_LOG("Bloom FBO is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_Init = true;
    return true;
}

BloomBuffer::BloomBuffer() : m_Init(false) {}
BloomBuffer::~BloomBuffer() {}

void BloomBuffer::Destroy() {
    for (int i = 0; i < m_MipChain.size(); i++) {
        glDeleteTextures(1, &m_MipChain[i].texture);
        m_MipChain[i].texture = 0;
    }
    glDeleteFramebuffers(1, &m_Framebuffer);
    m_Framebuffer = 0;
    m_Init = false;
}

void BloomBuffer::BindWriting() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
}

bool BloomRenderer::Init(uint32_t windowWidth, uint32_t windowHeight) {
    if (mInit)
        return true;
    m_SrcViewportSize = glm::ivec2(windowWidth, windowHeight);
    m_SrcViewportSizeFloat = glm::vec2((float)windowWidth, (float)windowHeight);

    // Framebuffer
    const uint32_t num_bloom_mips = 5; // Experiment with this value
    bool status = m_Framebuffer.Init(windowWidth, windowHeight, num_bloom_mips);
    if (!status) {
        std::cerr << "Failed to initialize bloom FBO - cannot create bloom "
                     "renderer!\n";
        return false;
    }

    // Shaders
    m_DownsampleShader = new Shader("shaders/downsample.glsl");
    m_UpsampleShader = new Shader("shaders/upsample.glsl");

    // Downsample
    m_DownsampleShader->Bind();
    m_DownsampleShader->SetUniform1i("srcTexture", 0);

    // Upsample
    m_UpsampleShader->Bind();
    m_UpsampleShader->SetUniform1i("srcTexture", 0);

    mInit = true;
    return true;
}

BloomRenderer::BloomRenderer() : mInit(false) {}
BloomRenderer::~BloomRenderer() {}

void BloomRenderer::Destroy() {
    m_Framebuffer.Destroy();
    delete m_DownsampleShader;
    delete m_UpsampleShader;
    mInit = false;
}

void BloomRenderer::RenderBloomTexture(uint32_t srcTexture, float filterRadius,
                                       uint32_t &quadVAO) {
    m_Framebuffer.BindWriting();

    this->RenderDownsamples(srcTexture, quadVAO);
    this->RenderUpsamples(filterRadius, quadVAO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Restore viewport
    glViewport(0, 0, m_SrcViewportSize.x, m_SrcViewportSize.y);
}

uint32_t BloomRenderer::BloomTexture() {
    return m_Framebuffer.GetMipChain()[0].texture;
}

void BloomRenderer::RenderDownsamples(uint32_t srcTexture, uint32_t &quadVAO) {
    const std::vector<BloomMip> &mipChain = m_Framebuffer.GetMipChain();

    m_DownsampleShader->Bind();
    m_DownsampleShader->SetUniform2f("srcResolution", m_SrcViewportSizeFloat.x,
                                     m_SrcViewportSizeFloat.y);

    // Bind srcTexture (HDR color buffer) as initial texture input
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    // Progressively downsample through the mip chain
    for (int i = 0; i < mipChain.size(); i++) {
        const BloomMip &mip = mipChain[i];
        glViewport(0, 0, mip.size.x, mip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, mip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Set current mip resolution as srcResolution for next iteration
        m_DownsampleShader->SetUniform2f("srcResolution", mip.size.x,
                                         mip.size.y);
        // Set current mip as texture input for next iteration
        glBindTexture(GL_TEXTURE_2D, mip.texture);
    }
}

void BloomRenderer::RenderUpsamples(float filterRadius, uint32_t &quadVAO) {
    const std::vector<BloomMip> &mipChain = m_Framebuffer.GetMipChain();

    m_UpsampleShader->Bind();
    m_DownsampleShader->SetUniform1f("filterRadius", filterRadius);

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (int i = mipChain.size() - 1; i > 0; i--) {
        const BloomMip &mip = mipChain[i];
        const BloomMip &nextMip = mipChain[i - 1];

        // Bind viewport and texture from where to read
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Set framebuffer render target (we write to this texture)
        glViewport(0, 0, nextMip.size.x, nextMip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, nextMip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    // Disable additive blending
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was
    // default
    glDisable(GL_BLEND);
}
} // namespace n_Bloom

namespace CppScripting {
std::vector<SharedObject> cpp_scripts;

void LoadScripts() {
#ifndef _WIN32
    auto iter = fs::recursive_directory_iterator("assets");
    for (auto &dirEntry : iter) {
        if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
            std::string file = dirEntry.path();
            file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

            if (!fs::exists(file))
                continue;
            SharedObject sharedObj;

            sharedObj.handle = dlopen(file.c_str(), RTLD_LAZY);
            sharedObj.create =
                (Script * (*)()) dlsym(sharedObj.handle, "create_object");
            sharedObj.name = dirEntry.path().filename();
            cpp_scripts.push_back(sharedObj);
        }
    }
#endif
}

void CompileLinuxScripts() {
#ifndef _WIN32
    auto iter = fs::recursive_directory_iterator("assets");
    for (auto &dirEntry : iter) {
        if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
            std::string file = dirEntry.path();
            file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

            std::string objFile = dirEntry.path();
            objFile = std::regex_replace(objFile, std::regex("\\.cpp"), ".o");

            std::string headers =
                "-I\"./src/vendor\" -I\"./src/vendor/bullet/bullet\" "
                "-I\"./src/vendor/NoesisGUI\"";
            system((std::string(config.linuxCompiler) + " -c -fPIC " +
                    dirEntry.path().string() + " " + headers +
                    " -rdynamic -o " + objFile)
                       .c_str());

            system((std::string(config.linuxCompiler) + " -shared " + objFile +
                    " -o " + file)
                       .c_str());

            for (auto script : cpp_scripts) {
                dlclose(script.handle);
            }
            cpp_scripts.clear();

            HYPER_LOG("C++ Scripts have been compiled (Linux Compiler)")
            LoadScripts();
        }
    }
#endif
}

void CompileWindowsScripts() {
    auto iter = fs::recursive_directory_iterator("assets");
    for (auto &dirEntry : iter) {
        if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
            std::string file = dirEntry.path();
            file = std::regex_replace(file, std::regex("\\.cpp"), ".dll");

            std::string objFile = dirEntry.path();
            objFile = std::regex_replace(objFile, std::regex("\\.cpp"), ".o");

            std::string headers =
                "-I\"./src/vendor\" -I\"./src/vendor/bullet/bullet\" "
                "-I\"./src/vendor/NoesisGUI\"";
            headers += " ";
            headers += "-lstdc++fs -L\"./win_libs\" -lglfw3dll -lstdc++fs "
                       "-lluajit-5.1 -lbox2d -lassimp.dll -lfreetype "
                       "-lSDL2.dll -lSDL2_mixer.dll -ldiscord-rpc -ltinyxml2";
            headers +=
                " -lBulletDynamics.dll -lBulletCollision.dll -lLinearMath.dll";
            system((std::string(config.windowsCompiler) + " -c -DBUILD_DLL " +
                    dirEntry.path().string() + " src/api.cpp " + headers +
                    " -o " + objFile)
                       .c_str());

            system((std::string(config.windowsCompiler) + " " + headers +
                    " -shared -o " + file + " " + objFile +
                    " -Wl,--out-implib,libshared_lib.a")
                       .c_str());

            // for(auto script : cpp_scripts) {
            //     dlclose(script.handle);
            // }
            // cpp_scripts.clear();

            HYPER_LOG("C++ Scripts have been compiled (Windows Compiler)")
        }
    }
}
} // namespace CppScripting

#ifndef _WIN32
namespace Text {
Font::Font(const std::string &path, int size) : scale(size) {
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
                  << std::endl;
    }

    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                     face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<uint32_t>(face->glyph->advance.x)};

        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Font::Render(Shader &shader, Camera &camera, std::string text,
                  Experimental::Transform &transform) {

    shader.Bind();
    shader.SetUniform3f("textColor", 1.0f, 1.0f, 1.0f);
    shader.SetUniformMat4("camera", camera.camMatrix);
    //            transform.Update();
    //            shader.SetUniformMat4("transform", transform.transform);

    glActiveTexture(GL_TEXTURE0);

    float x = 0;
    float y = 0;

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * 0.5;
        float h = ch.Size.y * 0.5;
        // update VBO for each character
        float vertices[6][4] = {
            // square vertices, do not use xpos and ypos
            {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},

            {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f, 0.0f}};
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        shader.SetUniform1i("text", 0);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += (ch.Advance >> 6) *
             scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace Text
#endif
} // namespace HyperAPI

namespace Hyper {
void Application::Run(
    std::function<void(uint32_t &)> update,
    std::function<void(uint32_t &PPT, uint32_t &PPFBO, uint32_t &gui_gui)> gui,
    std::function<void(HyperAPI::Shader &)> shadowMapRender) {
    HYPER_LOG("Application started")

    float gamma = 2.2f;
    // if (renderOnScreen) {
    // glEnable(GL_FRAMEBUFFER_SRGB);
    // }

    HyperAPI::Shader shadowMapProgram("shaders/shadowMap.glsl");
    HyperAPI::Shader framebufferShader("shaders/framebuffer.glsl");
    HyperAPI::Shader blurShader("shaders/blur.glsl");

    framebufferShader.Bind();
    framebufferShader.SetUniform1f("gamma", gamma);
    // HyperAPI::Shader shader("shaders/sprite.glsl");
    // shader.Bind();
    // shader.SetUniform1f("ambient", 0.5);

    glfwSwapInterval(1);

    uint32_t rectVAO, rectVBO;
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    // dynamic
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr,
                 GL_DYNAMIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVert), &rectangleVert,
    // GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));

    uint32_t postProcessingFBO;
    glGenFramebuffers(1, &postProcessingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

    uint32_t postProcessingTexture;
    glGenTextures(1, &postProcessingTexture);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           postProcessingTexture, 0);

    uint32_t bloomTexture;
    glGenTextures(1, &bloomTexture);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           bloomTexture, 0);

    // r32i entityTexture
    uint32_t entityTexture;
    glGenTextures(1, &entityTexture);
    glBindTexture(GL_TEXTURE_2D, entityTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                           entityTexture, 0);

    // rbo
    uint32_t rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    uint32_t attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                               GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    uint32_t pingpongFBO[2];
    uint32_t pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);

    for (uint32_t i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, pingpongBuffer[i], 0);
    }

    uint32_t S_PPFBO;
    glGenFramebuffers(1, &S_PPFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, S_PPFBO);

    uint32_t S_PPT;
    glGenTextures(1, &S_PPT);
    glBindTexture(GL_TEXTURE_2D, S_PPT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           S_PPT, 0);

    uint32_t SRBO;
    glGenRenderbuffers(1, &SRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, SRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderer->samples,
                                     GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, SRBO);

    float timeStep = 1.0f / 60.0f;

    HYPER_LOG("Renderer initialized")
    float timeSinceAppStart = 0.0f;

    uint32_t depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const uint32_t SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    uint32_t depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
                 SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<HyperAPI::Experimental::GameObject *> mouseOverObjects;

    glEnable(GL_DEPTH_TEST);

    HyperAPI::n_Bloom::BloomRenderer bloomRenderer;
    bloomRenderer.Init(width, height);

    // create a framebuffer test
    uint32_t testFBO;
    glGenFramebuffers(1, &testFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, testFBO);

    uint32_t testTexture;
    glGenTextures(1, &testTexture);
    glBindTexture(GL_TEXTURE_2D, testTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           testTexture, 0);

    uint32_t testRBO;
    glGenRenderbuffers(1, &testRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, testRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, testRBO);

    float prevWidth, prevHeight;
    while (!glfwWindowShouldClose(renderer->window)) {
        bloomRenderer.m_SrcViewportSize = glm::vec2(width, height);
        bloomRenderer.m_SrcViewportSizeFloat = glm::vec2(width, height);
        framebufferShader.Bind();
        framebufferShader.SetUniform1f("exposure", exposure);

        framebufferShader.Bind();
        framebufferShader.SetUniform1f(
            "chromaticAmount",
            HyperAPI::config.postProcessing.chromaticAberration.intensity);
        framebufferShader.SetUniform1f(
            "vignetteAmount",
            HyperAPI::config.postProcessing.vignette.intensity);

        HyperAPI::Timestep::currentFrame = glfwGetTime();
        HyperAPI::Timestep::deltaTime =
            HyperAPI::Timestep::currentFrame - HyperAPI::Timestep::lastFrame;
        HyperAPI::Timestep::lastFrame = HyperAPI::Timestep::currentFrame;

        timeSinceAppStart += HyperAPI::Timestep::deltaTime;

        glfwPollEvents();
        glfwGetWindowSize(renderer->window, &winWidth, &winHeight);
        glClear(GL_DEPTH_BUFFER_BIT);

        if (width != prevWidth || prevHeight != height) {
            glDeleteFramebuffers(1, &postProcessingFBO);
            glDeleteTextures(1, &postProcessingTexture);

            glDeleteFramebuffers(1, &S_PPFBO);
            glDeleteTextures(1, &S_PPT);
            glDeleteTextures(1, &bloomTexture);
            glDeleteFramebuffers(2, pingpongFBO);
            glDeleteTextures(2, pingpongBuffer);
            glDeleteRenderbuffers(1, &SRBO);
            glDeleteRenderbuffers(1, &rbo);
            glDeleteTextures(1, &entityTexture);
            glDeleteTextures(1, &testTexture);
            glDeleteFramebuffers(1, &testFBO);
            glDeleteRenderbuffers(1, &testRBO);

            glGenFramebuffers(1, &testFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, testFBO);

            glGenTextures(1, &testTexture);
            glBindTexture(GL_TEXTURE_2D, testTexture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, testTexture, 0);

            glGenRenderbuffers(1, &testRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, testRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,
                                  height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER, testRBO);

            glGenFramebuffers(1, &postProcessingFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

            glGenTextures(1, &postProcessingTexture);
            glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, postProcessingTexture, 0);

            glGenTextures(1, &bloomTexture);
            glBindTexture(GL_TEXTURE_2D, bloomTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                         GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                   GL_TEXTURE_2D, bloomTexture, 0);

            glGenTextures(1, &entityTexture);
            glBindTexture(GL_TEXTURE_2D, entityTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0,
                         GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                                   GL_TEXTURE_2D, entityTexture, 0);

            glDrawBuffers(3, attachments);

            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderer->samples,
                                             GL_DEPTH24_STENCIL8, width,
                                             height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER, rbo);

            glBindFramebuffer(GL_FRAMEBUFFER, S_PPFBO);

            // glGenFramebuffers(2, pingpongFBO);
            // glGenTextures(2, pingpongBuffer);

            // for(uint32_t i = 0; i < 2; i++)
            // {
            //     glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            //     glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
            //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0,
            //     GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            //     GL_NEAREST); glTexParameteri(GL_TEXTURE_2D,
            //     GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            //     GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_2D,
            //     GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            //     GL_TEXTURE_2D, pingpongBuffer[i], 0);
            // }
            // glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glGenFramebuffers(1, &S_PPFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, S_PPFBO);

            glGenTextures(1, &S_PPT);
            glBindTexture(GL_TEXTURE_2D, S_PPT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, S_PPT, 0);
            glBindBuffer(GL_ARRAY_BUFFER, rectVBO);

            glGenRenderbuffers(1, &SRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, SRBO);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderer->samples,
                                             GL_DEPTH24_STENCIL8, width,
                                             height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_STENCIL_ATTACHMENT,
                                      GL_RENDERBUFFER, SRBO);
        }

        prevWidth = width;
        prevHeight = height;

        float rectangleVert[] = {
            1, -1, 1, 0, -1, -1, 0, 0, -1, 1, 0, 1,

            1, 1,  1, 1, 1,  -1, 1, 0, -1, 1, 0, 1,
        };

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rectangleVert),
                        rectangleVert);
        NewFrame(postProcessingFBO, width, height);

        if (renderOnScreen) {
            glfwGetWindowSize(renderer->window, &width, &height);
            glViewport(0, 0, width, height);
        }

        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        update(S_PPT);

        glReadBuffer(GL_COLOR_ATTACHMENT2);
        uint32_t entityId;
        glReadPixels(sceneMouseX, sceneMouseY, 1, 1, GL_RED_INTEGER,
                     GL_UNSIGNED_INT, &entityId);

        for (auto &gameObject : HyperAPI::Scene::m_GameObjects) {
            if (sceneMouseY < 0 || sceneMouseX < 0) {
                break;
            }

            using namespace HyperAPI::Experimental;

            if (HyperAPI::isRunning) {
                for (auto &overObject : mouseOverObjects) {
                    if (overObject->entity != (entt::entity)entityId) {
                        if (overObject->HasComponent<NativeScriptManager>()) {
                            auto &nativeManager =
                                overObject->GetComponent<NativeScriptManager>();
                            for (auto &script : nativeManager.m_StaticScripts) {
                                script->OnMouseExit();
                                mouseOverObjects.push_back(overObject);
                            }
                        }

                        if (overObject->HasComponent<m_LuaScriptComponent>()) {
                            auto &nativeManager =
                                overObject
                                    ->GetComponent<m_LuaScriptComponent>();
                            for (auto &script : nativeManager.scripts) {
                                script.OnMouseExit();
                                mouseOverObjects.push_back(overObject);
                            }
                        }

                        mouseOverObjects.erase(
                            std::remove(mouseOverObjects.begin(),
                                        mouseOverObjects.end(), overObject),
                            mouseOverObjects.end());
                        break;
                    }
                }
            }

            if (gameObject->entity == (entt::entity)entityId) {
                if (HyperAPI::isRunning) {
                    if (gameObject->HasComponent<NativeScriptManager>()) {
                        auto &nativeManager =
                            gameObject->GetComponent<NativeScriptManager>();
                        for (auto &script : nativeManager.m_StaticScripts) {
                            script->OnMouseEnter();
                        }
                    }
                    // if exists
                    if (std::find(mouseOverObjects.begin(),
                                  mouseOverObjects.end(),
                                  gameObject) == mouseOverObjects.end()) {
                        mouseOverObjects.push_back(gameObject);
                    }

                    if (gameObject->HasComponent<m_LuaScriptComponent>()) {
                        auto &nativeManager =
                            gameObject->GetComponent<m_LuaScriptComponent>();
                        for (auto &script : nativeManager.scripts) {
                            script.OnMouseEnter();
                        }
                    }
                }

                double mouseX, mouseY;
                glfwGetCursorPos(renderer->window, &mouseX, &mouseY);

                if (ImGui::IsMouseDoubleClicked(0) && !mouseClicked &&
                    !isGuzimoInUse) {
                    HyperAPI::Scene::m_Object = gameObject;
                    strncpy(HyperAPI::Scene::name,
                            HyperAPI::Scene::m_Object->name.c_str(), 499);
                    HyperAPI::Scene::name[499] = '\0';

                    strncpy(HyperAPI::Scene::tag,
                            HyperAPI::Scene::m_Object->tag.c_str(), 499);
                    HyperAPI::Scene::tag[499] = '\0';

                    strncpy(HyperAPI::Scene::layer,
                            HyperAPI::Scene::m_Object->layer.c_str(), 32);
                    HyperAPI::Scene::layer[31] = '\0';
                    glfwSetCursorPos(renderer->window, mouseX, mouseY);

                    mouseClicked = true;
                } else if (!ImGui::IsMouseDoubleClicked(0)) {
                    mouseClicked = false;
                }

                break;
            }
        }

        glClear(GL_DEPTH_BUFFER_BIT);

        bool horizontal = true, first_iteration = true;
        int amount = 10;
        blurShader.Bind();

        glDisable(GL_BLEND);
        bloomRenderer.RenderBloomTexture(bloomTexture, 0.005f, rectVAO);

        glActiveTexture(GL_TEXTURE16);
        glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());

        glClear(GL_DEPTH_BUFFER_BIT);
        framebufferShader.Bind();
        glActiveTexture(GL_TEXTURE16);
        glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());
        framebufferShader.SetUniform1i("bloomTexture", 16);
        if (renderOnScreen) {
            EndFrame(framebufferShader, *renderer, rectVAO,
                     postProcessingTexture, postProcessingFBO, width, height);
        } else {
            EndEndFrame(framebufferShader, *renderer, rectVAO,
                        postProcessingTexture, postProcessingFBO, S_PPT,
                        S_PPFBO, width, height, sceneMouseX, sceneMouseY);
        }

        if (!renderOnScreen) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();

            gui(S_PPT, S_PPFBO, testTexture);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        glfwSwapBuffers(renderer->window);
    }

    HYPER_LOG("Closing Vault Engine")
}

MousePicker::MousePicker(Application *app, HyperAPI::Camera *camera,
                         glm::mat4 projection) {
    this->camera = camera;
    this->projectionMatrix = projection;
    this->appRef = app;
}

Vector3 MousePicker::getCurrentRay() { return currentRay; }

void MousePicker::update() { currentRay = calculateMouseRay(); }

Vector3 MousePicker::calculateMouseRay() {
    Vector2 normalizedCoords = getNormalizedDeviceCoords(mouseX, mouseY);
    Vector4 clipCoords =
        Vector4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
    Vector4 eyeCoords = toEyeCoords(clipCoords);
    Vector3 worldRay = toWorldCoords(eyeCoords);

    return worldRay;
}

Vector2 MousePicker::getNormalizedDeviceCoords(float mouseX, float mouseY) {
    float x = (2.0f * mouseX) / winX - 1;
    float y = (2.0f * mouseY) / winY - 1.0f;
    return Vector2(x, y);
}

Vector4 MousePicker::toEyeCoords(Vector4 clipCoords) {
    glm::mat4 invertedProjection = glm::inverse(camera->projection);
    Vector4 eyeCoords = invertedProjection * clipCoords;
    return Vector4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

Vector3 MousePicker::toWorldCoords(Vector4 eyeCoords) {
    glm::mat4 invertedView = glm::inverse(camera->view);
    Vector4 rayWorld = invertedView * eyeCoords;
    Vector3 mouseRay = Vector3(rayWorld.x, rayWorld.y, rayWorld.z);
    mouseRay = glm::normalize(mouseRay);
    return mouseRay;
}

float LerpFloat(float a, float b, float t) { return a + t * (b - a); }
} // namespace Hyper