#include <parsers/network/cpp_impl_generator.h>
#include <parsers/network/utils.h>
#include <base/logger.h>

#include <fstream>
#include <filesystem>
#include <sstream>
#include <set>

using namespace parser;
using namespace parser::utils;
using namespace curious::log;

CppImplGenerator::CppImplGenerator(const std::map<std::string, Message>& messages)
    : messages(messages) {}

void CppImplGenerator::generate(const std::string& outputDir) {
    std::filesystem::create_directories(outputDir + "/network/src/readonly");

    for (const auto& [name, msg] : messages) {
        std::string path = outputDir + "/network/src/readonly/__" + toLowerSnakeCase(name) + "__.cpp";

        std::ostringstream includes;
        generateIncludes(includes, msg);

        std::ostringstream serialization;
        generateSerializationImplementations(serialization, msg);

        std::ostringstream full;
        full << includes.str();
        full << serialization.str();

        std::ofstream file(path);
        if (!file.is_open()) {
            LOG_ERR << "Failed to open file: " << path << go;
            continue;
        }

        file << full.str();
        file.close();
        LOG_INFO << "Generated implementation: " << path << go;
    }
}

void CppImplGenerator::generateIncludes(std::ostringstream& out, const Message& msg) const {
    out << "#include <network/" << toLowerSnakeCase(msg.name) << ".h>\n";
    out << "#include <capnp/message.h>\n";
    out << "#include <capnp/serialize.h>\n";
    out << "#include <kj/std/iostream.h>\n";
    out << "#include <server/logger.h>\n\n";
    out << "using namespace curious::net;\n";
    out << "using namespace curious::log;\n\n";
    
    // Add namespace declaration
    out << "namespace curious::net {\n\n";
}

void CppImplGenerator::generateSerializationImplementations(std::ostringstream& out, const Message& msg) const {
    std::string className = toLowerSnakeCase(msg.name);
    
    // Collect all fields including inherited ones
    std::vector<Field> allFields;
    collectAllFields(msg, allFields);
    
    // Generate toCapnp implementation
    generateToCapnpImpl(out, msg, className, allFields);
    
    // Generate fromCapnp implementation
    generateFromCapnpImpl(out, msg, className, allFields);
    
    // Generate serialize implementation
    generateSerializeImpl(out, msg, className);
    
    // Generate deserialize implementation
    generateDeserializeImpl(out, msg, className);
    
    // Close namespace
    out << "}  // namespace curious::net\n";
}

void CppImplGenerator::collectAllFields(const Message& msg, std::vector<Field>& allFields) const {
    // Add parent fields first
    if (!msg.parent.empty() && messages.count(msg.parent)) {
        const Message& parentMsg = messages.at(msg.parent);
        collectAllFields(parentMsg, allFields);
    }
    
    // Add own fields
    for (const auto& field : msg.fields) {
        allFields.push_back(field);
    }
}

void CppImplGenerator::generateToCapnpImpl(std::ostringstream& out, const Message& msg, 
                                          const std::string& className, const std::vector<Field>& allFields) const {
    out << "void " << className << "::toCapnp(curious::message::" << msg.name << "::Builder& builder) const {\n";
    
    // Set message type if this is NetworkMessage or derived
    if (msg.name == "NetworkMessage" || !msg.parent.empty()) {
        out << "    builder.setMsgType(curious::message::MessageType::" 
            << toAllUpperPreserveUnderscore(toLowerSnakeCase(msg.name)) << ");\n\n";
    }
    
    // Set all fields
    for (const auto& field : allFields) {
        std::string fieldName = toCamelCase(field.name);
        std::string propertyName = "_" + toCamelCase(toLowerSnakeCase(field.name));
        std::string capnpFieldName = fieldName;
        capnpFieldName[0] = std::toupper(capnpFieldName[0]);
        
        // Skip MsgType field as it's handled separately
        if (capnpFieldName == "MsgType") {
            continue;
        }
        
        if (isComplexType(field.type)) {
            generateComplexFieldSerialization(out, field, propertyName, capnpFieldName);
        } else {
            out << "    builder.set" << capnpFieldName << "(" << propertyName << ");\n";
        }
    }
    
    out << "}\n\n";
}

