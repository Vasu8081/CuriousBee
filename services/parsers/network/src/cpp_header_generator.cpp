#include <parsers/network/cpp_header_generator.h>
#include <parsers/network/utils.h>
#include <base/logger.h>

#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <sstream>

using namespace parser;
using namespace parser::utils;
using namespace curious::log;

CppHeaderGenerator::CppHeaderGenerator(const std::map<std::string, Message>& messages)
    : messages(messages) {}

void CppHeaderGenerator::generate(const std::string& outputDir) {
    std::filesystem::create_directories(outputDir + "/include/network");

    for (const auto& [name, msg] : messages) {
        std::string path = outputDir + "/include/network/" + toLowerSnakeCase(name) + ".h";

        std::ostringstream preservedIncludes;
        preserveUserDefinedIncludes(path, preservedIncludes);

        std::ostringstream preseredClassContent;
        preserveUserDefinedClassContent(path, preseredClassContent);

        std::ostringstream includes;
        generateIncludes(includes, msg);

        std::ostringstream startContent;
        generateStartContent(startContent, msg);

        std::ostringstream endContent;
        generateEndContent(endContent);

        std::ostringstream properties;
        generateProperties(properties, msg);

        std::ostringstream constructor;
        generateConstructor(constructor, msg);

        std::ostringstream accessors;
        generateAccessors(accessors, msg);

        std::ostringstream serialization;
        generateSerializationFunctions(serialization, msg);

        std::ostringstream full;
        full << includes.str();
        full << preservedIncludes.str();
        full << startContent.str();
        full << properties.str();
        full << constructor.str();
        full << accessors.str();
        full << serialization.str();
        full << preseredClassContent.str();
        full << endContent.str();

        std::ofstream file(path);
        if (!file.is_open()) {
            LOG_ERR << "Failed to open file: " << path << go;
            continue;
        }

        file << full.str();
        file.close();
        LOG_INFO << "Generated header: " << path << go;
    }
}

void CppHeaderGenerator::generateStartContent(std::ostringstream& out, const Message& msg) const {
    out << "\nnamespace curious::net {\n\n";
    out << "class " << getClassName(msg);
    if (!getParentName(msg).empty()) {
        out << " : public " << getParentName(msg);
    }
    out << " {\n";
}

void CppHeaderGenerator::generateEndContent(std::ostringstream& out) const {
    out << "};";
    out << "\n}  // namespace curious::net\n";
}

std::string CppHeaderGenerator::mapTypeToCpp(const std::string& type) const {
    static const std::unordered_map<std::string, std::string> builtin = {
        {"int8", "int8_t"}, {"int16", "int16_t"}, {"int32", "int32_t"}, {"int64", "int64_t"},
        {"uint8", "uint8_t"}, {"uint16", "uint16_t"}, {"uint32", "uint32_t"}, {"uint64", "uint64_t"},
        {"float", "float"}, {"float32", "float"}, {"float64", "double"},
        {"bool", "bool"}, {"string", "std::string"}, {"bytes", "std::vector<uint8_t>"},
        {"Data", "std::vector<uint8_t>"}, {"Text", "std::string"}, {"any", "capnp::AnyPointer"},
        {"void", "void"}
    };

    if (builtin.count(type)) return builtin.at(type);

    if (type.starts_with("list<") && type.ends_with(">")) {
        std::string inner = type.substr(5, type.length() - 6);
        return "std::vector<" + mapTypeToCpp(inner) + ">";
    }

    if (type.starts_with("map<") && type.ends_with(">")) {
        std::string inner = type.substr(4, type.length() - 5);
        size_t comma = inner.find(',');
        std::string key = trim(inner.substr(0, comma));
        std::string val = trim(inner.substr(comma + 1));
        return "std::map<" + mapTypeToCpp(key) + ", " + mapTypeToCpp(val) + ">";
    }

    return toLowerSnakeCase(type); // Assume user-defined type
}

