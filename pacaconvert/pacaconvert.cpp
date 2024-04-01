#include "pacaconvert.hpp"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include <unordered_set>
#include <vector>

namespace paca_format {

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

Material processMaterial(aiMaterial *material, const std::string &outName, unsigned int indexOfMaterial)
{
    Material result;
    result.textures[TextureType::diffuse] = loadMaterialTextures(material, aiTextureType_DIFFUSE);
    result.textures[TextureType::specular] = loadMaterialTextures(material, aiTextureType_SPECULAR);
    result.textures[TextureType::normal] = loadMaterialTextures(material, aiTextureType_NORMALS);
    result.textures[TextureType::depth] = loadMaterialTextures(material, aiTextureType_HEIGHT);

    result.name = outName + "Material" + std::to_string(indexOfMaterial);

    return result;
}

Mesh processMesh(aiMesh *mesh, const aiScene *scene, const std::string &outName, std::unordered_set<unsigned int> &usedMaterialIndexes)
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

    VertexType vertexType;
    if (mesh->HasTangentsAndBitangents())
        vertexType = VertexType::float3pos_float3norm_float3tang_float2texture;
    else
        vertexType = VertexType::float3pos_float3norm_float2texture;

    usedMaterialIndexes.insert(mesh->mMaterialIndex);

    return Mesh{vertexType, IndexType::int32bit, vertices, indices, outName + "Material" + std::to_string(mesh->mMaterialIndex)};
}

void processNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshesOut, const std::string &outName, std::unordered_set<unsigned int> &usedMaterialIndexes)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshesOut.push_back(processMesh(mesh, scene, outName, usedMaterialIndexes));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, meshesOut, outName, usedMaterialIndexes);
    }
}

std::optional<ModelAndMaterials> modelToPacaFormat(const std::string &path, const std::string &outName)
{
    Model model;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace);
    std::unordered_set<unsigned int> usedMaterialIndexes;

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "%s", importer.GetErrorString());
        fprintf(stderr, "Error loading file: %s.\n", path.c_str());
        return {};
    }

    processNode(scene->mRootNode, scene, model.meshes, outName, usedMaterialIndexes);

    std::vector<Material> materials;

    for (const unsigned int &i : usedMaterialIndexes)
        materials.emplace_back(processMaterial(scene->mMaterials[i], outName, i));

    return ModelAndMaterials{model, materials};
}

} // namespace paca_format
