#pragma once

#include <pacaread/pacaread.hpp>
#include <optional>
#include <vector>

namespace paca_format {

struct ModelAndMaterials {
    paca_format::Model model;
    std::vector<paca_format::Material> materials;
};

std::optional<ModelAndMaterials> modelToPacaFormat(const std::string &path, const std::string &outName);

} // namespace paca_format
