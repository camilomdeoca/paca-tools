#include "material.hpp"
#include "meshes.hpp"
#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <cstdio>
#include <optional>
#include <set>
#include <string>
#include <vector>

static std::vector<Mesh> meshes;
static std::set<uint32_t> materialIndexes;
static std::vector<Material> materials;

std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}

Material processMaterial(aiMaterial *material)
{
    Material result;
    result.textures[TextureType::diffuse] = loadMaterialTextures(material, aiTextureType_DIFFUSE);
    result.textures[TextureType::specular] = loadMaterialTextures(material, aiTextureType_SPECULAR);
    result.textures[TextureType::normal] = loadMaterialTextures(material, aiTextureType_NORMALS);
    result.textures[TextureType::depth] = loadMaterialTextures(material, aiTextureType_HEIGHT);

    return result;
}

Mesh processMesh(aiMesh *mesh, const aiScene *scene, const std::string &directory)
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // Get vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        bool hasNormals = mesh->HasNormals();
        vertices.push_back(hasNormals ? mesh->mNormals[i].x : 0);
        vertices.push_back(hasNormals ? mesh->mNormals[i].y : 0);
        vertices.push_back(hasNormals ? mesh->mNormals[i].z : 0);

        bool hasTangents = mesh->HasTangentsAndBitangents();
        vertices.push_back(hasTangents ? mesh->mTangents[i].x : 0);
        vertices.push_back(hasTangents ? mesh->mTangents[i].y : 0);
        vertices.push_back(hasTangents ? mesh->mTangents[i].z : 0);

        bool hasTextureCoords = mesh->mTextureCoords[0];
        vertices.push_back(hasTextureCoords ? mesh->mTextureCoords[0][i].x : 0);
        vertices.push_back(hasTextureCoords ? mesh->mTextureCoords[0][i].y : 0);
    }

    // Get indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    printf("Num of materials %d, but using %d\n", scene->mNumMaterials, mesh->mMaterialIndex);
    //aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    //MaterialSpecification materialSpec;
    //materialSpec.textureMaps[MaterialTextureType::diffuse] = loadMaterialTextures(material, aiTextureType_DIFFUSE, directory);
    //materialSpec.textureMaps[MaterialTextureType::specular] = loadMaterialTextures(material, aiTextureType_SPECULAR, directory);
    //materialSpec.textureMaps[MaterialTextureType::normal] = loadMaterialTextures(material, aiTextureType_NORMALS, directory);
    //materialSpec.textureMaps[MaterialTextureType::normal] = loadMaterialTextures(material, aiTextureType_NORMALS, directory);
    
    VertexType::Type vertexType;
    if (mesh->HasTangentsAndBitangents())
        vertexType = VertexType::float3pos_float3norm_float3tang_float2texture;
    else
        vertexType = VertexType::float3pos_float3norm_float2texture;

    return Mesh{vertexType, vertices, indices, "Material" + std::to_string(mesh->mMaterialIndex)};
}

void processNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshesOut, const std::string &directory)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshesOut.push_back(processMesh(mesh, scene, directory));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, meshesOut, directory);
    }
}

std::optional<Model> loadModel(const std::string &path)
{
    std::vector<Mesh> meshes;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "%s", importer.GetErrorString());
        fprintf(stderr, "Error loading file: %s.\n", path.c_str());
        return {};
    }

    std::string directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene, meshes, directory);

    std::vector<Material> materials;

    materials.emplace_back(processMaterial(scene->mMaterials[0]));
    materials[0].name = "Material" + std::to_string(0);
    saveMaterial(materials[0], "material.pmat");

    return Model{meshes};
}

int main (int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <input_model>\n", argv[0]);
        return 1;
    }

    std::string inputFilepath(argv[1]);

    Assimp::Importer importer;
    std::optional<Model> model = loadModel(inputFilepath);

    if (!model.has_value())
        return 1; 

    saveModel("ModelName", model.value().meshes, "model.pmdl");
    
    return 0;
}
