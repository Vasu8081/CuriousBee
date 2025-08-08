#include <parsers/network/utils.h>
#include <cctype>

namespace parser::utils {

std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string toLowerSnakeCase(const std::string &camelCase) {
    std::string result;
    for (size_t i = 0; i < camelCase.size(); ++i) {
        char c = camelCase[i];
        if (std::isupper(c) && i > 0)
            result += '_';
        result += std::tolower(c);
    }
    return result;
}

std::string toCamelCase(const std::string &snake_case) {
    std::string result;
    bool capitalizeNext = false;
    for (char c : snake_case) {
        if (c == '_') {
            capitalizeNext = true;
        } else if (capitalizeNext) {
            result += std::toupper(c);
            capitalizeNext = false;
        } else {
            result += c;
        }
    }
    return result;
}

std::string toAllUpperPreserveUnderscore(const std::string &str) {
    std::string result;
    for (char c : str) {
        result += (c == '_' ? '_' : std::toupper(c));
    }
    return result;
}

}  // namespace parser
