#pragma once
#include <string>
#include <map>
#include <vector>
#include <network_parser/types.h>

namespace parser {

class BracketSchemaParser {
public:
    void parseSchema(const std::string &schemaContent);
    void printParsedMessages() const;

    // Generation stubs
    void generateAllFiles(const std::string &outputDir);

private:
    std::map<std::string, Message> messages;

    void parseTokens(const std::vector<std::string> &tokens);
    void handleMessageDefinition(size_t &i, const std::vector<std::string> &tokens);
};

}  // namespace parser
