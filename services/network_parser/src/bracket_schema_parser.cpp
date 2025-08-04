#include <network_parser/bracket_schema_parser.h>
#include <network_parser/tokenizer.h>
#include <network_parser/utils.h>
#include <server/logger.h>
#include <network_parser/capnp_generator.h>
#include <network_parser/cpp_header_generator.h>
#include <network_parser/cpp_impl_generator.h>
#include <network_parser/cpp_generator.h>

#include <sstream>
#include <iostream>

using namespace parser;
using namespace curious::log;

void BracketSchemaParser::parseSchema(const std::string &schemaContent) {
    LOG_INFO << "Starting to parse schema..." << go;

    auto tokens = Tokenizer::tokenize(schemaContent);
    LOG_INFO << "Found " << tokens.size() << " tokens" << go;

    parseTokens(tokens);

    LOG_INFO << "Parsing completed. Found " << messages.size() << " messages." << go;
}

void BracketSchemaParser::parseTokens(const std::vector<std::string> &tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string &token = tokens[i];
        LOG_DBG << "Processing token: " << token << go;

        if (token == "message") {
            handleMessageDefinition(i, tokens);
        }
    }
}

void BracketSchemaParser::handleMessageDefinition(size_t &i, const std::vector<std::string> &tokens) {
    if (i + 4 >= tokens.size() || tokens[i + 2] != "(" || tokens[i + 4] != ")") {
        LOG_ERR << "Invalid message syntax at token index " << i << go;
        return;
    }

    std::string name = tokens[i + 1];
    int id = std::stoi(tokens[i + 3]);
    std::string parent;

    i += 4;  // move past ')'

    // optional: "extends ParentName"
    if (i + 2 < tokens.size() && tokens[i + 1] == "extends") {
        parent = tokens[i + 2];
        i += 2;
        LOG_INFO << "Message " << name << " extends " << parent << go;
    }

    if (i + 1 >= tokens.size() || tokens[i + 1] != "{") {
        LOG_ERR << "Expected '{' after message declaration: " << name << go;
        return;
    }

    i++;  // skip '{'

    Message message(name, id, parent);
    i++;  // first field token

    while (i + 2 < tokens.size() && tokens[i] != "}") {
        std::string type = tokens[i];
        std::string fieldName = tokens[i + 1];
        std::string semicolon = tokens[i + 2];

        if (semicolon == ";") {
            message.fields.emplace_back(type, fieldName);
            LOG_DBG << "  Field: " << type << " " << fieldName << go;
            i += 3;
        } else {
            LOG_WARN << "Unexpected token near field declaration: " << tokens[i] << go;
            i++;
        }
    }

    if (tokens[i] == "}") {
        LOG_INFO << "Finished parsing message: " << name << go;
    }

    messages[name] = std::move(message);
}

void BracketSchemaParser::printParsedMessages() const {
    LOG_INFO << "\nParsed messages:" << go;
    for (const auto &[name, msg] : messages) {
        LOG_INFO << "  " << name << " (ID: " << msg.id << ", Parent: " << msg.parent << ")" << go;
        for (const auto &field : msg.fields) {
            LOG_INFO << "    " << field.type << " " << field.name << go;
        }
    }
}

void BracketSchemaParser::generateAllFiles(const std::string &outputDir) {
    CapnpGenerator capnp(messages);
    capnp.generate(outputDir);

    CppHeaderGenerator headers(messages);
    headers.generate(outputDir);

    CppImplGenerator impls(messages);
    impls.generate(outputDir);

    parser::CppGenerator generator(messages);
    generator.generateAll(outputDir);
    // other generators will follow here (headers, C++ impl, factory builder)
}
