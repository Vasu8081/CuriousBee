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

    // Core
    std::string mapTypeToCpp(const std::string& type) const;
    std::string getDefaultForCppType(const std::string& cppType) const;
    std::string getHeaderForType(const std::string& type) const;

    // Modular generators
    std::set<std::string> collectIncludes(const Message& msg) const;
    std::string getClassName(const Message& msg) const;
    std::string getParentName(const Message& msg) const;

    void generateIncludes(std::ostringstream& out, const Message& msg) const;
    void generateClass(std::ostringstream& out, const Message& msg) const;
    void generateProperties(std::ostringstream& out, const Message& msg) const;
    void generateConstructor(std::ostringstream& out, const Message& msg) const;
    void generateAccessors(std::ostringstream& out, const Message& msg) const;
    void generateSerializationFunctions(std::ostringstream& out, const Message& msg) const;
};

}  // namespace parser
