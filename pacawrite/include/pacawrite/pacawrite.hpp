#pragma once

#include <pacaread/pacaread.hpp>

namespace paca_format {
    void saveMaterial(const Material &material, const std::string &filename);
    void saveModel(const Model &model, const std::string &filename);
    void saveAnimation(const Animation &animation, const std::string &filename);
};
