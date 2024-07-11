#include "CodeGenerator.hpp"
#include "ASTParser.hpp"
#include "SerializerTemplate.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace CodeGenerator {

constexpr char HEADER_FILE_NAME[] = "Serializer.hpp";
constexpr char CODE_FILE_NAME[] = "Serializer.cpp";

constexpr char HEADER_TEMPLATE_FORWARD_DECLARATION_LINE[] = "// @FORWARD_DECLARATIONS@";
constexpr char HEADER_TEMPLATE_SERIALIZER_DECLARATIONS_LINE[] = "// @SERIALIZER_DECLARATIONS@";
constexpr char HEADER_TEMPLATE_UNSERIALIZER_DECLARATIONS_LINE[] = "// @UNSERIALIZER_DECLARATIONS@";

constexpr char TAB_STRING[] = "    ";

void copy_files(std::filesystem::path outPath)
{
    std::ofstream headerOfs(outPath / HEADER_FILE_NAME);
    headerOfs << SERIALIZER_HEADER_TEMPLATE;
    std::ofstream codeOfs(outPath / CODE_FILE_NAME);
}

void generateHeader(const CXXParser::ExecutionData &data, std::filesystem::path headerFilePath)
{
    std::ofstream ofs(headerFilePath);
    std::istringstream iss(SERIALIZER_HEADER_TEMPLATE);
    if (!ofs)
    {
        std::cout << "Error opening file" << headerFilePath << std::endl;
        return;
    }
    std::cout << "Generating header file: " << headerFilePath << std::endl;

    std::string line;
    while (std::getline(iss, line))
    {
        if (line == HEADER_TEMPLATE_FORWARD_DECLARATION_LINE)
        {
            for (const CXXParser::RecordDefinitionData &recordData : data.records)
            {
                if (recordData.type == CXXParser::RecordDefinitionData::Type::Class)
                    ofs << "class " << recordData.name << ";" << std::endl;
                else if (recordData.type == CXXParser::RecordDefinitionData::Type::Struct)
                    ofs << "struct " << recordData.name << ";" << std::endl;
            }
        }
        else if (line == HEADER_TEMPLATE_SERIALIZER_DECLARATIONS_LINE)
            for (const CXXParser::RecordDefinitionData &recordData : data.records)
                ofs << TAB_STRING << "void operator()(const " << recordData.name << " &object);"
                    << std::endl;
        else if (line == HEADER_TEMPLATE_UNSERIALIZER_DECLARATIONS_LINE)
            for (const CXXParser::RecordDefinitionData &recordData : data.records)
                ofs << TAB_STRING << "void operator()(" << recordData.name << " &object);"
                    << std::endl;
        else
            ofs << line << std::endl;
    }
}

void generate(const CXXParser::ExecutionData &data, std::filesystem::path outPath)
{
    generateHeader(data, outPath / HEADER_FILE_NAME);
    std::filesystem::path codeFilePath = outPath / CODE_FILE_NAME;
    std::ofstream ofs(codeFilePath);
    if (!ofs)
    {
        std::cout << "Error opening file" << codeFilePath << std::endl;
        return;
    }
    std::cout << "Generating code file: " << codeFilePath << std::endl;
    std::unordered_set<std::string> includes;

    // Include Serializer header
    ofs << "#include \"" << HEADER_FILE_NAME << "\"" << std::endl << std::endl;

    // Include Types to serialize headers
    for (const CXXParser::RecordDefinitionData &structData : data.records)
    {
        const std::filesystem::path headerPath(structData.headerPath);
        const std::filesystem::path relativeHeaderPath = std::filesystem::relative(headerPath, outPath);
        const std::string relativeHeaderPathString = relativeHeaderPath.generic_string();
        if (includes.find(relativeHeaderPathString) != includes.end())
            continue;
        includes.emplace(relativeHeaderPathString);
        ofs << "#include \"" << relativeHeaderPathString << "\"" << std::endl;
    }
    ofs << std::endl;

    // This asumes base classes appear first could be fixed with declaration before
    for (const CXXParser::RecordDefinitionData &structData : data.records)
    {
        //void Serializer::operator()(const Type &object)
        //{
        //    (*this)(static_cast<const Base1&>(object));
        //    ...
        //    (*this)(object.field1);
        //    (*this)(object.field2);
        //    ...
        //}
        //
        //void Unserializer::operator()(const Type &object)
        //{
        //    (*this)(static_cast<const Base1&>(object));
        //    ..
        //    (*this)(object.field1);
        //    (*this)(object.field2);
        //    ...
        //}

        ofs << "void Serializer::operator()(const " << structData.name << " &object)" << std::endl
            << "{" << std::endl;
        for (const std::string &baseClass : structData.bases)
        {
            ofs << TAB_STRING << "(*this)(static_cast<const " << baseClass << "&>(object));" << std::endl;
        }
        for (const CXXParser::RecordDefinitionData::FieldData &field : structData.fields)
        {
            ofs << TAB_STRING << "(*this)(object." << field.name << ");" << std::endl;
        }
        ofs << "}" << std::endl << std::endl;

        ofs << "void Unserializer::operator()(" << structData.name << " &object)" << std::endl
            << "{" << std::endl;
        for (const std::string &baseClass : structData.bases)
        {
            ofs << TAB_STRING << "(*this)(static_cast<" << baseClass << "&>(object));" << std::endl;
        }
        for (const CXXParser::RecordDefinitionData::FieldData &field : structData.fields)
        {
            ofs << TAB_STRING << "(*this)(object." << field.name << ");" << std::endl;
        }
        ofs << "}" << std::endl << std::endl;
    }
}

} // namespace CodeGenerator
