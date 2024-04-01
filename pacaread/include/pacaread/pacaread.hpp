#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <array>
#include <vector>

namespace paca_format {

enum class VertexType : uint32_t {
    float3pos_float3norm_float2texture = 1,
    float3pos_float3norm_float3tang_float2texture = 2
};

enum class IndexType : uint32_t {
    no_indices = 0,
    int32bit = 1,
};

struct Mesh {
    VertexType vertexType;
    IndexType indexType;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::string materialName;
};

struct Model {
    std::vector<Mesh> meshes;
    std::string name;
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
    uint32_t materialNameLength;
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

}

size_t vertexTypeToSize(VertexType type);
uint32_t vertexTypeToNumOfFloats(VertexType type);
size_t indexTypeToSize(IndexType type);

} // namespace paca_read
