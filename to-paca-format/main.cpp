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

    std::optional<paca_format::ModelAndMaterials> modelAndMaterials = paca_format::modelToPacaFormat(inputFilepath, outName);

    if (!modelAndMaterials.has_value())
        return 1; 

    paca_format::saveModel(modelAndMaterials.value().model, outName + ".pmdl");

    for (unsigned int i = 0; i < modelAndMaterials.value().materials.size(); i++)
    {
        paca_format::saveMaterial(modelAndMaterials.value().materials[i], outName + ".pmat");
    }
    
    return 0;
}
