#pragma once

#include <string>
#include <map>
#include <set>
#include <sstream>
#include <parser/types.h>

namespace parser {

class CppHeaderGenerator {
public:
    explicit CppHeaderGenerator(const std::map<std::string, Message>& messages);
    void generate(const std::string& outputDir);

private:
    const std::map<std::string, Message>& messages;

    // Mapping and helper methods
    std::string mapTypeToCpp(const std::string& type) const;
    void getHeaderForType(const std::string& type, std::set<std::string> &headers) const;

    std::set<std::string> collectIncludes(const Message& msg) const;
    void collectAllFieldsRecursively(const Message& msg, std::map<std::string, Field>& fields) const;
    std::string getClassName(const Message& msg) const;
    std::string getParentName(const Message& msg) const;
    std::string getPropertyName(const std::string& name) const;
    std::string getPropertyType(const std::string& type) const;
    std::string getPropertyDefaultValue(const std::string& type, const Message& msg) const;
    std::string getMessageType(const Message& msg) const;

    // Code generation components
    void generateIncludes(std::ostringstream& out, const Message& msg) const;
    void preserveUserDefinedIncludes(const std::string& path, std::ostringstream& out) const;
    void preserveUserDefinedClassContent(const std::string& path, std::ostringstream& out) const;
    void generateEndContent(std::ostringstream& out) const;
    void generateStartContent(std::ostringstream& out, const Message& msg) const;
    void generateProperties(std::ostringstream& out, const Message& msg) const;
    void generateConstructor(std::ostringstream& out, const Message& msg) const;
    void generateAccessors(std::ostringstream& out, const Message& msg) const;
    void generateSerializationFunctions(std::ostringstream& out, const Message& msg) const;
};

}  // namespace parser
