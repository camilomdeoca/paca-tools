#pragma once

#include <assimp/material.h>
#include <cstdint>
#include <string>
#include <array>
#include <vector>

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

struct MaterialHeader {
    char8_t magicBytes[4] = {'P', 'M', 'A', 'T'};
    uint32_t version = 0;
    uint32_t textureCount;
    uint32_t nameLength;
};

struct Material {
    std::array<std::vector<Texture>, TextureType::Type::last> textures;
    //std::string name;
};

void saveMaterial(const std::string &name, Material material, const std::string &filename);
