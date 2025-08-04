#pragma once

#include <string>
#include <map>
#include <vector>
#include <network_parser/types.h>

namespace parser {

class CapnpGenerator {
public:
    CapnpGenerator(const std::map<std::string, Message>& messages);
    void generate(const std::string& outputDir);

private:
    const std::map<std::string, Message>& messages;

    void writeEnum(std::ostream& out);
    void writeStructs(std::ostream& out);
    void writeCustomTypes(std::ostream& out);
    std::vector<std::string> determineDependencyOrder() const;
    std::string mapTypeToCapnp(const std::string& type) const;
};

}  // namespace parser
