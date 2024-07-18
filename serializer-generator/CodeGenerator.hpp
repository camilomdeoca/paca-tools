#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ASTParser.hpp"

#include <filesystem>

namespace CodeGenerator {

void copy_files(std::filesystem::path outHeaderFilepath, std::filesystem::path outCodeFilepath);
void generate(const CXXParser::ExecutionData &data, std::filesystem::path outHeaderFilepath, std::filesystem::path outCodeFilepath);

} // namespace CodeGenerator

#endif // CODE_GENERATOR_HPP
