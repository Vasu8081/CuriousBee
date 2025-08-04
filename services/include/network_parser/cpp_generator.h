#pragma once
#include <network_parser/types.h>
#include <network_parser/utils.h>
#include <string>
#include <map>
#include <fstream>
#include <filesystem>
#include <sstream>

using namespace parser;
using namespace parser::utils;

namespace parser {

class CppGenerator {
private:
    const std::map<std::string, Message>& messages;

public:
    explicit CppGenerator(const std::map<std::string, Message>& messages) : messages(messages) {}

    // Generate message type enum header
    void generateMessageTypeEnum(const std::string& outputDir) const {
        std::filesystem::create_directories(outputDir + "/include/network");
        std::ofstream file(outputDir + "/include/network/message_type.h");
        
        file << "#pragma once\n";
        file << "#include <string>\n";
        file << "#include <cstdint>\n";
        file << "#include <messages/network_msg.capnp.h>\n\n";
        file << "namespace curious::net {\n\n";
        
        // Enum definition
        file << "enum class message_type : int {\n";
        file << "  unknown = 0,\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "  " << enumName << " = " << msg.id << ",\n";
        }
        file << "};\n\n";
        
        // toString function
        file << "inline std::string toString(message_type type) {\n";
        file << "  switch (type) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "    case message_type::" << enumName << ": return \"" << name << "\";\n";
        }
        file << "    default: return \"Unknown\";\n";
        file << "  }\n";
        file << "}\n\n";
        
        // fromString function
        file << "inline message_type fromString(const std::string& str) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "  if (str == \"" << name << "\") return message_type::" << enumName << ";\n";
        }
        file << "  return message_type::unknown;\n";
        file << "}\n\n";
        
