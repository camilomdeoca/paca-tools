#include "ASTParser.hpp"
#include "CodeGenerator.hpp"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

enum ExitCodes {
    EXIT_CODE_OK = 0,
    EXIT_CODE_NO_FILES,
    EXIT_CODE_BAD_PARAMETERS = 2,
};

struct ParsedArguments {
    std::string serializerOutHeaderFilename = "serializer.h";
    std::string serializerOutCodeFilename = "serializer.cpp";
    std::string compilationDBFolderPath = ".";
    bool showHelp = false;
};

ParsedArguments getArgs(int &argc, const char **&argv)
{
    ParsedArguments result;

    while (++argv)
    {
        argc--;
        if (strcmp(*argv, "--out-header") == 0)
        {
            if (++argv)
            {
                argc--;
                result.serializerOutHeaderFilename = *argv;
            }
            else
            {
                std::cerr << "The --out-header parameter has no value" << std::endl;
                exit(EXIT_CODE_BAD_PARAMETERS);
            }
        }
        else if (strcmp(*argv, "--out-code") == 0)
        {
            if (++argv)
            {
                argc--;
                result.serializerOutCodeFilename = *argv;
            }
            else
            {
                std::cerr << "The --out-code parameter has no value" << std::endl;
                exit(EXIT_CODE_BAD_PARAMETERS);
            }
        }
        else if (strcmp(*argv, "--compilation-db") == 0)
        {
            if (++argv)
            {
                argc--;
                result.compilationDBFolderPath = *argv;
            }
            else
            {
                std::cerr << "The --compilation-db parameter has no value" << std::endl;
                exit(EXIT_CODE_BAD_PARAMETERS);
            }
        }
        else if (strcmp(*argv, "--help") == 0 || strcmp(*argv, "-h") == 0)
        {
            result.showHelp = true;
        }
        else
        {
            return result;
        }
    }

    return result;
}

void printHelp(const char *programName)
{
    std::cout << "Usage: " << programName << " [options] files_classes_to_serialize..." << std::endl
    << "Options:" << std::endl
    //  |1                                                                             |80
    << "  --out-header      The generated header file (`serializer.h` by default)"     << std::endl
    << "  --out-code        The generated code file (`serializer.cpp` by default)"     << std::endl
    << "  --compilation-db  The path of the folder including the compilation database" << std::endl
    << "                    (`.` by default)"                                          << std::endl
    << "  -h, --help        Print the program options" << std::endl;
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        printHelp(argv[0]);
        std::cerr << "Error: No files provided." << std::endl;
        exit(EXIT_CODE_NO_FILES);
    }

    const char **relative_argv = argv;
    int relative_argc = argc;
    ParsedArguments args = getArgs(relative_argc, relative_argv);

    if (args.showHelp)
    {
        printHelp(argv[0]);
        exit(EXIT_CODE_OK);
    }

    std::vector<std::string> files;
    for (int i = 0; i < relative_argc; i++)
    {
        files.emplace_back(relative_argv[i]);
    }

    CodeGenerator::copy_files(args.serializerOutHeaderFilename, args.serializerOutCodeFilename);

    std::cout << "Files: " << std::endl;
    for (const std::string &file : files)
    {
        std::cout << "  " << file << std::endl;
    }
    std::cout << std::endl;
    CXXParser::ExecutionData executionData;
    int error = CXXParser::parse(files, args.compilationDBFolderPath, executionData);

    CodeGenerator::generate(executionData, args.serializerOutHeaderFilename, args.serializerOutCodeFilename);

    llvm::errs().flush();
    llvm::outs().flush();

    return error;
}
