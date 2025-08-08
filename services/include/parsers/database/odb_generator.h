#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <base/logger.h>
#include <parsers/database/types.h>

namespace parser {

class ODBGenerator {
public:
    explicit ODBGenerator(const std::string& output_directory = "./generated");
    ~ODBGenerator() = default;

    // Main generation functions
    bool generateSchema(const Schema& schema);
    bool generateTable(const Table& table, const DatabaseOptions& db_options = {});
    bool generateAllTables(const std::vector<Table>& tables, const DatabaseOptions& db_options = {});

    // Configuration
    void setOutputDirectory(const std::string& directory);
    void setNamespace(const std::string& ns);
    void setIncludeGuardPrefix(const std::string& prefix);
    void setIndentation(const std::string& indent = "    ");

private:
    std::string output_dir_;
    std::string namespace_;
    std::string include_guard_prefix_;
    std::string indent_;

    // Core generation functions
    std::string generateHeaderFile(const Table& table, const DatabaseOptions& db_options);
    std::string generateSourceFile(const Table& table, const DatabaseOptions& db_options);
    
    // Header generation components
    std::string generateIncludes(const Table& table, const DatabaseOptions& db_options);
    std::string generateClassDeclaration(const Table& table);
    std::string generateMemberVariables(const Table& table);
    std::string generateConstructors(const Table& table);
    std::string generateAccessors(const Table& table);
    std::string generateOperators(const Table& table);
    std::string generateCallbackDeclarations(const Table& table);
    
    // ODB pragma generation
    std::string generateTablePragmas(const Table& table, const DatabaseOptions& db_options);
    std::string generateColumnPragmas(const Column& column, const Table& table);
    std::string generateRelationshipPragmas(const Column& column, const Table& table);
    std::string generateContainerPragmas(const Column& column);
    std::string generateIndexPragmas(const Table& table);
    std::string generateConstraintPragmas(const Table& table);
    std::string generateInheritancePragmas(const Table& table);
    std::string generateCallbackPragmas(const Table& table);
    std::string generateCachePragmas(const Table& table);
    std::string generateSessionPragmas(const Table& table);
    
    // Type conversion and validation
    std::string convertToCppType(const std::string& db_type, const Column& column);
    std::string generateIncludeGuard(const std::string& filename);
    std::string generateNamespaceBegin();
    std::string generateNamespaceEnd();
    std::string sanitizeIdentifier(const std::string& identifier);
    std::string getRelationshipString(RelationshipType type);
    std::string getCascadeString(CascadeType type);
    std::string getCallbackTypeString(CallbackType type);
    std::string getDatabaseTypeString(DatabaseType type);
    
    // Utility functions
    bool writeToFile(const std::string& filename, const std::string& content);
    bool createDirectoryIfNotExists(const std::string& path);
    std::string joinPath(const std::string& base, const std::string& path);
    std::string toUpperCase(const std::string& str);
    std::string toLowerCase(const std::string& str);
    std::string toCamelCase(const std::string& str);
    std::string toPascalCase(const std::string& str);
    std::string toSnakeCase(const std::string& str);
    
    // Validation functions
    bool validateTable(const Table& table);
    bool validateColumn(const Column& column, const Table& table);
    bool validateRelationships(const Table& table);
    bool validateConstraints(const Table& table);
};
}