        // fromId function
        file << "inline message_type fromId(int id) {\n";
        file << "  switch (id) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "    case " << msg.id << ": return message_type::" << enumName << ";\n";
        }
        file << "    default: return message_type::unknown;\n";
        file << "  }\n";
        file << "}\n\n";

        // toCapnp function
        file << "inline void toCapnp(message_type type, curious::message::NetworkMessage::Builder& builder) {\n";
        file << "  switch (type) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            std::string capnpEnumName = toAllUpperPreserveUnderscore(toLowerSnakeCase(name));
            file << "    case message_type::" << enumName << ": builder.setMsgType(curious::message::MessageType::" << capnpEnumName << "); break;\n";
        }
        file << "    default: builder.setMsgType(curious::message::MessageType::UNKNOWN); break;\n";
        file << "  }\n";
        file << "}\n\n";
        
        // fromCapnp function
        file << "inline message_type fromCapnp(const curious::message::NetworkMessage::Reader& reader) {\n";
        file << "  switch (reader.getMsgType()) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            std::string capnpEnumName = toAllUpperPreserveUnderscore(toLowerSnakeCase(name));
            file << "    case curious::message::MessageType::" << capnpEnumName << ": return message_type::" << enumName << ";\n";
        }
        file << "    default: return message_type::unknown;\n";
        file << "  }\n";
        file << "}\n\n";
        
        // fromCapnpType function
        file << "inline message_type fromCapnpType(curious::message::MessageType type) {\n";
        file << "  switch (type) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            std::string capnpEnumName = toAllUpperPreserveUnderscore(toLowerSnakeCase(name));
            file << "    case curious::message::MessageType::" << capnpEnumName << ": return message_type::" << enumName << ";\n";
        }
        file << "    default: return message_type::unknown;\n";
        file << "  }\n";
        file << "}\n\n";
        
        file << "}  // namespace curious::net\n";
        file.close();
    }

    // Generate factory builder header
    void generateFactoryBuilder(const std::string& outputDir) const {
        std::filesystem::create_directories(outputDir + "/include/network");
        std::ofstream file(outputDir + "/include/network/factory_builder.h");

        file << "#pragma once\n";
        file << "#include <network/message_type.h>\n";
        file << "#include <memory>\n";
        file << "#include <string>\n";
        file << "#include <stdexcept>\n";
        file << "#include <capnp/message.h>\n\n";

        // Include all message headers
        for (const auto& [name, msg] : messages) {
            file << "#include <network/" << toLowerSnakeCase(name) << ".h>\n";
        }

        file << "\nnamespace curious::net {\n\n";

        file << "class FactoryBuilder {\n";
        file << "public:\n";
        
        // createMessage by type
        file << "  static std::shared_ptr<network_message> createMessage(message_type type) {\n";
        file << "    switch (type) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "      case message_type::" << enumName << ": return std::make_shared<" << toLowerSnakeCase(name) << ">();\n";
        }
        file << "      default: return nullptr; // Unknown message type\n";
        file << "    }\n";
        file << "  }\n\n";
        
        // createMessage by string
        file << "  static std::shared_ptr<network_message> createMessage(const std::string& typeName) {\n";
        file << "    message_type type = fromString(typeName);\n";
        file << "    return createMessage(type);\n";
        file << "  }\n\n";
        
        // createMessage by id
        file << "  static std::shared_ptr<network_message> createMessage(int id) {\n";
        file << "    message_type type = fromId(id);\n";
        file << "    return createMessage(type);\n";
        file << "  }\n\n";
        
        // fromCapnp function to create a message from a Cap'n Proto reader
        file << "  static std::shared_ptr<network_message> fromCapnp(capnp::MessageReader& reader) {\n";
        file << "    auto msgType = curious::net::fromCapnp(reader.getRoot<curious::message::NetworkMessage>());\n";
        file << "    switch (msgType) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "      case message_type::" << enumName << ": {\n";
            file << "        auto typedMsg = std::make_shared<" << toLowerSnakeCase(name) << ">(" 
                 << toLowerSnakeCase(name) << "::fromCapnp(reader.getRoot<curious::message::" << msg.name << ">()));\n";
            file << "        return typedMsg;\n";
            file << "      }\n";
        }
        file << "      default: return nullptr; // Unknown message type\n";
        file << "    }\n";
        file << "  }\n\n";

        // toCapnp function - calls child's toCapnp function given type and a shared_ptr<network_message>
        file << "  static void toCapnp(capnp::MallocMessageBuilder& builder, const std::shared_ptr<network_message>& msg) {\n";
        file << "    if (!msg) {\n";
        file << "      throw std::runtime_error(\"Cannot serialize null message\");\n";
        file << "    }\n\n";
        file << "    switch (msg->getMsgType()) {\n";
        for (const auto& [name, msg] : messages) {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "      case message_type::" << enumName << ": {\n";
            file << "        auto root = builder.initRoot<curious::message::" << msg.name << ">();\n";
            file << "        auto typedMsg = std::dynamic_pointer_cast<" << toLowerSnakeCase(name) << ">(msg);\n";
            file << "        if (typedMsg) {\n";
            file << "          typedMsg->toCapnp(root);\n";
            file << "        } else {\n";
            file << "          throw std::runtime_error(\"Failed to cast message to type " << toLowerSnakeCase(name) << "\");\n";
            file << "        }\n";
            file << "        break;\n";
            file << "      }\n";
        }
        file << "      default: throw std::runtime_error(\"Unknown message type\");\n";
        file << "    }\n";
        file << "  }\n\n";

        // Serialize function - convenience method that combines toCapnp with serialization
        file << "  static std::string serialize(const std::shared_ptr<network_message>& msg) {\n";
        file << "    if (!msg) {\n";
        file << "      throw std::runtime_error(\"Cannot serialize null message\");\n";
        file << "    }\n\n";
        file << "    capnp::MallocMessageBuilder builder;\n";
        file << "    toCapnp(builder, msg);\n\n";
        file << "    kj::VectorOutputStream output;\n";
        file << "    capnp::writeMessage(output, builder);\n";
        file << "    auto data = output.getArray();\n";
        file << "    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());\n";
        file << "  }\n\n";

        // Deserialize function - convenience method that combines deserialization with fromCapnp
        file << "  static std::shared_ptr<network_message> deserialize(const std::string& data) {\n";
        file << "    if (data.empty()) {\n";
        file << "      throw std::runtime_error(\"Cannot deserialize empty data\");\n";
        file << "    }\n\n";
        file << "    try {\n";
        file << "      kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));\n";
        file << "      capnp::InputStreamMessageReader reader(input);\n";
        file << "      return fromCapnp(reader);\n";
        file << "    } catch (const std::exception& e) {\n";
        file << "      throw std::runtime_error(std::string(\"Failed to deserialize message: \") + e.what());\n";
        file << "    }\n";
        file << "  }\n\n";

        file << "};\n\n";
        file << "}  // namespace curious::net\n";

        file.close();
    }

    // Generate both files
    void generateAll(const std::string& outputDir) const {
        generateMessageTypeEnum(outputDir);
        generateFactoryBuilder(outputDir);
    }
};

}  // namespace parser