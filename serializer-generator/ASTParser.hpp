#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclGroup.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/Signals.h>

#include <memory>
#include <unordered_set>

namespace CXXParser {

struct RecordDefinitionData {
    struct FieldData {
        std::string typeName;
        std::string name;
        unsigned int length;
    };

    enum class Type {
        Struct,
        Class
    };

    Type type;
    std::string name;
    std::string headerPath;
    std::vector<std::string> bases;
    std::vector<FieldData> fields;
};

struct ExecutionData {
    std::vector<RecordDefinitionData> records;
};

class MyASTConsumer : public clang::ASTConsumer
{
public:
    MyASTConsumer(ExecutionData &executionData, std::unordered_set<std::string> &alreadyParsedStructsNames)
        : m_executionData(executionData), m_alreadyParsedStructsNames(alreadyParsedStructsNames)
    {}

    virtual bool HandleTopLevelDecl(clang::DeclGroupRef DG) override;

private:
    void HandleChildDeclarations(clang::DeclContext *declContext);
    bool HandleDecl(clang::DeclGroupRef DG);
    void HandleSingleDecl(const clang::CXXRecordDecl &pRecord);
    bool IsRecordFlaggedToSerialize(const clang::CXXRecordDecl &pRecord);

    ExecutionData &m_executionData;
    std::unordered_set<std::string> &m_alreadyParsedStructsNames;
};

class MyAction : public clang::SyntaxOnlyAction {
public:
    MyAction(ExecutionData &structDefinitionData, std::unordered_set<std::string> &alreadyParsedStructsNames)
        : m_executionData(structDefinitionData), m_alreadyParsedStructsNames(alreadyParsedStructsNames)
    {}

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &CI, llvm::StringRef InFile) override;
    bool BeginSourceFileAction(clang::CompilerInstance &CI) override;

private:
    ExecutionData &m_executionData;
    std::unordered_set<std::string> &m_alreadyParsedStructsNames;
};

class MyFactory : public clang::tooling::FrontendActionFactory
{
public:
    MyFactory(ExecutionData &structDefinitionData)
        : m_executionData(structDefinitionData)
    {}

    std::unique_ptr<clang::FrontendAction> create() override;

private:
    ExecutionData &m_executionData;
    std::unordered_set<std::string> m_alreadyParsedStructsNames;
};

int parse(std::vector<std::string> files, std::string compilationDbPath, ExecutionData &executionData);

} // namespace CXXParser

