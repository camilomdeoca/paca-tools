#include "model.hpp"
#include "meshes.hpp"

#include <fstream>

void saveModel(const std::string &name, const std::vector<Mesh> &meshes, const std::string &filename)
{
    std::ofstream outfile(filename, std::ios::binary);
    ModelHeader header;
    header.numOfMeshes = meshes.size();
    header.nameLength = name.size();
    
    outfile.write(reinterpret_cast<const char*>(&header), sizeof(header));
    outfile.write(name.c_str(), name.size());

    for (const Mesh &mesh : meshes)
    {
        MeshSubHeader subheader;
        subheader.vertexType = mesh.vertexType;
        subheader.vertexCount = mesh.vertices.size();
        subheader.indexType = IndexType::int32bit;
        subheader.indexCount = mesh.indices.size();
        subheader.materialLength = mesh.materialName.size();

        outfile.write(reinterpret_cast<const char*>(&subheader), sizeof(subheader));
        outfile.write(mesh.materialName.data(), mesh.materialName.size());
        outfile.write(reinterpret_cast<const char*>(mesh.vertices.data()), sizeof(float) * mesh.vertices.size());
        outfile.write(reinterpret_cast<const char*>(mesh.indices.data()), sizeof(uint32_t) * mesh.indices.size());
    }
}
