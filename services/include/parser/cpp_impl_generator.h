#pragma once
#include <string>
#include <map>
#include <parser/types.h>

namespace parser {

class CppImplGenerator {
public:
    CppImplGenerator(const std::map<std::string, Message>& messages);
    void generate(const std::string& outputDir);

private:
    const std::map<std::string, Message>& messages;

    std::string mapTypeToCpp(const std::string& type) const;
    void writeReadonlyCppFile(const Message& msg, const std::string& path);
    void createEditableStubIfNeeded(const Message& msg, const std::string& dir);
};

}  // namespace parser
