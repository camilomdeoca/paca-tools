#include "pacaread.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include <uchar.h>
#include <vector>

namespace paca_read {

struct model_header {
    char8_t magic_bytes[4];
    uint32_t version;
    uint32_t num_of_meshes;
    uint32_t name_length;
};

struct mesh_subheader {
    uint32_t vertex_type;
    uint32_t vertex_count;
    uint32_t index_type;
    uint32_t index_count;
    uint32_t material_name_length;
};

struct material_header {
    char8_t magic_bytes[4] = {'P', 'M', 'A', 'T'};
    uint32_t version = 0;
    uint32_t texture_count;
    uint32_t name_length;
};

struct texture_subheader {
    uint32_t texture_type;
    uint32_t path_length;
};

size_t vertex_type_to_size(vertex_type::type type)
{
    switch (type) {
        case vertex_type::float3pos_float3norm_float2texture:            return (3+3+2)*sizeof(float);
        case vertex_type::float3pos_float3norm_float3tang_float2texture: return (3+3+3+2)*sizeof(float);
        default: break;
    }

    exit(1);
}

uint32_t vertex_type_to_num_of_floats(vertex_type::type type)
{
    switch (type) {
        case vertex_type::float3pos_float3norm_float2texture:            return 3+3+2;
        case vertex_type::float3pos_float3norm_float3tang_float2texture: return 3+3+3+2;
        default: break;
    }

    exit(1);
}

size_t index_type_to_size(index_type::type type)
{
    switch (type) {
        case index_type::no_indices: return 0;
        case index_type::int32bit:   return sizeof(uint32_t);
        default: break;
    }

    exit(1);
}

std::optional<model> read_model(const std::string &filepath)
{
    std::vector<mesh> meshes;
    std::ifstream file(filepath, std::ios::binary);

    if (file.fail()) {
        // Failed to open file
        return {};
    }

    model_header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // check magic bytes
    char8_t correct_magic_bytes[4] = {'P', 'M', 'D', 'L'};
    for (unsigned int i = 0; i < sizeof(header.magic_bytes); i++)
    {
        if (header.magic_bytes[i] != correct_magic_bytes[i])
            return {};
    }

    std::string model_name(header.name_length, '\0');
    file.read(model_name.data(), header.name_length);

    for (uint32_t i = 0; i < header.num_of_meshes; i++)
    {
        mesh_subheader subheader;
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        file.read(reinterpret_cast<char*>(&subheader), sizeof(mesh_subheader));
        vertices.resize(subheader.vertex_count * vertex_type_to_num_of_floats(vertex_type::type(subheader.vertex_type)));
        indices.resize(subheader.index_count);
        std::string material_name(subheader.material_name_length, '\0');
        file.read(material_name.data(), subheader.material_name_length);
        file.read(reinterpret_cast<char*>(vertices.data()), subheader.vertex_count * vertex_type_to_size(vertex_type::type(subheader.vertex_type)));
        file.read(reinterpret_cast<char*>(indices.data()), subheader.index_count * index_type_to_size(index_type::type(subheader.index_type)));

        meshes.emplace_back(subheader.vertex_type, vertices, indices, material_name);
    }

    return model{meshes, model_name};
}

std::optional<material> read_material(const std::string &filepath)
{
    std::array<std::vector<texture>, texture_type::last> textures;
    std::ifstream file(filepath, std::ios::binary);

    if (file.fail()) {
        // Failed to open file
        return {};
    }

    material_header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // check magic bytes
    char8_t correct_magic_bytes[4] = {'P', 'M', 'A', 'T'};
    for (unsigned int i = 0; i < sizeof(header.magic_bytes); i++)
    {
        if (header.magic_bytes[i] != correct_magic_bytes[i])
            return {};
    }

    std::string material_name(header.name_length, '\0');
    file.read(material_name.data(), header.name_length);

    for (uint32_t i = 0; i < header.texture_count; i++)
    {
        texture_subheader subheader;
        file.read(reinterpret_cast<char*>(&subheader), sizeof(subheader));
        std::string texture_path(subheader.path_length, '\0');
        file.read(texture_path.data(), texture_path.size());
        textures[subheader.texture_type].emplace_back(texture_path);
    }

    return material{textures, material_name};
}

} // namespace paca_read
