#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <array>
#include <vector>

namespace paca_read {

namespace vertex_type {
    enum type : uint32_t {
        float3pos_float3norm_float2texture = 1,
        float3pos_float3norm_float3tang_float2texture = 2
    };
}

namespace index_type {
    enum type : uint32_t {
        no_indices = 0,
        int32bit = 1,
    };
}

struct mesh {
    uint32_t vertex_type;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::string material_name;
};

struct model {
    std::vector<mesh> meshes;
};

namespace texture_type {
    enum type : uint32_t {
        none = 0,
        diffuse = 1,
        specular = 2,
        normal = 3,
        depth = 4,
        last = 5
    };
}

struct texture {
    std::string path;
};

struct material {
    std::array<std::vector<texture>, texture_type::last> textures;
    std::string name;
};

std::optional<model> read_model(const std::string &filepath);
std::optional<material> read_material(const std::string &filepath);

} // namespace paca_read
