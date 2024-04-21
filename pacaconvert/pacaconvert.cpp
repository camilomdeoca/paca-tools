#include "pacaconvert.hpp"
#include "pacaread/pacaread.hpp"

#include <assimp/Importer.hpp>
#include <assimp/anim.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstddef>
#include <cstring>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace paca_format {

static inline glm::mat4 toGlmMatrix(const aiMatrix4x4& from)
{
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

static inline glm::vec3 toGlmVec(const aiVector3D& vec)
{
    return glm::vec3(vec.x, vec.y, vec.z);
}


static inline glm::quat toGlmQuat(const aiQuaternion& quat)
{
    return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

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

void processAnimation(aiAnimation *assimpAnim, Animation &outAnimation, const std::unordered_map<std::string, uint32_t> &boneNameToId, unsigned int indexOfAnimation)
{
    outAnimation.name = std::string(assimpAnim->mName.C_Str()) + "Animation" + std::to_string(indexOfAnimation);
    outAnimation.duration = assimpAnim->mDuration;
    outAnimation.ticksPerSecond = assimpAnim->mTicksPerSecond;
    outAnimation.keyframes.resize(boneNameToId.size());
    // For each bone (There is a channel with keyframes for each bone)
    for (unsigned int i = 0; i < assimpAnim->mNumChannels; i++)
    {
        std::unordered_map<std::string, uint32_t>::const_iterator it = boneNameToId.find(assimpAnim->mChannels[i]->mNodeName.C_Str());
        if (it == boneNameToId.end())
        {
            printf("Error: animation channel for non bone node.\n");
            continue;
        }
        uint32_t boneId = it->second;
        // For each position keyframe
        for (unsigned int keyFrameIndex = 0; keyFrameIndex < assimpAnim->mChannels[i]->mNumPositionKeys; keyFrameIndex++)
        {
            outAnimation.keyframes[boneId].positions.emplace_back(
                    assimpAnim->mChannels[i]->mPositionKeys[keyFrameIndex].mTime,
                    toGlmVec(assimpAnim->mChannels[i]->mPositionKeys[keyFrameIndex].mValue));
        }
        // For each rotation keyframe
        for (unsigned int keyFrameIndex = 0; keyFrameIndex < assimpAnim->mChannels[i]->mNumRotationKeys; keyFrameIndex++)
        {
            outAnimation.keyframes[boneId].rotations.emplace_back(
                    assimpAnim->mChannels[i]->mRotationKeys[keyFrameIndex].mTime,
                    toGlmQuat(assimpAnim->mChannels[i]->mRotationKeys[keyFrameIndex].mValue));
        }
        // For each scaling keyframe
        for (unsigned int keyFrameIndex = 0; keyFrameIndex < assimpAnim->mChannels[i]->mNumScalingKeys; keyFrameIndex++)
        {
            outAnimation.keyframes[boneId].scalings.emplace_back(
                    assimpAnim->mChannels[i]->mScalingKeys[keyFrameIndex].mTime,
                    toGlmVec(assimpAnim->mChannels[i]->mScalingKeys[keyFrameIndex].mValue));
        }
    }
}

struct BoneData {
    uint32_t id;
    glm::mat4 offset;
};

void processBoneChilds(aiNode *node, Skeleton &outSkeleton, const std::unordered_map<std::string, BoneData> &bonesData, uint32_t parentID)
{
    auto it = bonesData.find(node->mName.C_Str());
    if (it != bonesData.end()) // if node is a bone
    {
        outSkeleton.bones.emplace_back(parentID, it->second.offset);
        outSkeleton.boneNames.emplace_back(node->mName.C_Str());
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
			processBoneChilds(node->mChildren[i], outSkeleton, bonesData, it->second.id);
		}
    }
    else
    {
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
			processBoneChilds(node->mChildren[i], outSkeleton, bonesData, parentID);
		}
    }
}

template<typename T>
void insertIntoCharVector(T value, std::vector<char> &vector)
{
    const char *bytes = reinterpret_cast<const char*>(&value);
    vector.insert(vector.end(), bytes, bytes + sizeof(T));
}

template<typename T>
void insertIntoCharVector(T value, std::vector<char> &vector, std::vector<char>::iterator position)
{
    const char *bytes = reinterpret_cast<const char*>(&value);
    vector.insert(position, bytes, bytes + sizeof(T));
}

