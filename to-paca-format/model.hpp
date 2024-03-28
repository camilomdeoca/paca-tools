#pragma once

#include "meshes.hpp"

#include <string>
#include <uchar.h>
#include <vector>

struct ModelHeader {
    char8_t magicBytes[4] = {'P', 'M', 'D', 'L'};
    uint32_t version = 0;
    uint32_t numOfMeshes;
    uint32_t nameLength;
};

struct Model {
    std::vector<Mesh> meshes;
};

void saveModel(const std::string &name, const std::vector<Mesh> &meshes, const std::string &filename);