void CppImplGenerator::generateFromCapnpImpl(std::ostringstream& out, const Message& msg, 
                                            const std::string& className, const std::vector<Field>& allFields) const {
    out << className << " " << className << "::fromCapnp(const curious::message::" << msg.name << "::Reader& reader) {\n";
    out << "    " << className << " obj;\n\n";
    
    // Handle all fields
    for (const auto& field : allFields) {
        std::string fieldName = toCamelCase(field.name);
        std::string propertyName = "_" + toCamelCase(toLowerSnakeCase(field.name));
        std::string capnpFieldName = fieldName;
        capnpFieldName[0] = std::toupper(capnpFieldName[0]);
        
        if (capnpFieldName == "MsgType") {
            out << "    obj." << propertyName << " = fromCapnpType(reader.get" << capnpFieldName << "());\n";
            continue;
        }
        
        if (isComplexType(field.type)) {
            generateComplexFieldDeserialization(out, field, propertyName, capnpFieldName);
        } else {
            out << "    obj." << propertyName << " = reader.get" << capnpFieldName << "();\n";
        }
    }
    
    out << "\n    return obj;\n";
    out << "}\n\n";
}

void CppImplGenerator::generateSerializeImpl(std::ostringstream& out, const Message& msg, const std::string& className) const {
    out << "std::string " << className << "::serialize() const {\n";
    out << "    ::capnp::MallocMessageBuilder message;\n";
    out << "    auto builder = message.initRoot<curious::message::" << msg.name << ">();\n";
    out << "    toCapnp(builder);\n\n";
    out << "    kj::VectorOutputStream output;\n";
    out << "    capnp::writeMessage(output, message);\n";
    out << "    auto data = output.getArray();\n";
    out << "    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());\n";
    out << "}\n\n";
}

void CppImplGenerator::generateDeserializeImpl(std::ostringstream& out, const Message& msg, const std::string& className) const {
    out << className << " " << className << "::deserialize(const std::string& data) {\n";
    out << "    try {\n";
    out << "        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));\n";
    out << "        ::capnp::InputStreamMessageReader message(input);\n";
    out << "        auto reader = message.getRoot<curious::message::" << msg.name << ">();\n";
    out << "        return fromCapnp(reader);\n";
    out << "    } catch (const std::exception& e) {\n";
    out << "        LOG_ERR << \"Failed to deserialize " << msg.name << ": \" << e.what() << go;\n";
    out << "        return " << className << "{}; // Return default-constructed object\n";
    out << "    }\n";
    out << "}\n\n";
}

void CppImplGenerator::generateComplexFieldSerialization(std::ostringstream& out, const Field& field, 
                                                        const std::string& propertyName, const std::string& fieldName) const {
    if (field.type.starts_with("list<")) {
        std::string innerType = field.type.substr(5, field.type.length() - 6);
        out << "    // Serialize list field: " << field.name << "\n";
        out << "    auto " << fieldName << "List = builder.init" << fieldName << "(" << propertyName << ".size());\n";
        out << "    for (size_t i = 0; i < " << propertyName << ".size(); ++i) {\n";
        
        if (isBuiltinType(innerType)) {
            out << "        " << fieldName << "List.set(i, " << propertyName << "[i]);\n";
        } else {
            out << "        auto itemBuilder = " << fieldName << "List[i];\n";
            out << "        " << propertyName << "[i].toCapnp(itemBuilder);\n";
        }
        out << "    }\n\n";
    } else if (field.type.starts_with("Map<") || field.type.starts_with("map<")) {
        out << "    // Serialize map field: " << field.name << "\n";
        out << "    auto " << fieldName << "Entries = builder.init" << fieldName << "();\n";
        out << "    auto entries = " << fieldName << "Entries.initEntries(" << propertyName << ".size());\n";
        out << "    size_t idx = 0;\n";
        out << "    for (const auto& [key, value] : " << propertyName << ") {\n";
        out << "        auto entry = entries[idx++];\n";
        out << "        entry.setKey(key);\n";
        out << "        entry.setValue(value);\n";
        out << "    }\n\n";
    } else {
        // Custom message type
        out << "    // Serialize custom message field: " << field.name << "\n";
        out << "    auto " << fieldName << "Builder = builder.init" << fieldName << "();\n";
        out << "    " << propertyName << ".toCapnp(" << fieldName << "Builder);\n\n";
    }
}