Mesh processMesh(aiMesh *mesh, const aiScene *scene, const std::string &outName, std::unordered_set<unsigned int> &usedMaterialIndexes)
{
    constexpr size_t MAX_BONES_INFLUENCE = 4;

    Mesh result;

    if (mesh->HasBones())
        result.vertexType = VertexType::float3pos_float3norm_float3tang_float2texture_int4boneIds_float4boneWeights;
    else if (mesh->HasTangentsAndBitangents())
        result.vertexType = VertexType::float3pos_float3norm_float3tang_float2texture;
    else
        result.vertexType = VertexType::float3pos_float3norm_float2texture;

    struct BoneInfluence {
        int32_t boneID;
        float weight;
    };

    // Get bones influences
    std::vector<std::vector<BoneInfluence>> bonesInfluences(mesh->mNumVertices); // The index is vertexID
    for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
    {
        for (unsigned int weightIndex = 0; weightIndex < mesh->mBones[boneIndex]->mNumWeights; weightIndex++)
        {
            unsigned int vertexID = mesh->mBones[boneIndex]->mWeights[weightIndex].mVertexId;
            float weight = mesh->mBones[boneIndex]->mWeights[weightIndex].mWeight;
            bonesInfluences[vertexID].emplace_back(mesh->mNumBones - 1 - boneIndex, weight);

            //Bone &newBone = result.bones.emplace_back();
            //std::memcpy(newBone.offset, &mesh->mBones[boneIndex]->mOffsetMatrix, sizeof(newBone.offset));
            //newBone.id = boneIndex;
            //newBone.name = mesh->mBones[boneIndex]->mName.C_Str();
        }
    }

    // Get vertices
    for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++)
    {
        insertIntoCharVector(mesh->mVertices[vertexIndex].x, result.vertices);
        insertIntoCharVector(mesh->mVertices[vertexIndex].y, result.vertices);
        insertIntoCharVector(mesh->mVertices[vertexIndex].z, result.vertices);

        bool hasNormals = mesh->HasNormals();
        insertIntoCharVector(hasNormals ? mesh->mNormals[vertexIndex].x : 0, result.vertices);
        insertIntoCharVector(hasNormals ? mesh->mNormals[vertexIndex].y : 0, result.vertices);
        insertIntoCharVector(hasNormals ? mesh->mNormals[vertexIndex].z : 0, result.vertices);

        bool hasTangents = mesh->HasTangentsAndBitangents();
        insertIntoCharVector(hasTangents ? mesh->mTangents[vertexIndex].x : 0, result.vertices);
        insertIntoCharVector(hasTangents ? mesh->mTangents[vertexIndex].y : 0, result.vertices);
        insertIntoCharVector(hasTangents ? mesh->mTangents[vertexIndex].z : 0, result.vertices);

        bool hasTextureCoords = mesh->mTextureCoords[0];
        insertIntoCharVector(hasTextureCoords ? mesh->mTextureCoords[0][vertexIndex].x : 0, result.vertices);
        insertIntoCharVector(hasTextureCoords ? mesh->mTextureCoords[0][vertexIndex].y : 0, result.vertices);

        if (mesh->HasBones())
        {
            // Set bone IDs
            for (size_t i = 0; i < MAX_BONES_INFLUENCE; i++)
            {
                if (i < bonesInfluences[vertexIndex].size())
                    insertIntoCharVector(bonesInfluences[vertexIndex][i].boneID, result.vertices);
                else
                    insertIntoCharVector(static_cast<uint32_t>(std::numeric_limits<uint32_t>::max()), result.vertices);
                //assert(bonesInfluences[vertexIndex].size() <= MAX_BONES_INFLUENCE && "Too much bones influence a vertex");
            }
            // Set bone weights
            for (size_t i = 0; i < MAX_BONES_INFLUENCE; i++)
            {
                if (i < bonesInfluences[vertexIndex].size())
                    insertIntoCharVector(bonesInfluences[vertexIndex][i].weight, result.vertices);
                else
                    insertIntoCharVector(static_cast<float>(0.0f), result.vertices);
                //assert(bonesInfluences[vertexIndex].size() <= MAX_BONES_INFLUENCE && "Too much bones influence a vertex");
            }
        }
    }

    // Get indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            result.indices.push_back(face.mIndices[j]);
    }

    if (mesh->mNumBones > 0)
    {
        std::unordered_map<std::string, BoneData> bonesData;
        for (unsigned int boneId = 0; boneId < mesh->mNumBones; boneId++)
        {
            bonesData.emplace(std::make_pair(
                        mesh->mBones[boneId]->mName.C_Str(),
                        BoneData{
                            mesh->mNumBones - 1 - boneId,
                            toGlmMatrix(mesh->mBones[boneId]->mOffsetMatrix)
                        }));
        }
        processBoneChilds(scene->mRootNode, result.skeleton, bonesData, std::numeric_limits<uint32_t>::max());
    }

    if (scene->HasAnimations())
    {
        for (unsigned int animIndex = 0; animIndex < scene->mNumAnimations; animIndex++)
        {
            result.animations.emplace_back(std::string(scene->mAnimations[animIndex]->mName.C_Str()) + "Animation" + std::to_string(animIndex));
        }
    }

    usedMaterialIndexes.insert(mesh->mMaterialIndex);

    result.materialName = outName + "Material" + std::to_string(mesh->mMaterialIndex);
    result.indexType = IndexType::uint32bit;

    return result;
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

ModelMaterialsAnimations modelToPacaFormat(const std::string &path, const std::string &outName)
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

    std::vector<Animation> animations;
    if (scene->HasAnimations())
    {
        std::unordered_map<std::string, uint32_t> boneNameToId;
        for (unsigned int boneId = 0; boneId < model.meshes[0].skeleton.boneNames.size(); boneId++)
        {
            boneNameToId.emplace(std::make_pair(model.meshes[0].skeleton.boneNames[boneId], boneId));
        }
        for (unsigned int animIndex = 0; animIndex < scene->mNumAnimations; animIndex++)
        {
            Animation &anim = animations.emplace_back();
            processAnimation(scene->mAnimations[0], anim, boneNameToId, animIndex);
        }
    }

    std::vector<Material> materials;

    for (const unsigned int &i : usedMaterialIndexes)
        materials.emplace_back(processMaterial(scene->mMaterials[i], outName, i));

    model.name = outName;

    return ModelMaterialsAnimations{model, materials, animations};
}

} // namespace paca_format
