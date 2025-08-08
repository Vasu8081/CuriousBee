#include <parsers/network/capnp_generator.h>
#include <parsers/network/utils.h>
#include <base/logger.h>

#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <sstream>

using namespace parser;
using namespace curious::log;

CapnpGenerator::CapnpGenerator(const std::map<std::string, Message>& messages)
    : messages(messages) {}

void CapnpGenerator::generate(const std::string& outputDir) {
    std::filesystem::create_directories(outputDir + "/messages/src");
    std::ofstream file(outputDir + "/messages/src/network_msg.capnp");
    if (!file.is_open()) {
        LOG_ERR << "Failed to open output file for Cap'n Proto: " << outputDir << go;
        return;
    }

    file << "@0xecd3d69fdf10c2c3;\n";
    file << "using Cxx = import \"/capnp/c++.capnp\";\n";
    file << "$Cxx.namespace(\"curious::message\");\n\n";

    writeEnum(file);
    writeCustomTypes(file);
    writeStructs(file);

    file.close();
    LOG_INFO << "Generated: " << outputDir << "/messages/src/network_msg.capnp" << go;
}

void CapnpGenerator::writeEnum(std::ostream& out) {
    out << "enum MessageType {\n";
    out << "  unknown @0;\n";
    for (const auto& [name, msg] : messages) {
        std::string enumName = utils::toCamelCase(name);
        enumName[0] = std::tolower(enumName[0]);
        out << "  " << enumName << " @" << msg.id << ";\n";
    }
    out << "}\n\n";
}

void CapnpGenerator::writeCustomTypes(std::ostream& out) {
    out << "struct Map(Key, Value) {\n";
    out << "  entries @0 :List(Entry);\n";
    out << "  struct Entry {\n";
    out << "    key @0 :Key;\n";
    out << "    value @1 :Value;\n";
    out << "  }\n";
    out << "}\n\n";
}

std::vector<std::string> CapnpGenerator::determineDependencyOrder() const {
    std::vector<std::string> ordered;
    std::unordered_set<std::string> added;

    bool addedAny = true;
    while (addedAny) {
        addedAny = false;
        for (const auto& [name, msg] : messages) {
            if (added.count(name)) continue;

            if (msg.parent.empty() || added.count(msg.parent)) {
                ordered.push_back(name);
                added.insert(name);
                addedAny = true;
            }
        }
    }

    return ordered;
}

void CapnpGenerator::writeStructs(std::ostream& out) {
    auto order = determineDependencyOrder();

    for (const auto& name : order) {
        const Message& msg = messages.at(name);

        out << "struct " << name << " {\n";

        std::vector<Field> fields;

        // include parent fields
        std::function<void(const std::string&)> collectFields;
        collectFields = [&](const std::string& parentName) {
            if (messages.count(parentName)) {
                const auto& parent = messages.at(parentName);
                if (!parent.parent.empty()) {
                    collectFields(parent.parent);
                }
                fields.insert(fields.end(), parent.fields.begin(), parent.fields.end());
            }
        };

        if (!msg.parent.empty()) {
            collectFields(msg.parent);
        }

        fields.insert(fields.end(), msg.fields.begin(), msg.fields.end());

        for (size_t i = 0; i < fields.size(); ++i) {
            const auto& f = fields[i];
            out << "  " << utils::toCamelCase(f.name)
                << " @" << i << " : " << mapTypeToCapnp(f.type) << ";\n";
        }

        out << "}\n\n";
    }
}

std::string CapnpGenerator::mapTypeToCapnp(const std::string& type) const {
    static const std::map<std::string, std::string> primitiveMap = {
        {"int8", "Int8"}, {"int16", "Int16"}, {"int32", "Int32"}, {"int64", "Int64"}, {"int", "Int32"},
        {"uint8", "UInt8"}, {"uint16", "UInt16"}, {"uint32", "UInt32"}, {"uint64", "UInt64"}, {"uint", "UInt32"},
        {"float", "Float32"}, {"float32", "Float32"}, {"float64", "Float64"},
        {"bool", "Bool"}, {"string", "Text"}, {"bytes", "Data"}, {"void", "Void"}, {"any", "AnyPointer"}
    };

    auto it = primitiveMap.find(type);
    if (it != primitiveMap.end()) return it->second;

    if (type.starts_with("list<") && type.ends_with(">")) {
        std::string inner = type.substr(5, type.length() - 6);
        return "List(" + mapTypeToCapnp(inner) + ")";
    }

    if (type.starts_with("map<") && type.ends_with(">")) {
        size_t comma = type.find(',');
        std::string key = type.substr(4, comma - 4);
        std::string value = type.substr(comma + 1, type.length() - comma - 2);
        return "Map(" + mapTypeToCapnp(utils::trim(key)) + ", " + mapTypeToCapnp(utils::trim(value)) + ")";
    }

    return type;  // assume custom message
}
