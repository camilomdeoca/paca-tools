#include "pacawrite.hpp"

#include <fstream>

namespace paca_format {

void saveMaterial(const Material &material, const std::string &filename)
{
    std::ofstream outputfile(filename, std::ios::binary);
    headers::MaterialHeader header;

    uint32_t textureCount = 0;
    for (const std::vector<Texture> &texturesOfType : material.textures)
        textureCount += texturesOfType.size();

    header.textureCount = textureCount;
    header.nameLength = material.name.size();

    outputfile.write(reinterpret_cast<const char*>(&header), sizeof(headers::MaterialHeader));
    outputfile.write(material.name.c_str(), material.name.size());

    for (uint32_t textureType = TextureType::none; textureType < TextureType::last; textureType++)
    {
        for (const Texture &texture : material.textures[textureType])
        {
            headers::TextureSubheader textureHeader;
            textureHeader.textureType = textureType;
            textureHeader.pathLength = texture.path.size();
            outputfile.write(reinterpret_cast<const char*>(&textureHeader), sizeof(textureHeader));
            outputfile.write(texture.path.c_str(), texture.path.size());
        }
    }
}

void saveModel(const Model &model, const std::string &filename)
{
    std::ofstream outfile(filename, std::ios::binary);
    headers::ModelHeader header;
    header.numOfMeshes = model.meshes.size();
    header.nameLength = model.name.size();
    
    outfile.write(reinterpret_cast<const char*>(&header), sizeof(header));
    outfile.write(model.name.c_str(), model.name.size());

    for (const Mesh &mesh : model.meshes)
    {
        // Write header
        headers::MeshSubheader subheader;
        subheader.vertexType = static_cast<uint32_t>(mesh.vertexType);
        subheader.vertexCount = mesh.vertices.size()/vertexTypeToNumOfFloats(VertexType(mesh.vertexType));
        subheader.indexType = static_cast<uint32_t>(mesh.indexType);
        subheader.indexCount = mesh.indices.size();
        subheader.materialNameLength = mesh.materialName.size();
        outfile.write(reinterpret_cast<const char*>(&subheader), sizeof(subheader));
        outfile.write(mesh.materialName.data(), mesh.materialName.size());

        // Write vertices and indices
        outfile.write(reinterpret_cast<const char*>(mesh.vertices.data()), sizeof(float) * mesh.vertices.size());
        outfile.write(reinterpret_cast<const char*>(mesh.indices.data()), sizeof(uint32_t) * mesh.indices.size());
    }
}

} // namespace paca_format

