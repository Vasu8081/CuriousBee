#pragma once
#include <string>

namespace parser::utils {

// Trim whitespace from both ends
std::string trim(const std::string &str);

// Convert CamelCase to lower_snake_case
std::string toLowerSnakeCase(const std::string &camelCase);

// Convert snake_case to CamelCase
std::string toCamelCase(const std::string &snake_case);

// Convert to ALL_UPPER_CASE preserving underscores
std::string toAllUpperPreserveUnderscore(const std::string &str);

}  // namespace parser
