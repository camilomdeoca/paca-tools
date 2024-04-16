#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace paca_format {

enum class VertexType : uint32_t {
    float3pos_float3norm_float2texture = 1,
    float3pos_float3norm_float3tang_float2texture = 2,
    float3pos_float3norm_float3tang_float2texture_int4boneIds_float4boneWeights = 3
};

enum class IndexType : uint32_t {
    no_indices = 0,
    uint32bit = 1,
};

struct Bone {
    uint32_t parentID;
    glm::mat4 offsetMatrix;
};

struct Skeleton {
    std::vector<Bone> bones;
    std::vector<std::string> boneNames;
};

struct Mesh {
    VertexType vertexType;
    IndexType indexType;
    std::vector<char> vertices;
    std::vector<uint32_t> indices;
    std::string materialName;
    Skeleton skeleton;
};

struct Model {
    std::vector<Mesh> meshes;
    std::string name;
};

struct PositionKeyFrame {
    float time;
    glm::vec3 position;
};

struct RotationKeyFrame {
    float time;
    glm::quat quaternion;
};

struct ScaleKeyFrame {
    float time;
    glm::vec3 scale;
};

struct BoneKeyFrames {
    std::vector<PositionKeyFrame> positions;
    std::vector<RotationKeyFrame> rotations;
    std::vector<ScaleKeyFrame> scalings;
};

struct Animation {
    // Duration of the animation in ticks
    float duration;
    uint32_t ticksPerSecond;
    std::string name;

    // Keyframes of each bone
    std::vector<BoneKeyFrames> keyframes;
};

namespace TextureType {
    enum Type : uint32_t {
        none = 0,
        diffuse = 1,
        specular = 2,
        normal = 3,
        depth = 4,
        last = 5
    };
}

struct Texture {
    std::string path;
};

struct Material {
    std::array<std::vector<Texture>, TextureType::last> textures;
    std::string name;
};

std::optional<Model> readModel(const std::string &filepath);
std::optional<Material> readMaterial(const std::string &filepath);
std::optional<Animation> readAnimation(const std::string &filepath);

namespace headers {

struct ModelHeader {
    char8_t magicBytes[4] = {'P', 'M', 'D', 'L'};
    uint32_t version = 0;
    uint32_t numOfMeshes;
    uint32_t nameLength;
};

struct MeshSubheader {
    uint32_t vertexType;
    uint32_t vertexCount;
    uint32_t indexType;
    uint32_t indexCount;
    uint32_t boneCount;
    uint32_t materialNameLength;
};

struct BoneSubheader {
    int32_t parentID;
    glm::mat4 offset;
    uint32_t boneNameLength;
};

struct MaterialHeader {
    char8_t magicBytes[4] = {'P', 'M', 'A', 'T'};
    uint32_t version = 0;
    uint32_t textureCount;
    uint32_t nameLength;
};

struct TextureSubheader {
    uint32_t textureType;
    uint32_t pathLength;
};

struct AnimationHeader {
    char8_t magicBytes[4] = {'P', 'A', 'N', 'I'};
    uint32_t version = 0;
    uint32_t boneKeyFramesCount;
    float duration;
    float ticksPerSecond;
    uint32_t nameLength;
};

struct AnimationBoneSubheader {
    uint32_t positionKeyframesCount;
    uint32_t rotationKeyframesCount;
    uint32_t scalingKeyframesCount;
};

struct PositionKeyframe {
    float time;
    glm::vec3 position;
};

struct RotationKeyframe {
    float time;
    glm::quat quaternion;
};

struct ScalingKeyframe {
    float time;
    glm::vec3 scaling;
};

} // namespace headers

size_t vertexTypeToSize(VertexType type);
size_t indexTypeToSize(IndexType type);

} // namespace paca_read
