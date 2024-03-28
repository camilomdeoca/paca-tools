#include "material.hpp"
#include <assimp/material.h>
#include <assimp/types.h>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

uint32_t aiTextureTypeToPMATType(aiTextureType type)
{
    switch (type) {
        case aiTextureType_DIFFUSE: return TextureType::diffuse;
        case aiTextureType_SPECULAR: return TextureType::specular;
        case aiTextureType_AMBIENT:
        case aiTextureType_EMISSIVE:
        case aiTextureType_HEIGHT:
        case aiTextureType_NORMALS:
        case aiTextureType_SHININESS:
        case aiTextureType_OPACITY:
        case aiTextureType_DISPLACEMENT:
        case aiTextureType_LIGHTMAP:
        case aiTextureType_REFLECTION:
        case aiTextureType_BASE_COLOR:
        case aiTextureType_NORMAL_CAMERA:
        case aiTextureType_EMISSION_COLOR:
        case aiTextureType_METALNESS:
        case aiTextureType_DIFFUSE_ROUGHNESS:
        case aiTextureType_AMBIENT_OCCLUSION:
        case aiTextureType_SHEEN:
        case aiTextureType_CLEARCOAT:
        case aiTextureType_TRANSMISSION:
        case aiTextureType_UNKNOWN:
        case _aiTextureType_Force32Bit:
        default: break;
    }

    fprintf(stderr, "Invalid aiTextureType!\n");
    exit(2);
}

void saveMaterial(Material material, const std::string &filename)
{
    std::ofstream outputfile(filename, std::ios::binary);
    MaterialHeader header;
    header.textureCount = material.textures.size();
    header.nameLength = material.name.size();

    outputfile.write(reinterpret_cast<const char*>(&header), sizeof(MaterialHeader));
    outputfile.write(material.name.c_str(), material.name.size());

    for (int textureType = TextureType::none; textureType < TextureType::last; textureType++)
    {
        for (const Texture &texture : material.textures[textureType])
        {
            struct {
                uint32_t textureType;
                uint32_t pathLength;
            } textureHeader;
            textureHeader.textureType = textureType;
            textureHeader.pathLength = texture.path.size();
            outputfile.write(reinterpret_cast<char*>(&textureHeader), sizeof(textureHeader));
            outputfile.write(texture.path.c_str(), texture.path.size());
        }
    }
}
