#include "pacawrite.hpp"
#include "pacaread/pacaread.hpp"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

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
        subheader.vertexCount = mesh.vertices.size()/vertexTypeToSize(VertexType(mesh.vertexType));
        subheader.indexType = static_cast<uint32_t>(mesh.indexType);
        subheader.indexCount = mesh.indices.size();
        subheader.boneCount = mesh.skeleton.bones.size();
        subheader.animationsCount = mesh.animations.size();
        subheader.materialNameLength = mesh.materialName.size();
        outfile.write(reinterpret_cast<const char*>(&subheader), sizeof(subheader));
        outfile.write(mesh.materialName.data(), mesh.materialName.size());

        // Write animation names
        for (const std::string &animName : mesh.animations)
        {
            headers::MeshAnimationSubHeader animNameSubheader;
            animNameSubheader.animationNameLength = animName.size();
            outfile.write(reinterpret_cast<const char*>(&animNameSubheader), sizeof(animNameSubheader));
            outfile.write(animName.data(), animName.size());
        }

        // Write Skeleton
        for (uint32_t boneId = 0; boneId < mesh.skeleton.bones.size(); boneId++)
        {
            const Bone &bone = mesh.skeleton.bones[boneId];
            const std::string &boneName = mesh.skeleton.boneNames[boneId];
            headers::BoneSubheader boneSubheader;
            boneSubheader.parentID = bone.parentID;
            boneSubheader.offset = bone.offsetMatrix;
            boneSubheader.boneNameLength = boneName.size();
            outfile.write(reinterpret_cast<const char*>(&boneSubheader), sizeof(headers::BoneSubheader));
            outfile.write(boneName.data(), boneName.size());
        }

        // Write vertices and indices
        outfile.write(reinterpret_cast<const char*>(mesh.vertices.data()), mesh.vertices.size());
        outfile.write(reinterpret_cast<const char*>(mesh.indices.data()), sizeof(uint32_t) * mesh.indices.size());
    }
}

void saveAnimation(const Animation &animation, const std::string &filename)
{
    std::ofstream outfile(filename, std::ios::binary);

    headers::AnimationHeader header;
    header.duration = animation.duration;
    header.ticksPerSecond = animation.ticksPerSecond;
    header.boneKeyFramesCount = animation.keyframes.size();
    header.nameLength = animation.name.size();
    outfile.write(reinterpret_cast<const char*>(&header), sizeof(header));
    outfile.write(animation.name.data(), animation.name.size());

    // Continue
    for (uint32_t boneId = 0; boneId < animation.keyframes.size(); boneId++)
    {
        headers::AnimationBoneSubheader animationBoneSubheader;
        animationBoneSubheader.positionKeyframesCount = animation.keyframes[boneId].positions.size();
        animationBoneSubheader.rotationKeyframesCount = animation.keyframes[boneId].rotations.size();
        animationBoneSubheader.scalingKeyframesCount = animation.keyframes[boneId].scalings.size();

        outfile.write(reinterpret_cast<const char*>(&animationBoneSubheader), sizeof(animationBoneSubheader));
        for (uint32_t i = 0; i < animation.keyframes[boneId].positions.size(); i++)
        {
            headers::PositionKeyframe positionKeyframe;
            positionKeyframe.time = animation.keyframes[boneId].positions[i].time;
            positionKeyframe.position = animation.keyframes[boneId].positions[i].position;
            outfile.write(reinterpret_cast<const char*>(&positionKeyframe), sizeof(positionKeyframe));
        }
        for (uint32_t i = 0; i < animation.keyframes[boneId].rotations.size(); i++)
        {
            headers::RotationKeyframe rotationKeyframe;
            rotationKeyframe.time = animation.keyframes[boneId].rotations[i].time;
            rotationKeyframe.quaternion = animation.keyframes[boneId].rotations[i].quaternion;
            outfile.write(reinterpret_cast<const char*>(&rotationKeyframe), sizeof(rotationKeyframe));
        }
        for (uint32_t i = 0; i < animation.keyframes[boneId].scalings.size(); i++)
        {
            headers::ScalingKeyframe scalingKeyframe;
            scalingKeyframe.time = animation.keyframes[boneId].scalings[i].time;
            scalingKeyframe.scaling = animation.keyframes[boneId].scalings[i].scale;
            outfile.write(reinterpret_cast<const char*>(&scalingKeyframe), sizeof(scalingKeyframe));
        }
    }
}

} // namespace paca_format