void CppHeaderGenerator::getHeaderForType(const std::string& type, std::set<std::string> &headers) const {
    if (type.empty()) return;
    if (type == "int8_t" || type == "int16_t" || type == "int32_t" || type == "int64_t" || type == "int" ||
        type == "uint8_t" || type == "uint16_t" || type == "uint32_t" || type == "uint64_t" || type == "uint") {
        headers.insert("cstdint");
        return;
    }
    if (type == "float" || type == "double") {
        headers.insert("cfloat");
        return;
    }
    if (type == "bool") {
        headers.insert("cstdbool");
        return;
    }
    if (type == "void") {
        headers.insert("cstddef");
        return;
    }
    if (type == "string") {
        headers.insert("string");
        return;
    }
    if (type.starts_with("list<") && type.ends_with(">")) {
        std::string inner = type.substr(5, type.length() - 6);
        getHeaderForType(inner, headers);
        headers.insert("vector");
        return;
    }
    if (type.starts_with("map<") && type.ends_with(">")) {
        std::string inner = type.substr(4, type.length() - 5);
        size_t comma = inner.find(',');
        std::string key = trim(inner.substr(0, comma));
        std::string val = trim(inner.substr(comma + 1));
        getHeaderForType(key, headers);
        getHeaderForType(val, headers);
        headers.insert("map");
        return;
    }
    headers.insert("network/" + toLowerSnakeCase(type) + ".h");
}

std::string CppHeaderGenerator::getClassName(const Message& msg) const {
    return toLowerSnakeCase(msg.name);
}

std::string CppHeaderGenerator::getParentName(const Message& msg) const {
    return msg.parent.empty() ? "" : toLowerSnakeCase(msg.parent);
}

std::set<std::string> CppHeaderGenerator::collectIncludes(const Message& msg) const {
    std::set<std::string> includes;
    if (!msg.parent.empty()) {
        includes.insert("network/" + toLowerSnakeCase(msg.parent) + ".h");
    }

    for (const auto& f : msg.fields) {
        getHeaderForType(f.type, includes);
    }

    return includes;
}

void CppHeaderGenerator::preserveUserDefinedIncludes(const std::string& path, std::ostringstream& out) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERR << "Cannot open file for reading: " << path << go;
        return;
    }

    std::string line;
    bool inside = false;
    bool addedStart = false, addedEnd = false;
    std::ostringstream temp;

    while (std::getline(file, line)) {
        if (line.find("//#editable_headers_start_dont_remove_this_line_only_write_below") != std::string::npos) {
            inside = true;
            addedStart = true;
            temp << line << "\n";
            continue;
        }
        if (line.find("//#editable_headers_end_dont_remove_this_line_only_write_above") != std::string::npos) {
            temp << line << "\n";
            addedEnd = true;
            inside = false;
            break;  // done
        }
        if (inside) {
            temp << line << "\n";
        }
    }

    if (!addedStart) {
        temp << "//#editable_headers_start_dont_remove_this_line_only_write_below\n\n";
    }
    if (!addedEnd) {
        temp << "//#editable_headers_end_dont_remove_this_line_only_write_above\n";
    }

    out << temp.str();
}

void CppHeaderGenerator::preserveUserDefinedClassContent(const std::string& path, std::ostringstream& out) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERR << "Cannot open file for reading: " << path << go;
        return;
    }

    std::string line;
    bool inside = false;
    bool addedStart = false, addedEnd = false;
    std::ostringstream temp;

    while (std::getline(file, line)) {
        if (line.find("//#editable_class_start_dont_remove_this_line_only_write_above") != std::string::npos) {
            inside = true;
            addedStart = true;
            temp << line << "\n";
            continue;
        }
        if (line.find("//#editable_class_end_dont_remove_this_line_only_write_below") != std::string::npos) {
            temp << line << "\n";
            addedEnd = true;
            inside = false;
            break;
        }
        if (inside) {
            temp << line << "\n";
        }
    }

    if (!addedStart) {
        temp << "//#editable_class_start_dont_remove_this_line_only_write_above\n\n";
    }
    if (!addedEnd) {
        temp << "//#editable_class_end_dont_remove_this_line_only_write_below\n";
    }

    out << temp.str();
}


void CppHeaderGenerator::generateIncludes(std::ostringstream& out, const Message& msg) const {
    out << "#pragma once\n";
    out << "#include <messages/network_msg.capnp.h>\n";
    out << "#include <capnp/message.h>\n";
    out << "#include <capnp/serialize.h>\n";
    out << "#include <string>\n";
    out << "#include <cstdint>\n";

    for (const auto& h : collectIncludes(msg)) {
        out << "#include <" << h << ">\n";
    }
}

