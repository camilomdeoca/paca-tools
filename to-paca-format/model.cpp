#include "model.hpp"
#include "meshes.hpp"

#include <fstream>

uint32_t vertexTypeToNumOfFloats(VertexType::Type type)
{
    switch (type) {
        case VertexType::float3pos_float3norm_float2texture: return 3+3+2;
        case VertexType::float3pos_float3norm_float3tang_float2texture: return 3+3+3+2;
        default: break;
    }

    exit(1);
}

void saveModel(const std::string &name, const Model &model, const std::string &filename)
{
    std::ofstream outfile(filename, std::ios::binary);
    ModelHeader header;
    header.numOfMeshes = model.meshes.size();
    header.nameLength = name.size();
    
    outfile.write(reinterpret_cast<const char*>(&header), sizeof(header));
    outfile.write(name.c_str(), name.size());

    for (const Mesh &mesh : model.meshes)
    {
        MeshSubHeader subheader;
        subheader.vertexType = mesh.vertexType;
        subheader.vertexCount = mesh.vertices.size()/vertexTypeToNumOfFloats(VertexType::Type(mesh.vertexType));
        subheader.indexType = IndexType::int32bit;
        subheader.indexCount = mesh.indices.size();
        subheader.materialLength = mesh.materialName.size();

        outfile.write(reinterpret_cast<const char*>(&subheader), sizeof(subheader));
        outfile.write(mesh.materialName.data(), mesh.materialName.size());
        outfile.write(reinterpret_cast<const char*>(mesh.vertices.data()), sizeof(float) * mesh.vertices.size());
        outfile.write(reinterpret_cast<const char*>(mesh.indices.data()), sizeof(uint32_t) * mesh.indices.size());
    }
}
