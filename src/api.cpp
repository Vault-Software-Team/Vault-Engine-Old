#include "lib/api.hpp"
#include "lib/scene.hpp"

std::vector<HyperAPI::PointLight*> PointLights;
std::vector<HyperAPI::SpotLight*> SpotLights;
std::vector<HyperAPI::Light2D*> Lights2D;
std::vector<HyperAPI::DirectionalLight*> DirLights;
std::vector<HyperAPI::Mesh*> hyperEntities;

const int width = 1280;
const int height = 720;

float rectangleVert[] = {
    1, -1,  1, 0,
    -1, -1,  0, 0,
    -1, 1,  0, 1,

    1, 1,  1, 1,
    1, -1,  1, 0,
    -1, 1,  0, 1,
};

glm::mat4 projection;
glm::mat4 view;

float rotation = 0.0f;
double previousTime = glfwGetTime();

namespace uuid {
    std::random_device              rd;
    std::mt19937                    gen(rd());
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
        };
        return ss.str();
    }
}

std::string get_file_contents(const char *file) {
    std::ifstream g_file(file);
    if (!g_file.is_open()) {
        std::cout << "Failed to open shader file: " << file << std::endl;
    }

    std::string content, line;
    while(getline(g_file, line)) {
        content += line + "\n";        
    }

    return content;
}

void NewFrame(unsigned int FBO, int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(pow(0.3f, 2.2f), pow(0.3f, 2.2f), pow(0.3f, 2.2f), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST); 
}

