#include "Model.hpp"
#include "Mesh.hpp"

namespace HyperAPI {
    void Model::Draw(Shader &shader, Camera &camera) {
        // scriptComponent.componentSystem = this;
        // scriptComponent.OnUpdate();
        for (uint32_t i = 0; i < meshes.size(); i++) {
            TransformComponent modelTransform =
                GetComponent<TransformComponent>();
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
                glm::scale(glm::mat4(1.0f),
                           Vector3(modelTransform.scale.x * 0.5,
                                   modelTransform.scale.y * 0.5,
                                   modelTransform.scale.z * 0.5));

            UpdateComponent(modelTransform);
            meshes[i]->Draw(shader, camera,
                            transform * modelTransform.transform);
        }
    }

    void Model::loadModel(std::string path) {
        ID = uuid::generate_uuid_v4();

        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString()
                      << std::endl;
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
            vertex.position =
                glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
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
            textures.insert(textures.end(), diffuseMaps.begin(),
                            diffuseMaps.end());
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
} // namespace HyperAPI