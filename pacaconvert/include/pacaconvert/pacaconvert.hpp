#pragma once

#include <pacaread/pacaread.hpp>
#include <optional>
#include <vector>

namespace paca_format {

struct ModelMaterialsAnimations {
    std::optional<Model> model;
    std::vector<Material> materials;
    std::vector<Animation> animation;
};

ModelMaterialsAnimations modelToPacaFormat(const std::string &path, const std::string &outName);

} // namespace paca_format