void EndFrame(HyperAPI::Shader &framebufferShader, HyperAPI::Renderer &renderer, unsigned int FBO, unsigned int rectVAO, unsigned int postProcessingTexture, unsigned int postProcessingFBO, const int width, const int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    framebufferShader.Bind();
    framebufferShader.SetUniform1i("screenTexture", 15);
    glBindVertexArray(rectVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDepthFunc(GL_LEQUAL);
    if(renderer.wireframe) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
}

void EndEndFrame(
    HyperAPI::Shader &framebufferShader, 
    HyperAPI::Renderer &renderer, 
    unsigned int FBO, 
    unsigned int rectVAO, 
    unsigned int postProcessingTexture, 
    unsigned int postProcessingFBO, 
    unsigned int S_FBO,  
    unsigned int S_postProcessingTexture, 
    unsigned int S_postProcessingFBO, 
    const int width, 
    const int height
) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    NewFrame(S_FBO, width, height);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    framebufferShader.Bind();
    framebufferShader.SetUniform1i("screenTexture", 15);
    glBindVertexArray(rectVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    EndFrame(framebufferShader, renderer, S_FBO, rectVAO, S_postProcessingTexture, S_postProcessingFBO, width, height);

    glDepthFunc(GL_LEQUAL);
    if(renderer.wireframe) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

    glClearColor(pow(0.3f, 2.2f), pow(0.3f, 2.2f), pow(0.3f, 2.2f), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SC_EndFrame(HyperAPI::Renderer &renderer, unsigned int FBO, unsigned int rectVAO, unsigned int postProcessingTexture, unsigned int postProcessingFBO, const int width, const int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool EndsWith(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

namespace HyperAPI {
    std::string cwd = "";
    std::string dirPayloadData = "";
    bool isRunning = false;
    bool isStopped = true;

    namespace AudioEngine {
        void PlaySound(const std::string &path, float volume, bool loop, int channel) {
            Mix_Chunk *chunk = Mix_LoadWAV(path.c_str());

            Mix_VolumeChunk(chunk, volume * 128);
            Mix_PlayChannel(channel, chunk, loop ? -1 : 0);
        }

        void StopSound(int channel) {
            Mix_HaltChannel(channel);
        }
        
        void PlayMusic(const std::string &path, float volume, bool loop) {
            // generate chunk
            Mix_Music *music = Mix_LoadMUS(path.c_str());
            if(music == NULL) {
                HYPER_LOG("Failed to load music: " + path);
                return;
            }

            // set volume, the volume scale is 0 - 1
            Mix_VolumeMusic(volume * 128);

            Mix_PlayMusic(music, loop ? -1 : 0);
        }

        void StopMusic() {
            Mix_HaltMusic();
        }
    }

    std::vector<HyperAPI::Animation> GetAnimationsFromXML(const char *texPath, float delay, Vector2 sheetSize, const std::string &xmlFile)
    {
        std::vector<HyperAPI::Animation> subtextures;
        return subtextures;
    }

    Renderer::Renderer(int width, int height, const char *title, Vector2 g_gravity, unsigned int samples, bool wireframe) {
        this->samples = samples;
        this->wireframe = wireframe;

        //mix
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

        if (!glfwInit()) {
            HYPER_LOG("Failed to initialize GLFW");
        }
        // //set verisons
        // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // glfwWindowHint(GLFW_SAMPLES, samples);

        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }

        glfwMakeContextCurrent(window);    
        gladLoadGL();

        //set minimum size of a window;
        glfwSetWindowSizeLimits(window, 800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        // blur cubemap
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        //add icon to window
        GLFWimage images[1];
        images[0].pixels = stbi_load("build/logo2.png", &images[0].width, &images[0].height, 0, 4);
        glfwSetWindowIcon(window, 1, images);

        // glEnable(GL_FRAMEBUFFER_SRGB);

        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
        // glFrontFace(GL_CW);

        if(wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glDepthFunc(GL_LESS);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        //belnding
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    }

    void Renderer::Render(Camera &camera) {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();


        glfwSwapBuffers(window);
    }

    Shader::Shader(const char *shaderPath) {
        if(strcmp(shaderPath, "NULL_SHADER") == 0) {
            return;
        }

        HYPER_LOG(std::string("Loading shader: ") + shaderPath);

        std::ifstream shaderFile(shaderPath);
        if (!shaderFile.is_open()) {
            std::cout << "Failed to open shader file" << std::endl;
        }

        std::string vertCode, fragCode, geometryCode, line;
        int type = -1;
        while(getline(shaderFile, line)) {
            if(line == "#shader vertex") {
                type = 0;
            } else if(line == "#shader fragment") {
                type = 1;
            } else if(line == "#shader geometry") {
                type = 2;
            } else {
                if(type == 0) {
                    vertCode += line + "\n";
                } else if(type == 1) {
                    fragCode += line + "\n";
                } else if(type == 2) {
                    geometryCode += line + "\n";
                }
            }
        }

        const char *vertShaderCode = vertCode.c_str();
        const char *fragShaderCode = fragCode.c_str();
        const char *geometryShaderCode = geometryCode.c_str();
        unsigned int vertShader, fragShader, geometryShader;
        int success;
        char infoLog[512];

        vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vertShaderCode, NULL);
        glCompileShader(vertShader);
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
            HYPER_LOG("Failed to compile Vertex Shader");
            std::cout << infoLog << std::endl;
        }

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragShaderCode, NULL);
        glCompileShader(fragShader);
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
            HYPER_LOG("Failed to compile Fragment Shader");
            std::cout << infoLog << std::endl;
        }

        // geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        // glShaderSource(geometryShader, 1, &geometryShaderCode, NULL);
        // glCompileShader(geometryShader);
        // glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        // if(!success) {
        //     glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        //     std::cout << "Failed to compile geometry shader" << std::endl;
        //     std::cout << infoLog << std::endl;
        // }

        ID = glCreateProgram();
        glAttachShader(ID, vertShader);
        glAttachShader(ID, fragShader);
        if(geometryCode != "") {
            // glAttachShader(ID, geometryShader);
        }
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            HYPER_LOG("Failed to link program");
            std::cout << infoLog << std::endl;
        }

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        // glDeleteShader(geometryShader);
    }

    void Shader::Bind() {
        glUseProgram(ID);
    }

    void Shader::Unbind() {
        glUseProgram(0);
    }

    void Shader::SetUniform1f(const char *name, float value) {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void Shader::SetUniform1i(const char *name, int value) {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void Shader::SetUniform2f(const char *name, float value1, float value2) {
        glUniform2f(glGetUniformLocation(ID, name), value1, value2);
    }

    void Shader::SetUniform3f(const char *name, float value1, float value2, float value3) {
        glUniform3f(glGetUniformLocation(ID, name), value1, value2, value3);
    }

    void Shader::SetUniform4f(const char *name, float value1, float value2, float value3, float value4) {
        glUniform4f(glGetUniformLocation(ID, name), value1, value2, value3, value4);
    }

    void Shader::SetUniformMat4(const char *name, glm::mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Material &material, bool empty, bool batched) {
        this->vertices = vertices;
        this->indices = indices;
        this->material = material;
        this->ID = uuid::generate_uuid_v4();
        this->empty = empty;
    
        TransformComponent component;
        component.position = Vector3(0,0,0);
        component.scale = Vector3(1,1,1);
        this->Components.push_back(component);

        if(empty) {
            return;
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &IBO);
        glBindVertexArray(VAO);

        if(!batched) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            //coords
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(0);

            //color
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
            glEnableVertexAttribArray(1);
            
            // normals
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(2);

            //texuv
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texUV));
            glEnableVertexAttribArray(3);

            glVertexAttribPointer(4, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
            glEnableVertexAttribArray(4);

            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
            glEnableVertexAttribArray(5);

            // glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
            // glEnableVertexAttribArray(4);

            // // add mat4 
            // glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(12 * sizeof(float)));
            // glEnableVertexAttribArray(5);
            // glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(16 * sizeof(float)));
            // glEnableVertexAttribArray(6);
            // glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(20 * sizeof(float)));
            // glEnableVertexAttribArray(7);
            // glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(24 * sizeof(float)));
            // glEnableVertexAttribArray(8);
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
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            //coords
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(0);

            //color
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            
            // normals
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            //texuv
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(float)));
            glEnableVertexAttribArray(3);

            glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
            glEnableVertexAttribArray(4);

            // add mat4 
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(12 * sizeof(float)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(16 * sizeof(float)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(20 * sizeof(float)));
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(24 * sizeof(float)));
            glEnableVertexAttribArray(8);
            //divisors
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

    void Mesh::Draw(
        Shader &shader,
        Camera &camera,
        glm::mat4 matrix,
        glm::vec3 translation,
        glm::quat rotation,
        glm::vec3 scale
    ) {
        material.Bind(shader);
        camera.Matrix(shader, "camera");

        previousTime = (float)glfwGetTime();
        shader.SetUniform1i("shadowMap", 2);
        if(camera.EnttComp) {
            auto &cameraTransform = Scene::m_Registry.get<Experimental::Transform>(camera.entity);
            shader.SetUniform3f("cameraPosition", cameraTransform.position.x, cameraTransform.position.y, cameraTransform.position.z);
        }
        else {
            TransformComponent cameraTransform = camera.GetComponent<TransformComponent>();
            shader.SetUniform3f("cameraPosition", cameraTransform.position.x, cameraTransform.position.y, cameraTransform.position.z);
        }
        shader.SetUniform1i("cubeMap", 10);

        // scriptComponent.OnUpdate();

        TransformComponent component = GetComponent<TransformComponent>();

        model = glm::translate(glm::mat4(1.0f), component.position) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::scale(glm::mat4(1.0f), Vector3(component.scale.x * 0.5, component.scale.y * 0.5, component.scale.z * 0.5));

        model = matrix * model;
       
        glm::mat4 trans = glm::mat4(1);
        glm::mat4 rot = glm::mat4(1);
        glm::mat4 sca = glm::mat4(1);

        component.transform = glm::translate(glm::mat4(1.0f), component.position) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::scale(glm::mat4(1.0f), component.scale) * matrix;

        model = component.transform;
        trans = glm::translate(trans, translation);
        rot = glm::mat4_cast(rotation);
        sca = glm::scale(sca, scale);

        shader.SetUniformMat4("model", model);
        shader.SetUniformMat4("translation", trans);
        shader.SetUniformMat4("rotation", rot);
        shader.SetUniformMat4("scale", sca);

        if(!camera.mode2D) {
            for(int i = 0; i < Scene::PointLights.size(); i++) {
                // Scene::PointLights[i]->scriptComponent.OnUpdate();
                shader.SetUniform3f(("pointLights[" + std::to_string(i) + "].lightPos").c_str(), Scene::PointLights[i]->lightPos.x, Scene::PointLights[i]->lightPos.y, Scene::PointLights[i]->lightPos.z);
                shader.SetUniform3f(("pointLights[" + std::to_string(i) + "].color").c_str(), Scene::PointLights[i]->color.x, Scene::PointLights[i]->color.y, Scene::PointLights[i]->color.z);
                shader.SetUniform1f(("pointLights[" + std::to_string(i) + "].intensity").c_str(), Scene::PointLights[i]->intensity);
            }
            if(Scene::PointLights.size() == 0) {
                for(int i = 0; i < 100; i++) {
                    shader.SetUniform3f(("pointLights[" + std::to_string(i) + "].lightPos").c_str(), 0,0,0);
                    shader.SetUniform3f(("pointLights[" + std::to_string(i) + "].color").c_str(), 0,0,0);
                    shader.SetUniform1f(("pointLights[" + std::to_string(i) + "].intensity").c_str(), 0);
                }
            }

            for(int i = 0; i < Scene::SpotLights.size(); i++) {
                // Scene::SpotLights[i]->scriptComponent.OnUpdate();
                shader.SetUniform3f(("spotLights[" + std::to_string(i) + "].lightPos").c_str(), Scene::SpotLights[i]->lightPos.x, Scene::SpotLights[i]->lightPos.y, Scene::SpotLights[i]->lightPos.z);
                shader.SetUniform3f(("spotLights[" + std::to_string(i) + "].color").c_str(), Scene::SpotLights[i]->color.x, Scene::SpotLights[i]->color.y, Scene::SpotLights[i]->color.z);
                // shader.SetUniform1f(("spotLights[" + std::to_string(i) + "].outerCone").c_str(), Scene::SpotLights[i]->outerCone);
                // shader.SetUniform1f(("spotLights[" + std::to_string(i) + "].innerCone").c_str(), Scene::SpotLights[i]->innerCone);
                shader.SetUniform3f(("spotLights[" + std::to_string(i) + "].angle").c_str(), Scene::SpotLights[i]->angle.x, Scene::SpotLights[i]->angle.y, Scene::SpotLights[i]->angle.z);
            }
            if(Scene::SpotLights.size() == 0) {
                for(int i = 0; i < 100; i++) {
                    shader.SetUniform3f(("spotLights[" + std::to_string(i) + "].lightPos").c_str(), 0,0,0);
                    shader.SetUniform3f(("spotLights[" + std::to_string(i) + "].color").c_str(), 0,0,0);
                    shader.SetUniform1f(("spotLights[" + std::to_string(i) + "].outerCone").c_str(), 0);
                    shader.SetUniform1f(("spotLights[" + std::to_string(i) + "].innerCone").c_str(), 0);
                }
            }

            for(int i = 0; i < Scene::DirLights.size(); i++) {
                // Scene::DirLights[i]->scriptComponent.OnUpdate();
                shader.SetUniform3f(("dirLights[" + std::to_string(i) + "].lightPos").c_str(), Scene::DirLights[i]->lightPos.x, Scene::DirLights[i]->lightPos.y, Scene::DirLights[i]->lightPos.z);
                shader.SetUniform3f(("dirLights[" + std::to_string(i) + "].color").c_str(), Scene::DirLights[i]->color.x, Scene::DirLights[i]->color.y, Scene::DirLights[i]->color.z);
                shader.SetUniform1f(("dirLights[" + std::to_string(i) + "].intensity").c_str(), Scene::DirLights[i]->intensity);
            }
            if(Scene::DirLights.size() == 0) {
                for(int i = 0; i < 100; i++) {
                    shader.SetUniform3f(("dirLights[" + std::to_string(i) + "].lightPos").c_str(), 0,0,0);
                    shader.SetUniform3f(("dirLights[" + std::to_string(i) + "].color").c_str(), 0,0,0);
                    shader.SetUniform1f(("dirLights[" + std::to_string(i) + "].intensity").c_str(), 0);
                }
            }
        } else {
            for(int i = 0; i < Scene::Lights2D.size(); i++) {
                shader.SetUniform2f(("pointLights[" + std::to_string(i) + "].lightPos").c_str(), Scene::Lights2D[i]->lightPos.x, Scene::Lights2D[i]->lightPos.y);
                shader.SetUniform3f(("pointLights[" + std::to_string(i) + "].color").c_str(), Scene::Lights2D[i]->color.x, Scene::Lights2D[i]->color.y, Scene::Lights2D[i]->color.z);
                shader.SetUniform1f(("pointLights[" + std::to_string(i) + "].range").c_str(), Scene::Lights2D[i]->range);
            }
            if(Scene::Lights2D.size() == 0) {
                for(int i = 0; i < 100; i++) {
                    shader.SetUniform2f(("pointLights[" + std::to_string(i) + "].lightPos").c_str(), 0,0);
                    shader.SetUniform3f(("pointLights[" + std::to_string(i) + "].color").c_str(), 0,0,0);
                    shader.SetUniform1f(("pointLights[" + std::to_string(i) + "].range").c_str(), 0);
                }
            }
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        material.Unbind(shader);
    }

    Texture::Texture(const char *texturePath, unsigned int slot, const char *textureType) {
        stbi_set_flip_vertically_on_load(true);
        texType = textureType;
        texStarterPath = texturePath;
        this->slot = slot;
        texPath = std::string(texturePath);
        data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

        HYPER_LOG("Texture " + std::to_string(slot) + " loaded from " + texturePath);

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if(std::string(textureType) == "texture_normal") {
            glTexImage2D(
                GL_TEXTURE_2D, 
                0, 
                GL_RGB, 
                width,
                height, 
                0, 
                GL_RGBA, 
                GL_UNSIGNED_BYTE, 
                data
            );
        }
        else if(nrChannels >= 4) 
            glTexImage2D(
                GL_TEXTURE_2D, 
                0, 
                GL_SRGB_ALPHA, 
                width,
                height, 
                0, 
                GL_RGBA, 
                GL_UNSIGNED_BYTE, 
                data
            );
        else if(nrChannels == 3)
            glTexImage2D(
                GL_TEXTURE_2D, 
                0, 
                GL_SRGB, 
                width,
                height, 
                0, 
                GL_RGB, 
                GL_UNSIGNED_BYTE, 
                data
            );
        else if(nrChannels == 1) 
            glTexImage2D(
                GL_TEXTURE_2D, 
                0, 
                GL_SRGB, 
                width,
                height, 
                0, 
                GL_RED, 
                GL_UNSIGNED_BYTE, 
                data
            );
        else
            return;
        
            // throw std::invalid_argument("Texture format not supported");

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Bind(unsigned int slot) {
        if(slot == -1) {
            glActiveTexture(GL_TEXTURE0 + this->slot);
            glBindTexture(GL_TEXTURE_2D, ID);
        } else {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, ID);
        }
    }

    void Texture::Unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Camera::Camera(bool mode2D, int width, int height, glm::vec3 position, entt::entity entity)
    {
        Camera::width = width;
        Camera::height = height;
        Camera::mode2D = mode2D;

        this->entity = entity;

        if(entity != entt::null) {
            EnttComp = true;
        } else {
            EnttComp = false;
        }

        if(EnttComp) {
            // auto &transform = Scene::m_Registry.get<Experimental::Transform>(entity);
            // transform.position = position;
            // transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);
        } else {
            TransformComponent transform;
            transform.position = position;
            transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);
            AddComponent(transform);
        }
    }

    void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane, Vector2 winSize)
    {
        if(EnttComp) {
            auto &transform = Scene::m_Registry.get<Experimental::Transform>(entity);
        
            view = glm::mat4(1.0f);
            projection = glm::mat4(1.0f);
            
            width = winSize.x;
            height = winSize.y;

            view = glm::lookAt(transform.position, transform.position + transform.rotation, Up);
            float aspect = (float)width/height;
            
            if(mode2D) {
                projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, 0.1f, 5000.0f);
            } else {
                projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
            }

            camMatrix = projection * view;
        } else {
            auto transform = GetComponent<TransformComponent>();
        
            view = glm::mat4(1.0f);
            projection = glm::mat4(1.0f);
            
            width = winSize.x;
            height = winSize.y;

            view = glm::lookAt(transform.position, transform.position + transform.rotation, Up);
            float aspect = (float)width/height;
            
            if(mode2D) {
                projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, 0.1f, 5000.0f);
            } else {
                projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
            }

            camMatrix = projection * view;
        }

    }

    void Camera::Matrix(Shader& shader, const char* uniform) {
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(camMatrix));
    }

    void Camera::Inputs(GLFWwindow* window, Vector2 winPos)
    {
        if(EnttComp) {
            auto &transform = Scene::m_Registry.get<Experimental::Transform>(entity);
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                if(!mode2D) {
                    transform.position += speed * transform.rotation;
                } else {
                    transform.position.y += speed;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                transform.position += speed * -glm::normalize(glm::cross(transform.rotation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                if(!mode2D) {
                    transform.position += speed * -transform.rotation;
                } else {
                    transform.position.y -= speed;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                transform.position += speed * glm::normalize(glm::cross(transform.rotation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                transform.position += speed * Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            {
                transform.position += speed * -Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                speed = 0.4f;
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            {
                speed = 0.1f;
            }

            if(!mode2D) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

                    if (firstClick)
                    {
                        glfwSetCursorPos(window, (width / 2), (height / 2));
                        firstClick = false;
                    }

                    double mouseX;
                    double mouseY;
                    glfwGetCursorPos(window, &mouseX, &mouseY);

                    rotX = sensitivity * (float)(mouseY - (winPos.y + (height / 2))) / height;
                    rotY = sensitivity * (float)(mouseX - (winPos.x + (width / 2))) / width;

                    glm::vec3 newOrientation = glm::rotate(transform.rotation, glm::radians(-rotX), glm::normalize(glm::cross(transform.rotation, Up)));

                    // if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
                    // {
                    transform.rotation = newOrientation;
                    // }

                    transform.rotation = glm::rotate(transform.rotation, glm::radians(-rotY), Up);

                    glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y + (height / 2));
                }
                else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    // glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y + (height / 2));
                    firstClick = true;
                }

            }
        } else {
            auto transform = GetComponent<TransformComponent>();
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                if(!mode2D) {
                    transform.position += speed * transform.rotation;
                } else {
                    transform.position.y += speed;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                transform.position += speed * -glm::normalize(glm::cross(transform.rotation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                if(!mode2D) {
                    transform.position += speed * -transform.rotation;
                } else {
                    transform.position.y -= speed;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                transform.position += speed * glm::normalize(glm::cross(transform.rotation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                transform.position += speed * Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            {
                transform.position += speed * -Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                speed = 0.4f;
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            {
                speed = 0.1f;
            }

            if(!mode2D) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

                    if (firstClick)
                    {
                        glfwSetCursorPos(window, (width / 2), (height / 2));
                        firstClick = false;
                    }

                    double mouseX;
                    double mouseY;
                    glfwGetCursorPos(window, &mouseX, &mouseY);

                    rotX = sensitivity * (float)(mouseY - (winPos.y + (height / 2))) / height;
                    rotY = sensitivity * (float)(mouseX - (winPos.x + (width / 2))) / width;

                    glm::vec3 newOrientation = glm::rotate(transform.rotation, glm::radians(-rotX), glm::normalize(glm::cross(transform.rotation, Up)));

                    // if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
                    // {
                    transform.rotation = newOrientation;
                    // }

                    transform.rotation = glm::rotate(transform.rotation, glm::radians(-rotY), Up);

                    glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y + (height / 2));
                }
                else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    // glfwSetCursorPos(window, winPos.x + (width / 2), winPos.y + (height / 2));
                    firstClick = true;
                }
                UpdateComponent(transform);
            }
            UpdateComponent(transform);
        }
    }
   
    void Model::Draw(Shader &shader, Camera &camera)
    {
        // scriptComponent.componentSystem = this;
        // scriptComponent.OnUpdate();
        for(unsigned int i = 0; i < meshes.size(); i++) {
            TransformComponent modelTransform = GetComponent<TransformComponent>();
            modelTransform.transform = glm::translate(glm::mat4(1.0f), modelTransform.position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(modelTransform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(modelTransform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(modelTransform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), Vector3(modelTransform.scale.x * 0.5, modelTransform.scale.y * 0.5, modelTransform.scale.z * 0.5));
            
            UpdateComponent(modelTransform);         
            meshes[i]->Draw(shader, camera, transform * modelTransform.transform);
        }
    }

    void Model::loadModel(std::string path) {
        ID = uuid::generate_uuid_v4();
         
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate);	
        
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }  

    void Model::processNode(aiNode *node, const aiScene *scene)
    {
        // process all the node's meshes (if any)
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            // get name
            std::string name = mesh->mName.C_Str();

            //get matricies
            aiMatrix4x4 aiTransform = node->mTransformation;
            transform = glm::mat4(1.0f);
            transform[0][0] = aiTransform.a1; transform[1][0] = aiTransform.b1; transform[2][0] = aiTransform.c1; transform[3][0] = aiTransform.d1;
            transform[0][1] = aiTransform.a2; transform[1][1] = aiTransform.b2; transform[2][1] = aiTransform.c2; transform[3][1] = aiTransform.d2;
            transform[0][2] = aiTransform.a3; transform[1][2] = aiTransform.b3; transform[2][2] = aiTransform.c3; transform[3][2] = aiTransform.d3;
            transform[0][3] = aiTransform.a4; transform[1][3] = aiTransform.b4; transform[2][3] = aiTransform.c4; transform[3][3] = aiTransform.d4;
        
            meshes.push_back(processMesh(mesh, scene, name));			
        }
        // then do the same for each of its children
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }   

    Mesh *Model::processMesh(aiMesh *mesh, const aiScene *scene, const std::string &name)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            if(mesh->mTextureCoords[0])
            {
                vertex.texUV = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
            {
                vertex.texUV = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        //indices
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }  

        Texture *diffuse = nullptr;
        Texture *specular = nullptr;
        Texture *normal = nullptr;

        if(mesh->mMaterialIndex >= 0 && texturesEnabled == true)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            diffuse = &diffuseMaps[0];
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            specular = &specularMaps[0];
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        if(texturesEnabled) {
            Material material(Color, textures);
            material.diffuse = diffuse;
            material.specular = specular;

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

    std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            std::string texPath = directory + '/' + str.C_Str();
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].texStarterPath, texPath.c_str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }

            if(!skip)
            {   // if texture hasn't been loaded already, load it
                std::string texPath = directory + '/' + str.C_Str();
                Texture texture(texPath.c_str(), i, typeName.c_str());
                textures.push_back(texture);
                textures_loaded.push_back(texture); // add to loaded textures
                currSlot++;
            }
        }
        return textures;
    }  

    Skybox::Skybox(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &front, const std::string &back) {
        shader = new Shader("shaders/skybox.glsl");
        facesCubemap.push_back(right);
        facesCubemap.push_back(left);
        facesCubemap.push_back(top);
        facesCubemap.push_back(bottom);
        facesCubemap.push_back(front);
        facesCubemap.push_back(back);

        HYPER_LOG("Skybox created");

        float skyboxVertices[] =
        {
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f
        };


        unsigned int skyboxIndices[] =
        {
            // Right
            1, 2, 6,
            6, 5, 1,
            // Left
            0, 4, 7,
            7, 3, 0,
            // Top
            4, 5, 6,
            6, 7, 4,
            // Bottom
            0, 3, 2,
            2, 1, 0,
            // Back
            0, 1, 5,
            5, 4, 0,
            // Front
            3, 7, 6,
            6, 2, 3
        };

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glGenBuffers(1, &skyboxEBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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

        for(unsigned int i = 0; i < 6; i++) {
            int width, height, nrChannels;

            unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
            if(data) {
                stbi_set_flip_vertically_on_load(false);
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                    0, 
                    GL_SRGB,
                    width, 
                    height, 
                    0, 
                    GL_RGB, 
                    GL_UNSIGNED_BYTE, 
                    data
                );
                stbi_image_free(data);
            } else {
                std::cout << "Cubemap texture failed to load at path: " << facesCubemap[i] << std::endl;
                stbi_image_free(data);
            }
        }

        shader->Bind();
        shader->SetUniform1i("skybox", 10);
    }

    void Skybox::Draw(Camera &camera, int width, int height) {
        glDepthFunc(GL_LEQUAL);

        shader->Bind();
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        if(camera.EnttComp) {
            auto &transform = Scene::m_Registry.get<Experimental::Transform>(camera.entity);
            view = glm::mat4(glm::mat3(glm::lookAt(transform.position, transform.position + transform.rotation, camera.Up)));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 5000.0f);
        }
        else {
            TransformComponent transform = camera.GetComponent<TransformComponent>();
            view = glm::mat4(glm::mat3(glm::lookAt(transform.position, transform.position + transform.rotation, camera.Up)));
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 5000.0f);
        }

        shader->SetUniformMat4("view", view);
        shader->SetUniformMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE10);
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

    void Renderer::Swap(Shader &framebufferShader, unsigned int FBO, unsigned int rectVAO, unsigned int postProcessingTexture, unsigned int postProcessingFBO) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
            Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(0.0f, 0.0f)},
            Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)},
            Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(1.0f, 1.0f)},
            Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f)}
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        Material material(Vector4(1,1,1,1), { Texture(texPath, 0, "texture_diffuse") });
        m_Mesh = new Mesh(vertices, indices, material);
    }

    void Sprite::Draw(Shader &shader, Camera &camera, glm::mat4 trans) {
        m_Mesh->Draw(shader, camera, trans);
    }

    Spritesheet::Spritesheet(const char *texPath, Material &mat, Vector2 sheetSize, Vector2 spriteSize, Vector2 spriteCoords) {
        // square vertex
        float xCoord = spriteCoords.x + spriteSize.x;
        float yCoord = (sheetSize.y - (spriteCoords.y + spriteSize.y)) + spriteSize.y;
        std::vector<Vector2> texCoords = {
            Vector2(spriteCoords.x / sheetSize.x, (sheetSize.y - (spriteCoords.y + spriteSize.y)) / sheetSize.y),
            Vector2(xCoord / sheetSize.x, (sheetSize.y - (spriteCoords.y + spriteSize.y)) / sheetSize.y),
            Vector2(xCoord / sheetSize.x, yCoord / sheetSize.y),
            Vector2(spriteCoords.x / sheetSize.x, yCoord / sheetSize.y)
        };

        std::vector<Vertex> vertices = {
            Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), 
            texCoords[0]},

            Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), 
            texCoords[1]},

            Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), 
            texCoords[2]},

            Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), 
            texCoords[3]}
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        // Material material(Vector4(1,1,1,1), { Texture(texPath, 0, "texture_diffuse") });
        m_Mesh = new Mesh(vertices, indices, mat);
    }

    void Spritesheet::Draw(Shader &shader, Camera &camera) {
        m_Mesh->Draw(shader, camera);
    }

    Graphic::Graphic(Vector3 rgb) {
        std::vector<HyperAPI::Vertex> vertices = {
            { glm::vec3(-0.5, -0.5, 0), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(0, 0) },
            { glm::vec3(-0.5, 0.5, 0), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(0, 1) },
            { glm::vec3(0.5, 0.5, 0), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(1, 1) },
            { glm::vec3(0.5, -0.5, 0), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(1, 0) },
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        Material material(Vector4(rgb, 1));
        m_Mesh = new Mesh(vertices, indices, material);
    }

    void Graphic::Draw(Shader &shader, Camera &camera) {
        m_Mesh->Draw(shader, camera);
    }

    Capsule::Capsule(Vector4 color) : Model("assets/models/default/capsule/capsule.obj", false, color) {}
    Cube::Cube(Vector4 color) : Model("assets/models/default/cube/cube.obj", false, color) {}
    Plane::Plane(Vector4 color) {
        std::vector<HyperAPI::Vertex> vertices =
        {
            {glm::vec3(-0.5, 0, 0.5), glm::vec3(0.3,0.3,0.3), glm::vec3(0,1,0), glm::vec2(0, 0)},
            {glm::vec3(-0.5, 0, -0.5), glm::vec3(0.3,0.3,0.3), glm::vec3(0,1,0), glm::vec2(0, 1)},
            {glm::vec3(0.5, 0, -0.5), glm::vec3(0.3,0.3,0.3), glm::vec3(0,1,0), glm::vec2(1, 1)},
            {glm::vec3(0.5, 0, 0.5), glm::vec3(0.3,0.3,0.3), glm::vec3(0,1,0), glm::vec2(1, 0)}
        };

        std::vector<unsigned int> indices =
        {
            0, 1, 2,
            0, 2, 3
        };

        this->color = color;
        Material material(color, {});
        m_Mesh = new Mesh(vertices, indices, material, false);
    }

    void Plane::Draw(Shader &shader, Camera &camera) {
        m_Mesh->Draw(shader, camera);
    }

    Sphere::Sphere(Vector4 color) : Model("assets/models/default/sphere/sphere.obj", false, color) {}
    Cylinder::Cylinder(Vector4 color) : Model("assets/models/default/cylinder/cylinder.obj", false, color) {}
    Cone::Cone(Vector4 color) : Model("assets/models/default/cone/cone.obj", false, color) {}
    Torus::Torus(Vector4 color) : Model("assets/models/default/torus/torus.obj", false, color) {}

    SpriteShader::SpriteShader() : Shader("NULL_SHADER") {
        const char *vertShaderCode = 
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec3 color;\n"
        "layout(location = 2) in vec3 aNormal;\n"
        "layout(location = 3) in vec2 g_texCoords;\n"

        "out vec2 texCoords;\n"
        "out vec3 Color;\n"
        "out vec3 Normal;\n"
        "out vec3 currentPosition;\n"
        "out vec3 reflectedVector;\n"

        "uniform mat4 camera;\n"
        "uniform mat4 translation;\n"
        "uniform mat4 rotation;\n"
        "uniform mat4 scale;\n"
        "uniform mat4 model;\n"
        "uniform vec3 cameraPosition;\n"

        "void main() {\n"
        "    vec4 worldPosition = model * vec4(position, 1.0);\n"
        "    currentPosition = vec3(model * translation * rotation * scale * vec4(position, 1.0));\n"
        "    gl_Position = camera * vec4(currentPosition, 1.0);\n"
        "    texCoords = g_texCoords;\n"
        "    Color = color;\n"
        "    Normal = aNormal;\n"

        "    vec3 viewVector = normalize(worldPosition.xyz - cameraPosition);\n"
        "    reflectedVector = reflect(viewVector, Normal);\n"
        "}\n";

        const char *fragShaderCode = 
        "#version 330 core\n"
        "in vec2 texCoords;\n"
        "in vec3 Color;\n"
        "in vec3 Normal;\n"
        "in vec3 currentPosition;\n"
        "in vec3 reflectedVector;\n"
        "struct PointLight {\n"
        "    vec2 lightPos;\n"
        "    vec3 color;\n"
        "    float range;\n"
        "};\n"
        "uniform float ambient;\n"
        "#define MAX_POINT_LIGHTS 100\n"
        "uniform PointLight pointLights[MAX_POINT_LIGHTS];\n"
        "uniform int isTex;\n"
        "uniform sampler2D texture_diffuse0;\n"
        "uniform sampler2D texture_specular0;\n"
        "uniform sampler2D shadowMap;\n"
        "uniform samplerCube cubeMap;\n"
        "uniform vec3 cameraPosition;\n"
        "uniform vec3 DefaultColor;\n"
        "vec4 pointLight(PointLight light) {\n"
        "    if(isTex == 1) {\n"
        "        vec4 frag_color = texture(texture_diffuse0, texCoords);\n"
        "        if(frag_color.a < 1.0)"
        "            discard;\n"
        "        float distance = distance(light.lightPos, currentPosition.xy);\n"
        "        float diffuse = 0.0;\n"
        "        if (distance <= light.range)"
        "            diffuse =  1.0 - abs(distance / light.range);\n"
        "        return vec4(min(frag_color.rgb * ((light.color * diffuse)), frag_color.rgb), 1.0);\n"
        "    } else {\n"
        "        vec4 frag_color = vec4(DefaultColor, 1.0);\n"
        "        if(frag_color.a < 1.0)"
        "            discard;\n"
        "        float distance = distance(light.lightPos, currentPosition.xy);\n"
        "        float diffuse = 0.0;\n"
        "        if (distance <= light.range)"
        "            diffuse =  1.0 - abs(distance / light.range);\n"
        "        return vec4(frag_color.rgb * ((light.color * diffuse)), frag_color.rgb);\n"
        "    }\n"
        "}\n"
        "void main() {\n"
        "    bool lights = false;\n"
        "    vec4 result;\n"
        "    if(isTex == 1) {\n"
        "        result = texture(texture_diffuse0, texCoords) * ambient;\n"
        "    } else {\n"
        "        result = vec4(DefaultColor, 1) * ambient;\n"
        "    }\n"
        "    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {\n"
        "        if(Scene::pointLights[i].range > 0) {\n"
        "            result += pointLight(Scene::pointLights[i]);\n"
        "            lights = true;\n"
        "        } else {\n"
        "            if(!lights)"
        "                lights = false;\n"
        "        }\n"
        "    }\n"
        "    if(!lights) {\n"
        "        if(isTex == 1) {\n"
        "            result = texture(texture_diffuse0, texCoords) * ambient;\n"
        "        } else {\n"
        "            result = vec4(DefaultColor, 1) * ambient;\n"
        "        }\n"
        "    }\n"
        "    gl_FragColor = result;\n"
        "}\n";

        const char *geometryShaderCode = "";
        unsigned int vertShader, fragShader, geometryShader;
        int success;
        char infoLog[512];

        vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vertShaderCode, NULL);
        glCompileShader(vertShader);
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
            std::cout << "Failed to compile vertex shader" << std::endl;
            std::cout << infoLog << std::endl;
        }

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragShaderCode, NULL);
        glCompileShader(fragShader);
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
            std::cout << "Failed to compile fragment shader" << std::endl;
            std::cout << infoLog << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertShader);
        glAttachShader(ID, fragShader);
        // glAttachShader(ID, geometryShader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "Failed to link program" << std::endl;
            std::cout << infoLog << std::endl;
        }

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }

    void SpriteShader::Bind() {
        glUseProgram(ID);
    }

    void SpriteShader::Unbind() {
        glUseProgram(0);
    }

    void SpriteShader::SetUniform1f(const char *name, float value) {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void SpriteShader::SetUniform1i(const char *name, int value) {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void SpriteShader::SetUniform2f(const char *name, float value1, float value2) {
        glUniform2f(glGetUniformLocation(ID, name), value1, value2);
    }

    void SpriteShader::SetUniform3f(const char *name, float value1, float value2, float value3) {
        glUniform3f(glGetUniformLocation(ID, name), value1, value2, value3);
    }

    void SpriteShader::SetUniform4f(const char *name, float value1, float value2, float value3, float value4) {
        glUniform4f(glGetUniformLocation(ID, name), value1, value2, value3, value4);
    }
    void SpriteShader::SetUniformMat4(const char *name, glm::mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    Material::Material(Vector4 baseColor, std::vector<Texture> textures, float shininess, float metallic, float roughness) {
        this->baseColor = baseColor;
        this->shininess = shininess;
        this->metallic = metallic;
        this->textures = textures;
        this->roughness = roughness;

        for(auto &tex : textures) {
            if(std::string(tex.texType) == "texture_diffuse") {
                this->diffuse = &tex;
            } else if(std::string(tex.texType) == "texture_specular") {
                this->specular = &tex;
            } else if(std::string(tex.texType) == "texture_normal") {
                this->normal = &tex;
            }
        }

        if(diffuse != nullptr) {
            std::cout << "Diffuse: " << diffuse->texType << std::endl;
        }
    }

    void Material::Bind(Shader &shader) {
        // unsigned int diffuse = 0;
        // unsigned int specular = 0;
        // unsigned int normal = 0;

        shader.Bind();
    
        shader.SetUniform4f("baseColor", baseColor.x, baseColor.y, baseColor.z, baseColor.w);
        shader.SetUniform1f("shininess", shininess);
        shader.SetUniform1f("metallic", metallic);
        shader.SetUniform1f("roughness", roughness);
        shader.SetUniform2f("texUvOffset", texUVs.x, texUVs.y);

        if(diffuse != nullptr) {
            diffuse->Bind(0);

            shader.SetUniform1i("isTex", 1);
            shader.SetUniform1i("texture_diffuse0", 0);
        } else {
            shader.SetUniform1i("isTex", 0);
        }

        if(specular != nullptr) {
            specular->Bind(1);
            shader.SetUniform1i("texture_specular0", 1);
        } else {
            shader.SetUniform1i("texture_specular0", -1);
        }
    
        if(normal != nullptr) {
            normal->Bind(2);
            shader.SetUniform1i("texture_normal0", 2);
            shader.SetUniform1i("hasNormalMap", 1);
        } else {
            shader.SetUniform1i("hasNormalMap", 1);
        }
    }

    void Material::Unbind(Shader &shader) {
        if(diffuse != nullptr)
            diffuse->Unbind();

        if(specular != nullptr)
            specular->Unbind();

        if(normal != nullptr)
            normal->Unbind();

        shader.Unbind();
    }

    BatchLayer::BatchLayer(std::vector<Vertex_Batch> &vertices, std::vector<unsigned int> &indices) {
        this->vertices = vertices;
        this->indices = indices;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &IBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_Batch) * 1000, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)0);
        glEnableVertexAttribArray(0);

        //color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, color));
        glEnableVertexAttribArray(1);

        // normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, normal));
        glEnableVertexAttribArray(2);

        //texuv
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, texUV));
        glEnableVertexAttribArray(3);

        // diffuse
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, diffuse));
        glEnableVertexAttribArray(4);

        // specular
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, specular));
        glEnableVertexAttribArray(5);

        // normalmap
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, normalMap));
        glEnableVertexAttribArray(6);

        //metallic
        glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, metallic));
        glEnableVertexAttribArray(7);

        //roughness
        glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, roughness));
        glEnableVertexAttribArray(8);

        //texuvoffset
        glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, texUVs));
        glEnableVertexAttribArray(9);

        //position vec3
        glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, m_position));
        glEnableVertexAttribArray(10);

        // rotation
        glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, rotation));
        glEnableVertexAttribArray(11);

        // scale
        glVertexAttribPointer(12, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Batch), (void*)offsetof(Vertex_Batch, scale));
        glEnableVertexAttribArray(12);

        glBindVertexArray(0);
    }

    void BatchLayer::Draw(Shader &shader, Camera &camera) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex_Batch) * 1000, vertices.data());

        shader.Bind();
        shader.SetUniformMat4("camera", camera.camMatrix);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        shader.Unbind();
    }

    QuadBatch::QuadBatch(BatchLayer *layer, std::vector<Vertex_Batch> &vertices, std::vector<unsigned int> &indices) : layer(layer) {
        this->layer = layer;
        vertices.push_back(
            Vertex_Batch(glm::vec3(-0.5, 0, 0.5), glm::vec3(1,1,1), glm::vec3(0,1,0), glm::vec2(0, 0), 1)
        );
        vertices.push_back(
            Vertex_Batch(glm::vec3(-0.5, 0, -0.5), glm::vec3(1,1,1), glm::vec3(0,1,0), glm::vec2(0, 1), 1)
        );
        vertices.push_back(
            Vertex_Batch(glm::vec3(0.5, 0, -0.5), glm::vec3(1,1,1), glm::vec3(0,1,0), glm::vec2(1, 1), 1)
        );
        vertices.push_back(
            Vertex_Batch(glm::vec3(0.5, 0, 0.5), glm::vec3(1,1,1), glm::vec3(0,1,0), glm::vec2(1, 0), 1)
        );

        vert1 = vertices.size() - 4;
        vert2 = vertices.size() - 3;
        vert3 = vertices.size() - 2;
        vert4 = vertices.size() - 1;

        // get the last 4 vertices for indices
        int index = vertices.size() - 4;
        indices.push_back(index);
        indices.push_back(index + 1);
        indices.push_back(index + 2);

        indices.push_back(index);
        indices.push_back(index + 2);
        indices.push_back(index + 3);
    }

    void QuadBatch::Update() {
        try {
            layer->vertices[vert1].m_position = transform.position;
        layer->vertices[vert2].m_position = transform.position;
        layer->vertices[vert3].m_position = transform.position;
        layer->vertices[vert4].m_position = transform.position;

        layer->vertices[vert1].rotation = transform.rotation;
        layer->vertices[vert2].rotation = transform.rotation;
        layer->vertices[vert3].rotation = transform.rotation;
        layer->vertices[vert4].rotation = transform.rotation;

        layer->vertices[vert1].scale = transform.scale;
        layer->vertices[vert2].scale = transform.scale;
        layer->vertices[vert3].scale = transform.scale;
        layer->vertices[vert4].scale = transform.scale;
        }
        catch(...) {}
    }

    namespace Experimental {
        void Model::Draw(Shader &shader, Camera &camera)
        {   
            Transform &mainTransform = mainGameObject->GetComponent<Transform>();
            mainTransform.Update();

            for(unsigned int i = 0; i < m_gameObjects.size(); i++) {
                MeshRenderer &meshRenderer = m_gameObjects[i]->GetComponent<MeshRenderer>();
                meshRenderer.m_Mesh->Draw(shader, camera, mainTransform.transform);
            }
        }

        void Model::loadModel(std::string path) {            
            Assimp::Importer import;
            const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate);	
            
            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
            {
                std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
                return;
            }
            directory = path.substr(0, path.find_last_of('/'));

            processNode(scene->mRootNode, scene);
        }  

        void Model::processNode(aiNode *node, const aiScene *scene)
        {
            // process all the node's meshes (if any)
            for(unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
                // get name
                std::string name = mesh->mName.C_Str();

                //get matricies
                aiMatrix4x4 aiTransform = node->mTransformation;
                transform = glm::mat4(1.0f);
                transform[0][0] = aiTransform.a1; transform[1][0] = aiTransform.b1; transform[2][0] = aiTransform.c1; transform[3][0] = aiTransform.d1;
                transform[0][1] = aiTransform.a2; transform[1][1] = aiTransform.b2; transform[2][1] = aiTransform.c2; transform[3][1] = aiTransform.d2;
                transform[0][2] = aiTransform.a3; transform[1][2] = aiTransform.b3; transform[2][2] = aiTransform.c3; transform[3][2] = aiTransform.d3;
                transform[0][3] = aiTransform.a4; transform[1][3] = aiTransform.b4; transform[2][3] = aiTransform.c4; transform[3][3] = aiTransform.d4;
            
                m_gameObjects.push_back(processMesh(mesh, scene, name));
                auto &meshRenderer = m_gameObjects[i]->GetComponent<MeshRenderer>();
                meshRenderer.extraMatrix = transform;			
            }
            // then do the same for each of its children
            for(unsigned int i = 0; i < node->mNumChildren; i++)
            {
                processNode(node->mChildren[i], scene);
            }
        }   

        GameObject *Model::processMesh(aiMesh *mesh, const aiScene *scene, const std::string &name)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            for(unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                if(mesh->mTextureCoords[0])
                {
                    vertex.texUV = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }
                else
                {
                    vertex.texUV = glm::vec2(0.0f, 0.0f);
                }
                vertices.push_back(vertex);
            }

            //indices
            for(unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for(unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }  

            Texture *diffuse = nullptr;
            Texture *specular = nullptr;
            Texture *normal = nullptr;

            if(mesh->mMaterialIndex >= 0 && texturesEnabled == true)
            {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
                diffuse = &diffuseMaps[0];
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
                std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
                specular = &specularMaps[0];
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            }

            if(texturesEnabled) {
                Material material(Color, textures);
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

                return gameObject;
            } else {
                Material material(Color, textures);
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
                
                return gameObject;
            }
        }   

        std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
        {
            std::vector<Texture> textures;
            for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                bool skip = false;
                std::string texPath = directory + '/' + str.C_Str();
                for(unsigned int j = 0; j < textures_loaded.size(); j++)
                {
                    if(std::strcmp(textures_loaded[j].texStarterPath, texPath.c_str()) == 0)
                    {
                        textures.push_back(textures_loaded[j]);
                        skip = true; 
                        break;
                    }
                }

                if(!skip)
                {   // if texture hasn't been loaded already, load it
                    std::string texPath = directory + '/' + str.C_Str();
                    Texture texture(texPath.c_str(), i, typeName.c_str());
                    textures.push_back(texture);
                    textures_loaded.push_back(texture); // add to loaded textures
                    currSlot++;
                }
            }
            return textures;
        }  
    }

    namespace f_GameObject {
        Experimental::GameObject *FindGameObjectByName(const std::string &name) {
            for(auto &gameObject : Scene::m_GameObjects) {
                if(gameObject->name == name) {
                    return gameObject;
                }
            }

            return nullptr;
        }

        Experimental::GameObject *FindGameObjectByTag(const std::string &tag) {
            for(auto &gameObject : Scene::m_GameObjects) {
                if(gameObject->tag == tag) {
                    return gameObject;
                }
            }

            return nullptr;
        }
    }
}

