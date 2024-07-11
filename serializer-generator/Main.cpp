#include "ASTParser.hpp"
#include "CodeGenerator.hpp"

#include <iostream>
#include <vector>

int main(int argc, const char **argv)
{
    CodeGenerator::copy_files(argv[2]);

    if (argc < 4)
        std::cout << "usage: " << argv[0] << " compilation_db out_folder files_of_project..."
                  << std::endl;

    std::vector<std::string> files;
    for (int i = 3; i < argc; i++) files.emplace_back(argv[i]);

    std::cout << "Files: " << std::endl;
    for (const std::string &file : files)
    {
        std::cout << "  " << file << std::endl;
    }
    std::cout << std::endl;
    CXXParser::ExecutionData executionData;
    int error = CXXParser::parse(files, argv[1], executionData);

    CodeGenerator::generate(executionData, argv[2]);

    llvm::errs().flush();
    llvm::outs().flush();

    return error;
}
