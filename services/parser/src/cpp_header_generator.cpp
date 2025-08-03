#include <parser/cpp_header_generator.h>
#include <parser/utils.h>
#include <server/logger.h>

#include <fstream>
#include <unordered_map>
#include <filesystem>

using namespace parser;
using namespace parser::utils;
using namespace curious::log;

CppHeaderGenerator::CppHeaderGenerator(const std::map<std::string, Message>& messages)
    : messages(messages) {}

void CppHeaderGenerator::generate(const std::string& outputDir) {
    std::filesystem::create_directories(outputDir + "/include/network");

    for (const auto& [name, msg] : messages) {
        std::ostringstream out;
        generateIncludes(out, msg);
        generateClass(out, msg);

        std::string filePath = outputDir + "/include/network/" + toLowerSnakeCase(name) + ".h";
        std::ofstream file(filePath);
        if (!file.is_open()) {
            LOG_ERR << "Failed to open file: " << filePath << go;
            continue;
        }

        file << out.str();
        file.close();
        LOG_INFO << "Generated header: " << filePath << go;
    }
}

std::set<std::string> CppHeaderGenerator::collectIncludes(const Message& msg) const {
    std::set<std::string> headers;

    if (!msg.parent.empty())
        headers.insert("network/" + toLowerSnakeCase(msg.parent) + ".h");

    for (const auto& f : msg.fields) {
        std::string h = getHeaderForType(f.type);
        if (!h.empty()) headers.insert(h);
    }

    return headers;
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

    out << "\nnamespace curious::net {\n\n";
}

void CppHeaderGenerator::generateClass(std::ostringstream& out, const Message& msg) const {
    std::string className = getClassName(msg);
    std::string parent = getParentName(msg);

    out << "class " << className;
    if (!parent.empty()) out << " : public " << parent;
    out << " {\n";

    out << "  // #generated start\n";
    out << "protected:\n";
    generateProperties(out, msg);

    out << "\npublic:\n";
    generateConstructor(out, msg);
    generateAccessors(out, msg);
    generateSerializationFunctions(out, msg);

    out << "  // #generated end\n";
    out << "};\n\n";
    out << "}  // namespace curious::net\n";
}


std::string CppHeaderGenerator::getClassName(const Message& msg) const {
    return toLowerSnakeCase(msg.name);
}

std::string CppHeaderGenerator::getParentName(const Message& msg) const {
    return msg.parent.empty() ? "" : toLowerSnakeCase(msg.parent);
}


void CppHeaderGenerator::generateProperties(std::ostringstream& out, const Message& msg) const {
    for (const auto& f : msg.fields) {
        std::string type = mapTypeToCpp(f.type);
        std::string var = "_" + toCamelCase(toLowerSnakeCase(f.name));
        out << "  " << type << " " << var << ";\n";
    }
}


void CppHeaderGenerator::generateConstructor(std::ostringstream& out, const Message& msg) const {
    std::string className = getClassName(msg);
    out << "  " << className << "() {\n";

    for (const auto& f : msg.fields) {
        std::string var = "_" + toCamelCase(toLowerSnakeCase(f.name));
        std::string def = getDefaultForCppType(mapTypeToCpp(f.type));
        if (!def.empty())
            out << "    " << var << " = " << def << ";\n";
        else
            out << "    // " << var << " = <custom init>\n";
    }

    out << "  }\n\n";
}


void CppHeaderGenerator::generateAccessors(std::ostringstream& out, const Message& msg) const {
    for (const auto& f : msg.fields) {
        std::string cppType = mapTypeToCpp(f.type);
        std::string field = toCamelCase(f.name);
        field[0] = std::toupper(field[0]);
        std::string varName = "_" + toCamelCase(toLowerSnakeCase(f.name));
        out << "  " << cppType << " get" << field << "() const { return " << varName << "; }\n";
        out << "  void set" << field << "(" << cppType << " val) { " << varName << " = val; }\n\n";
    }
}


void CppHeaderGenerator::generateSerializationFunctions(std::ostringstream& out, const Message& msg) const {
    std::string msgName = msg.name;
    std::string className = getClassName(msg);

    out << "  void toCapnp(curious::message::" << msgName << "::Builder& builder) const;\n";
    out << "  static " << className << " fromCapnp(const curious::message::" << msgName << "::Reader& reader);\n\n";
    out << "  std::string serialize() const;\n";
    out << "  static " << className << " deserialize(const std::string& data);\n";
}