namespace Hyper {
    void Application::Run(std::function<void()> update, std::function<void(unsigned int &PPT, unsigned int &PPFBO)> gui) {
        HYPER_LOG("Application started");
        float gamma = 2.2f;

        HyperAPI::Shader shadowMapProgram("shaders/shadowMap.glsl");

        HyperAPI::Shader framebufferShader("shaders/framebuffer.glsl");
        framebufferShader.Bind();
        framebufferShader.SetUniform1f("gamma", gamma);
        framebufferShader.SetUniform1f("exposure", 0.1f);
        // HyperAPI::Shader shader("shaders/sprite.glsl");
        // shader.Bind();
        // shader.SetUniform1f("ambient", 0.5);

        // glfwSwapInterval(1);

        unsigned int rectVAO, rectVBO;
        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &rectVBO);
        glBindVertexArray(rectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
        // dynamic
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVert), &rectangleVert, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        unsigned int FBO;
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        unsigned int bufferTexture;
        glGenTextures(1, &bufferTexture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bufferTexture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderer->samples, GL_RGB16F, width, height, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, bufferTexture, 0);
        
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderer->samples, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        unsigned int postProcessingFBO;
        glGenFramebuffers(1, &postProcessingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

        unsigned int postProcessingTexture;
        glGenTextures(1, &postProcessingTexture);
        glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

        unsigned int SFBO;
        glGenFramebuffers(1, &SFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, SFBO);

        unsigned int SbufferTexture;
        glGenTextures(1, &SbufferTexture);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, SbufferTexture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderer->samples, GL_RGB16F, width, height, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, SbufferTexture, 0);
        
