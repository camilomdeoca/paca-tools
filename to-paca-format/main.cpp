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

    if (modelAndMaterials.model.has_value())
        paca_format::saveModel(modelAndMaterials.model.value(), outName + ".pmdl");

    if (modelAndMaterials.materials.has_value())
        for (unsigned int i = 0; i < modelAndMaterials.materials.value().size(); i++)
        {
            paca_format::saveMaterial(modelAndMaterials.materials.value()[i], outName + ".pmat");
        }

    if (modelAndMaterials.animation.has_value())
        paca_format::saveAnimation(modelAndMaterials.animation.value(), outName + ".pani");
    
    return 0;
}
