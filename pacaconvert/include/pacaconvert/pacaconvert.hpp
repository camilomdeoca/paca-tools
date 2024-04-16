#pragma once

#include <pacaread/pacaread.hpp>
#include <optional>
#include <vector>

namespace paca_format {

struct ModelMaterialsAnimations {
    std::optional<paca_format::Model> model;
    std::optional<std::vector<paca_format::Material>> materials;
    std::optional<Animation> animation;
};

ModelMaterialsAnimations modelToPacaFormat(const std::string &path, const std::string &outName);

} // namespace paca_format