        unsigned int SRBO;
        glGenRenderbuffers(1, &SRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, SRBO);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderer->samples, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, SRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        unsigned int S_PPFBO;
        glGenFramebuffers(1, &S_PPFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, S_PPFBO);

        unsigned int S_PPT;
        glGenTextures(1, &S_PPT);
        glBindTexture(GL_TEXTURE_2D, S_PPT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, S_PPT, 0);

        float timeStep = 1.0f / 60.0f;

        unsigned int shadowMapFBO;
        glGenFramebuffers(1, &shadowMapFBO);

        unsigned int shadowMapWidth = 2042, shadowMapHeight = 2042;
        unsigned int shadowMap;
        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);


        float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        HYPER_LOG("Renderer initialized");

        // Matrices needed for the light's perspective
        glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
        glm::mat4 lightView = glm::lookAt(20.0f * glm::vec3(0.5, 0.5, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightProjection = orthgonalProjection * lightView;

        shadowMapProgram.Bind();
        shadowMapProgram.SetUniformMat4("lightProjection", lightProjection);
        HyperAPI::Scene::projection = lightProjection;

        while(!glfwWindowShouldClose(renderer->window)) {
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

            glfwPollEvents();

            glEnable(GL_DEPTH_TEST);
            glViewport(0, 0, shadowMapWidth, shadowMapHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            // for(auto &Mesh : HyperAPI::Scene::entities) {
            //     if(Mesh->empty) continue;
            //     m_Mesh->Draw(shadowMapProgram, *HyperAPI::Scene::mainCamera);
            // }

            // for(auto &model : HyperAPI::Scene::models) {
            //     model.Draw(shadowMapProgram, *HyperAPI::Scene::mainCamera);
            // }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // glfwGetWindowSize(renderer->window, &width, &height);

            // delete old framebuffer stuff please
            glDeleteFramebuffers(1, &FBO);
            glDeleteTextures(1, &bufferTexture);
            // check if rbo is needed
            try {
                glDeleteRenderbuffers(1, &rbo);
            }
            catch(...) {
                std::cout << "RBO not deleted" << std::endl;
            }
            // post processing delete them
            glDeleteFramebuffers(1, &postProcessingFBO);
            glDeleteTextures(1, &postProcessingTexture);

            glDeleteRenderbuffers(1, &SRBO);
            glDeleteFramebuffers(1, &SFBO);
            glDeleteFramebuffers(1, &S_PPFBO);
            glDeleteTextures(1, &S_PPT);
            glDeleteTextures(1, &SbufferTexture);

            // ----------

            // glGenFramebuffers(1, &mousePickFBO);
            // glBindFramebuffer(GL_FRAMEBUFFER, mousePickFBO);

            // glGenTextures(1, &mousePickTexture);
            // glBindTexture(GL_TEXTURE_2D, mousePickTexture);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mousePickTexture, 0);
        
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            glGenTextures(1, &bufferTexture);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bufferTexture);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderer->samples, GL_RGB16F, width, height, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, bufferTexture, 0);
            
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderer->samples, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

            glGenFramebuffers(1, &postProcessingFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

            glGenTextures(1, &postProcessingTexture);
            glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

            // delete srbo sfbo sppfbo sppt

            glGenFramebuffers(1, &SFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, SFBO);

            glGenTextures(1, &SbufferTexture);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, SbufferTexture);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, renderer->samples, GL_RGB16F, width, height, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, SbufferTexture, 0);
            
            glGenRenderbuffers(1, &SRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, SRBO);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, renderer->samples, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, SRBO);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

            glGenFramebuffers(1, &S_PPFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, S_PPFBO);

            glGenTextures(1, &S_PPT);
            glBindTexture(GL_TEXTURE_2D, S_PPT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, S_PPT, 0);

            if(renderOnScreen) {
                glfwGetWindowSize(renderer->window, &width, &height);
            }


            glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
            // make it so that the framebuffer keeps the size of 1280x720 no matter the window size

            float rectangleVert[] = {
                1, -1,  1, 0,
                -1, -1,  0, 0,
                -1, 1,  0, 1,

                1, 1,  1, 1,
                1, -1,  1, 0,
                -1, 1,  0, 1,
            };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rectangleVert), rectangleVert);
            
            NewFrame(FBO, width, height);
            glActiveTexture(GL_TEXTURE17);
            glBindTexture(GL_TEXTURE_2D, shadowMap);
            update();

            glClear(GL_DEPTH_BUFFER_BIT);
            if(!renderOnScreen) {
                EndEndFrame(framebufferShader, *renderer, FBO, rectVAO, postProcessingTexture, postProcessingFBO, SFBO, S_PPT, S_PPFBO, width, height);
            } else {
                EndFrame(framebufferShader, *renderer, FBO, rectVAO, postProcessingTexture, postProcessingFBO, width, height);
            }

            // HyperGUI::FrameBufferTexture = postProcessingTexture;
            // im gui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            gui(S_PPT, SFBO);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(renderer->window);

        }
    
        HYPER_LOG("Closing Static Engine");
    }

    MousePicker::MousePicker(Application *app, HyperAPI::Camera *camera, glm::mat4 projection) {
        this->camera = camera;
        this->projectionMatrix = projection;
        this->appRef = app;
    }

    Vector3 MousePicker::getCurrentRay() {
        return currentRay;
    }

    void MousePicker::update() {
        currentRay = calculateMouseRay();
    }

    Vector3 MousePicker::calculateMouseRay() {
        Vector2 normalizedCoords = getNormalizedDeviceCoords(mouseX, mouseY);
        Vector4 clipCoords = Vector4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
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

    float LerpFloat(float a, float b, float t) {
        return a + t * (b - a);
    }
}