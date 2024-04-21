#include <pacaconvert/pacaconvert.hpp>
#include <pacawrite/pacawrite.hpp>

#include <cstdio>
#include <string>


int main (int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s <input_model> <output_name>\n", argv[0]);
        return 1;
    }

    std::string inputFilepath(argv[1]);
    std::string outName(argv[2]);

    paca_format::ModelMaterialsAnimations modelAndMaterials = paca_format::modelToPacaFormat(inputFilepath, outName);

    printf("Model has %zu materials and %zu animations\n", modelAndMaterials.materials.size(), modelAndMaterials.animations.size());

    if (modelAndMaterials.model.has_value())
        paca_format::saveModel(modelAndMaterials.model.value(), outName + ".pmdl");

    for (unsigned int i = 0; i < modelAndMaterials.materials.size(); i++)
    {
        paca_format::saveMaterial(modelAndMaterials.materials[i], outName + ".pmat");
    }

    for (unsigned int i = 0; i < modelAndMaterials.animations.size(); i++)
    {
        paca_format::saveAnimation(modelAndMaterials.animations[i], outName + ".pani");
    }
    
    return 0;
}