void CppImplGenerator::generateComplexFieldDeserialization(std::ostringstream& out, const Field& field,
                                                          const std::string& propertyName, const std::string& fieldName) const {
    if (field.type.starts_with("list<")) {
        std::string innerType = field.type.substr(5, field.type.length() - 6);
        out << "    // Deserialize list field: " << field.name << "\n";
        out << "    auto " << fieldName << "List = reader.get" << fieldName << "();\n";
        out << "    obj." << propertyName << ".clear();\n";
        out << "    obj." << propertyName << ".reserve(" << fieldName << "List.size());\n";
        out << "    for (const auto& item : " << fieldName << "List) {\n";
        
        if (isBuiltinType(innerType)) {
            out << "        obj." << propertyName << ".push_back(item);\n";
        } else {
            out << "        obj." << propertyName << ".push_back(" << toLowerSnakeCase(innerType) << "::fromCapnp(item));\n";
        }
        out << "    }\n\n";
    } else if (field.type.starts_with("Map<") || field.type.starts_with("map<")) {
        out << "    // Deserialize map field: " << field.name << "\n";
        out << "    auto " << fieldName << "Reader = reader.get" << fieldName << "();\n";
        out << "    auto entries = " << fieldName << "Reader.getEntries();\n";
        out << "    obj." << propertyName << ".clear();\n";
        out << "    for (const auto& entry : entries) {\n";
        out << "        obj." << propertyName << "[entry.getKey()] = entry.getValue();\n";
        out << "    }\n\n";
    } else {
        // Custom message type
        out << "    // Deserialize custom message field: " << field.name << "\n";
        out << "    obj." << propertyName << " = " << toLowerSnakeCase(field.type) << "::fromCapnp(reader.get" << fieldName << "());\n\n";
    }
}

bool CppImplGenerator::isComplexType(const std::string& type) const {
    return type.starts_with("list<") || 
           type.starts_with("Map<") || 
           type.starts_with("map<") || 
           !isBuiltinType(type);
}

bool CppImplGenerator::isBuiltinType(const std::string& type) const {
    static const std::set<std::string> builtins = {
        "int8", "int16", "int32", "int64", "int",
        "uint8", "uint16", "uint32", "uint64", "uint",
        "float", "float32", "float64", "double",
        "bool", "string", "bytes", "Data", "Text", "void", "any"
    };
    return builtins.count(type) > 0;
}

std::string CppImplGenerator::capitalize(const std::string& str) const {
    if (str.empty()) return str;
    std::string result = str;
    result[0] = std::toupper(result[0]);
    return result;
}

std::string CppImplGenerator::toCamelCase(const std::string& snake_case) const {
    std::string result;
    bool capitalizeNext = false;
    for (char c : snake_case) {
        if (c == '_') {
            capitalizeNext = true;
        } else {
            if (capitalizeNext) {
                result += std::toupper(c);
                capitalizeNext = false;
            } else {
                result += c;
            }
        }
    }
    return result;
}

std::string CppImplGenerator::toLowerSnakeCase(const std::string& camelCase) const {
    std::string result;
    for (size_t i = 0; i < camelCase.length(); ++i) {
        char c = camelCase[i];
        if (i > 0 && std::isupper(c)) {
            result += '_';
        }
        result += std::tolower(c);
    }
    return result;
}

std::string CppImplGenerator::toAllUpperPreserveUnderscore(const std::string& str) const {
    std::string result;
    for (char c : str) {
        if (c == '_') {
            result += '_';
        } else {
            result += std::toupper(c);
        }
    }
    return result;
}

void CppImplGenerator::preserveUserDefinedContent(const std::string& path, std::ostringstream& out) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        // File doesn't exist, add default editable section
        out << "//#editable_implementation_start_dont_remove_this_line_only_write_below\n\n";
        out << "//#editable_implementation_end_dont_remove_this_line_only_write_above\n";
        return;
    }

    std::string line;
    bool inside = false;
    bool addedStart = false, addedEnd = false;
    std::ostringstream temp;

    while (std::getline(file, line)) {
        if (line.find("//#editable_implementation_start_dont_remove_this_line_only_write_below") != std::string::npos) {
            inside = true;
            addedStart = true;
            temp << line << "\n";
            continue;
        }
        if (line.find("//#editable_implementation_end_dont_remove_this_line_only_write_above") != std::string::npos) {
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
        temp << "//#editable_implementation_start_dont_remove_this_line_only_write_below\n\n";
    }
    if (!addedEnd) {
        temp << "//#editable_implementation_end_dont_remove_this_line_only_write_above\n";
    }

    out << temp.str();
}