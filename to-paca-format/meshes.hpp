#pragma once

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace VertexType {
    enum Type : uint32_t {
        float3pos_float3norm_float2texture = 1,
        float3pos_float3norm_float3tang_float2texture = 2
    };
}

namespace IndexType {
    enum Type : uint32_t {
        noIndices = 0,
        int32bit = 1,
    };
}

struct MeshSubHeader {
    uint32_t vertexType;
    uint32_t vertexCount;
    uint32_t indexType;
    uint32_t indexCount;
    uint32_t materialLength;
};

struct Mesh {
    uint32_t vertexType;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::string materialName;
};
