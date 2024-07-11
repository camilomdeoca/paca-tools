#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ASTParser.hpp"

#include <filesystem>

namespace CodeGenerator {

void copy_files(std::filesystem::path outPath);
void generate(const CXXParser::ExecutionData &data, std::filesystem::path outPath);

} // namespace CodeGenerator

#endif // CODE_GENERATOR_HPP
