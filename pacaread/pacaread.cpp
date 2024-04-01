#include "pacaread.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include <uchar.h>
#include <vector>

namespace paca_format {

size_t vertexTypeToSize(VertexType type)
{
    switch (type) {
        case VertexType::float3pos_float3norm_float2texture:            return (3+3+2)*sizeof(float);
        case VertexType::float3pos_float3norm_float3tang_float2texture: return (3+3+3+2)*sizeof(float);
        default: break;
    }

    exit(1);
}

uint32_t vertexTypeToNumOfFloats(VertexType type)
{
    switch (type) {
        case VertexType::float3pos_float3norm_float2texture:            return 3+3+2;
        case VertexType::float3pos_float3norm_float3tang_float2texture: return 3+3+3+2;
        default: break;
    }

    exit(1);
}

size_t indexTypeToSize(IndexType type)
{
    switch (type) {
        case IndexType::no_indices: return 0;
        case IndexType::int32bit:   return sizeof(uint32_t);
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
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        file.read(reinterpret_cast<char*>(&subheader), sizeof(headers::MeshSubheader));
        vertices.resize(subheader.vertexCount * vertexTypeToNumOfFloats(VertexType(subheader.vertexType)));
        indices.resize(subheader.indexCount);
        std::string materialName(subheader.materialNameLength, '\0');
        file.read(materialName.data(), subheader.materialNameLength);
        file.read(reinterpret_cast<char*>(vertices.data()), subheader.vertexCount * vertexTypeToSize(VertexType(subheader.vertexType)));
        file.read(reinterpret_cast<char*>(indices.data()), subheader.indexCount * indexTypeToSize(IndexType(subheader.indexType)));

        meshes.emplace_back(VertexType(subheader.vertexType), IndexType(subheader.indexType), vertices, indices, materialName);
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

} // namespace paca_read