std::string CppHeaderGenerator::getPropertyName(const std::string& name) const {
    std::string property = "_" + toCamelCase(toLowerSnakeCase(name));
    return property;
}

std::string CppHeaderGenerator::getPropertyType(const std::string& type) const {
    return mapTypeToCpp(type);
}

void CppHeaderGenerator::generateProperties(std::ostringstream& out, const Message& msg) const {
    out << "protected:\n";
    out << "  // Properties\n";
    for (const auto& f : msg.fields) {
        out << "  " << getPropertyType(f.type) << " " << getPropertyName(f.name) << ";\n";
    }
}

void CppHeaderGenerator::collectAllFieldsRecursively(const Message& msg, std::map<std::string, Field>& fields) const {
    for (const auto& f : msg.fields) {
        fields[f.name] = Field(f.type, f.name);
    }
    if (!msg.parent.empty()) {
        auto parentIt = messages.find(msg.parent);
        if (parentIt != messages.end()) {
            collectAllFieldsRecursively(parentIt->second, fields);
        } else {
            LOG_WARN << "Parent message " << msg.parent << " not found for message " << msg.name << go;
        }
    }
}

std::string CppHeaderGenerator::getMessageType(const Message& msg) const {
    auto name = toCamelCase(msg.name);
    if (name.empty()) {
        LOG_ERR << "Message name is empty for message with ID: " << msg.id << go;
        return "message_type::Unknown";
    }
    name[0] = std::tolower(name[0]);
    return "message_type::" + name;
}

std::string CppHeaderGenerator::getPropertyDefaultValue(const std::string& type, const Message& msg) const {
    std::string cppType = mapTypeToCpp(type);
    if (cppType == "std::string") return "\"\"";
    if (cppType == "bool") return "false";
    if (cppType == "float") return "0.0f";
    if (cppType == "double") return "0.0";
    if (cppType.find("int") != std::string::npos || cppType.find("uint") != std::string::npos) return "0";
    if (cppType.starts_with("std::vector<") || cppType.starts_with("std::map<")) {
        return "{}"; // Default for containers
    }
    if (cppType == "message_type") {
        return getMessageType(msg); // Default for message types
    }
    return ""; // Default for pointers or unknown types
}

void CppHeaderGenerator::generateConstructor(std::ostringstream& out, const Message& msg) const {
    std::string className = getClassName(msg);
    out << "public:\n";
    out << "  // Constructor\n";
    out << "  " << className << "() {\n";
    auto allFields = std::map<std::string, Field>();
    collectAllFieldsRecursively(msg, allFields);
    for (const auto& [name, f] : allFields) {
        auto defaultValue = getPropertyDefaultValue(f.type, msg);
        if (defaultValue.empty()) {
            LOG_WARN << "No default value for field " << f.name << " in message " << msg.name << go;
            continue;
        }
        out << "    " << getPropertyName(f.name) << " = " << defaultValue << ";\n";
    }
    out << "  }\n\n";
}

void CppHeaderGenerator::generateAccessors(std::ostringstream& out, const Message& msg) const {
    for (const auto& f : msg.fields) {
        std::string cppType = mapTypeToCpp(f.type);
        std::string field = toCamelCase(f.name);
        field[0] = std::toupper(field[0]);
        std::string var = "_" + toCamelCase(toLowerSnakeCase(f.name));
        out << "  " << cppType << " get" << field << "() const { return " << var << "; }\n";
        out << "  void set" << field << "(" << cppType << " value) { " << var << " = value; }\n\n";
    }
}

void CppHeaderGenerator::generateSerializationFunctions(std::ostringstream& out, const Message& msg) const {
    std::string className = getClassName(msg);
    out << "  void toCapnp(curious::message::" << msg.name << "::Builder& builder) const;\n";
    out << "  static " << className << " fromCapnp(const curious::message::" << msg.name << "::Reader& reader);\n";
    out << "  std::string serialize() const;\n";
    out << "  static " << className << " deserialize(const std::string& data);\n";
}
