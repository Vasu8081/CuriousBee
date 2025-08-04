#include <network_parser/tokenizer.h>
#include <network_parser/utils.h>
#include <server/logger.h>

#include <cctype>
#include <algorithm>

using namespace parser;
using namespace curious::log;

std::vector<std::string> Tokenizer::tokenize(const std::string &content) {
    std::vector<std::string> tokens;
    std::string current;

    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];

        if (std::isalpha(c)) {
            current += c;
            while (i + 1 < content.length() &&
                   (std::isalnum(content[i + 1]) || content[i + 1] == '_')) {
                current += content[++i];
            }

            // Generic type like Map<Key,Value>
            if (i + 1 < content.length() && content[i + 1] == '<') {
                current += '<';
                ++i;
                int angleDepth = 1;
                while (i + 1 < content.length() && angleDepth > 0) {
                    ++i;
                    char next = content[i];
                    current += next;
                    if (next == '<') angleDepth++;
                    else if (next == '>') angleDepth--;
                }
            }

            tokens.push_back(utils::trim(current));
            current.clear();
        } else if (c == '{' || c == '}' || c == '(' || c == ')' || c == ';' || c == ',') {
            if (!current.empty()) {
                tokens.push_back(utils::trim(current));
                current.clear();
            }
            tokens.emplace_back(1, c);
        } else if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(utils::trim(current));
                current.clear();
            }
        } else if (c == '#') {
            if (!current.empty()) {
                tokens.push_back(utils::trim(current));
                current.clear();
            }
            while (i < content.length() && content[i] != '\n') {
                ++i;
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        tokens.push_back(utils::trim(current));
    }

    LOG_DBG << "First 20 tokens: ";
    for (size_t i = 0; i < std::min(size_t(20), tokens.size()); ++i) {
        LOG_DBG << "'" << tokens[i] << "' ";
    }
    LOG_DBG << go;

    return tokens;
}
