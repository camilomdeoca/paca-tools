#include "pacaread.hpp"

#include <cstddef>
#include <cstring>
#include <fstream>
#include <string>
#include <uchar.h>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace paca_format {

size_t vertexTypeToSize(VertexType type)
{
    switch (type) {
        case VertexType::float3pos_float3norm_float2texture:            return (3+3+2)*sizeof(float);
        case VertexType::float3pos_float3norm_float3tang_float2texture: return (3+3+3+2)*sizeof(float);
        case VertexType::float3pos_float3norm_float3tang_float2texture_int4boneIds_float4boneWeights: return (3+3+3+2)*sizeof(float) + 4*sizeof(int32_t) + 4*sizeof(float);
        default: break;
    }

    exit(1);
}

size_t indexTypeToSize(IndexType type)
{
    switch (type) {
        case IndexType::no_indices: return 0;
        case IndexType::uint32bit:   return sizeof(uint32_t);
        default: break;
    }

    exit(1);
}

std::optional<Model> readModel(const std::string &filepath)
{
    std::vector<Mesh> meshes;
    std::ifstream file(filepath, std::ios::binary);

    if (file.fail()) {
        // Failed to open file
        return {};
    }

    headers::ModelHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // check magic bytes
    char8_t correctMagicNytes[4] = {'P', 'M', 'D', 'L'};
    for (unsigned int i = 0; i < sizeof(header.magicBytes); i++)
    {
        if (header.magicBytes[i] != correctMagicNytes[i])
            return {};
    }

    std::string modelName(header.nameLength, '\0');
    file.read(modelName.data(), header.nameLength);

    for (uint32_t i = 0; i < header.numOfMeshes; i++)
    {
        headers::MeshSubheader subheader;
        std::vector<char> vertices;
        std::vector<uint32_t> indices;
        std::vector<Bone> bones;
        std::vector<std::string> boneNames;
        file.read(reinterpret_cast<char*>(&subheader), sizeof(headers::MeshSubheader));
        vertices.resize(subheader.vertexCount * vertexTypeToSize(VertexType(subheader.vertexType)));
        indices.resize(subheader.indexCount);
        std::string materialName(subheader.materialNameLength, '\0');
        file.read(materialName.data(), subheader.materialNameLength);

        bones.reserve(subheader.boneCount);
        for (uint32_t j = 0; j < subheader.boneCount; j++)
        {
            headers::BoneSubheader boneSubheader;
            file.read(reinterpret_cast<char*>(&boneSubheader), sizeof(headers::BoneSubheader));
            std::string boneName(boneSubheader.boneNameLength, '\0');
            file.read(boneName.data(), boneName.length());
            Bone &newBone = bones.emplace_back();
            newBone.parentID = boneSubheader.parentID;
            newBone.offsetMatrix = boneSubheader.offset;
            boneNames.emplace_back(boneName);
        }

        file.read(reinterpret_cast<char*>(vertices.data()), subheader.vertexCount * vertexTypeToSize(VertexType(subheader.vertexType)));
        file.read(reinterpret_cast<char*>(indices.data()), subheader.indexCount * indexTypeToSize(IndexType(subheader.indexType)));

        meshes.emplace_back(VertexType(subheader.vertexType), IndexType(subheader.indexType), vertices, indices, materialName, Skeleton{bones, boneNames});
    }

    return Model{meshes, modelName};
}

std::optional<Material> readMaterial(const std::string &filepath)
{
    std::array<std::vector<Texture>, TextureType::last> textures;
    std::ifstream file(filepath, std::ios::binary);

    if (file.fail()) {
        // Failed to open file
        return {};
    }

    headers::MaterialHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // check magic bytes
    char8_t correctMagicBytes[4] = {'P', 'M', 'A', 'T'};
    for (unsigned int i = 0; i < sizeof(header.magicBytes); i++)
    {
        if (header.magicBytes[i] != correctMagicBytes[i])
            return {};
    }

    std::string materialName(header.nameLength, '\0');
    file.read(materialName.data(), header.nameLength);

    for (uint32_t i = 0; i < header.textureCount; i++)
    {
        headers::TextureSubheader subheader;
        file.read(reinterpret_cast<char*>(&subheader), sizeof(subheader));
        std::string texture_path(subheader.pathLength, '\0');
        file.read(texture_path.data(), texture_path.size());
        textures[subheader.textureType].emplace_back(texture_path);
    }

    return Material{textures, materialName};
}

std::optional<Animation> readAnimation(const std::string &filepath)
{
    Animation result;
    std::ifstream file(filepath, std::ios::binary);

    headers::AnimationHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    result.duration = header.duration;
    result.ticksPerSecond = header.ticksPerSecond;

    // check magic bytes
    char8_t correctMagicBytes[4] = {'P', 'A', 'N', 'I'};
    for (unsigned int i = 0; i < sizeof(header.magicBytes); i++)
    {
        if (header.magicBytes[i] != correctMagicBytes[i])
            return {};
    }

    std::string animationName(header.nameLength, '\0');
    file.read(animationName.data(), header.nameLength);
    result.name = animationName;

    result.keyframes.resize(header.boneKeyFramesCount);
    for (uint32_t boneId = 0; boneId < header.boneKeyFramesCount; boneId++)
    {
        headers::AnimationBoneSubheader animationBoneSubheader;
        file.read(reinterpret_cast<char*>(&animationBoneSubheader), sizeof(animationBoneSubheader));
        for (uint32_t i = 0; i < animationBoneSubheader.positionKeyframesCount; i++)
        {
            headers::PositionKeyframe positionKeyframe;
            file.read(reinterpret_cast<char*>(&positionKeyframe), sizeof(positionKeyframe));
            result.keyframes[boneId].positions.emplace_back(positionKeyframe.time, positionKeyframe.position);
        }
        for (uint32_t i = 0; i < animationBoneSubheader.rotationKeyframesCount; i++)
        {
            headers::RotationKeyframe rotationKeyframe;
            file.read(reinterpret_cast<char*>(&rotationKeyframe), sizeof(rotationKeyframe));
            result.keyframes[boneId].rotations.emplace_back(rotationKeyframe.time, rotationKeyframe.quaternion);
        }
        for (uint32_t i = 0; i < animationBoneSubheader.scalingKeyframesCount; i++)
        {
            headers::ScalingKeyframe scalingKeyframe;
            file.read(reinterpret_cast<char*>(&scalingKeyframe), sizeof(scalingKeyframe));
            result.keyframes[boneId].scalings.emplace_back(scalingKeyframe.time, scalingKeyframe.scaling);
        }
    }

    return result;
}

} // namespace paca_read
