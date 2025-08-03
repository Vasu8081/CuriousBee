#pragma once
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <parser/types.h>

namespace parser {

class CppImplGenerator {
private:
    const std::map<std::string, Message>& messages;

    // Helper methods for type checking
    bool isComplexType(const std::string& type) const;
    bool isBuiltinType(const std::string& type) const;
    
    // String manipulation utilities
    std::string capitalize(const std::string& str) const;
    std::string toCamelCase(const std::string& snake_case) const;
    std::string toLowerSnakeCase(const std::string& camelCase) const;
    std::string toAllUpperPreserveUnderscore(const std::string& str) const;
    
    // Field collection
    void collectAllFields(const Message& msg, std::vector<Field>& allFields) const;
    
    // Generation methods
    void generateIncludes(std::ostringstream& out, const Message& msg) const;
    void generateSerializationImplementations(std::ostringstream& out, const Message& msg) const;
    
    void generateToCapnpImpl(std::ostringstream& out, const Message& msg, 
                            const std::string& className, const std::vector<Field>& allFields) const;
    void generateFromCapnpImpl(std::ostringstream& out, const Message& msg, 
                              const std::string& className, const std::vector<Field>& allFields) const;
    void generateSerializeImpl(std::ostringstream& out, const Message& msg, const std::string& className) const;
    void generateDeserializeImpl(std::ostringstream& out, const Message& msg, const std::string& className) const;
    
    void generateComplexFieldSerialization(std::ostringstream& out, const Field& field, 
                                          const std::string& propertyName, const std::string& fieldName) const;
    void generateComplexFieldDeserialization(std::ostringstream& out, const Field& field,
                                            const std::string& propertyName, const std::string& fieldName) const;
    
    // User content preservation
    void preserveUserDefinedContent(const std::string& path, std::ostringstream& out) const;

public:
    explicit CppImplGenerator(const std::map<std::string, Message>& messages);
    void generate(const std::string& outputDir);
};

}  // namespace parser