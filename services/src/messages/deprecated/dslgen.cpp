#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <functional>

struct Field
{
    std::string type;
    std::string name;

    Field(const std::string &t, const std::string &n) : type(t), name(n) {}
};

struct Message
{
    std::string name;
    int id;
    std::string parent;
    std::vector<Field> fields;

    Message() : id(-1) {}
    Message(const std::string &n, int i, const std::string &p = "")
        : name(n), id(i), parent(p) {}
};

class BracketSchemaParser
{
private:
    std::map<std::string, Message> messages;

    std::string trim(const std::string &str)
    {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
            return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    std::string toLowerSnakeCase(const std::string &camelCase)
    {
        std::string result;
        for (size_t i = 0; i < camelCase.length(); ++i)
        {
            char c = camelCase[i];
            if (i > 0 && std::isupper(c))
            {
                result += '_';
            }
            result += std::tolower(c);
        }
        return result;
    }

    std::string toCamelCase(const std::string &snake_case)
    {
        std::string result;
        bool capitalizeNext = false;
        for (char c : snake_case)
        {
            if (c == '_')
            {
                capitalizeNext = true;
            }
            else
            {
                if (capitalizeNext)
                {
                    result += std::toupper(c);
                    capitalizeNext = false;
                }
                else
                {
                    result += c;
                }
            }
        }
        return result;
    }

    std::string toAllUpperCase(const std::string &str)
    {
        std::string result;
        for (char c : str)
        {
            result += std::toupper(c);
        }
        return result;
    }

    std::string toAllUpperPreserveUnderscore(const std::string &str)
    {
        std::string result;
        for (char c : str)
        {
            if (c == '_')
            {
                result += '_';
            }
            else
            {
                result += std::toupper(c);
            }
        }
        return result;
    }

    std::string mapTypeToCapnp(const std::string &type)
    {
        if (type == "int8")
            return "Int8";
        if (type == "int16")
            return "Int16";
        if (type == "int32")
            return "Int32";
        if (type == "int64")
            return "Int64";
        if (type == "int")
            return "Int32"; // Default to Int32 for "int" type

        if (type == "uint8")
            return "UInt8";
        if (type == "uint16")
            return "UInt16";
        if (type == "uint32")
            return "UInt32";
        if (type == "uint64")
            return "UInt64";
        if (type == "uint")
            return "UInt32"; // Default to UInt32 for "uint" type

        if (type == "float32")
            return "Float32";
        if (type == "float64")
            return "Float64";
        if (type == "float")
            return "Float32"; // Default to Float32 for "float" type
        
        if (type == "bool")
            return "Bool";

        if (type == "string")
            return "Text";
        if (type == "bytes")
            return "Data";
        
        if (type == "void")
            return "Void";

        if (type == "any")
            return "AnyPointer";
        
        if (type == "list")
            return "List";
        
        if (type.starts_with("Data<") && type.ends_with(">"))
        {
            // Handle templated Data types like Data<MyType>
            std::string innerType = type.substr(5, type.length() - 6);
            return "Data<" + mapTypeToCapnp(innerType) + ">";
        }

        if (type.starts_with("Text<") && type.ends_with(">"))
        {
            // Handle templated Text types like Text<MyType>
            std::string innerType = type.substr(5, type.length() - 6);
            return "Text<" + mapTypeToCapnp(innerType) + ">";
        }

        if (type.starts_with("list<") && type.ends_with(">"))
        {
            // Handle templated List types like List<MyType>
            std::string innerType = type.substr(5, type.length() - 6);
            return "List(" + mapTypeToCapnp(innerType) + ")";
        }
        
        if (type.starts_with("Map<") && type.ends_with(">"))
        {
            // Handle Map types like Map<Key, Value>
            std::string innerType = type.substr(4, type.length() - 5);
            size_t commaPos = innerType.find(',');
            if (commaPos != std::string::npos)
            {
                std::string keyType = trim(innerType.substr(0, commaPos));
                std::string valueType = trim(innerType.substr(commaPos + 1));
                return "Map(" + mapTypeToCapnp(keyType) + ", " + mapTypeToCapnp(valueType) + ")";
            }
        }

        // For custom types, assume they're defined structs
        return type;
    }

    std::string mapTypeToCpp(const std::string &type)
    {
        if (type == "int8")
            return "int8_t";
        if (type == "int16")
            return "int16_t";
        if (type == "int32")
            return "int32_t";
        if (type == "int64")
            return "int64_t";   
        if (type == "uint8")
            return "uint8_t";
        if (type == "uint16")
            return "uint16_t";
        if (type == "uint32")
            return "uint32_t";
        if (type == "uint64")
            return "uint64_t";
        if (type == "float32")
            return "float";
        if (type == "float64")
            return "double";
        if (type == "bool")
            return "bool";
        if (type == "string")
            return "std::string";
        if (type == "bytes")
            return "std::vector<uint8_t>";
        if (type == "void")
            return "void";
        if (type == "any")
            return "capnp::AnyPointer";
        if (type == "list")
            return "capnp::List";
        if (type == "Data")
            return "std::vector<uint8_t>";
        if (type == "Text")
            return "std::string";

        if (type.starts_with("Data<") && type.ends_with(">"))
        {
            // Handle templated Data types like Data<MyType>
            std::string innerType = type.substr(5, type.length() - 6);
            return "std::vector<" + mapTypeToCpp(innerType) + ">";
        }

        if (type.starts_with("Text<") && type.ends_with(">"))
        {
            // Handle templated Text types like Text<MyType>
            std::string innerType = type.substr(5, type.length() - 6);
            return "std::string"; // Assuming Text is always a string
        }

        if (type.starts_with("list<") && type.ends_with(">"))
        {
            // Handle templated List types like List<MyType>
            std::string innerType = type.substr(5, type.length() - 6);
            return "std::vector<" + mapTypeToCpp(innerType) + ">";
        }

        if (type.starts_with("Map<") && type.ends_with(">"))
        {
            // Handle Map types like Map<Key, Value>
            std::string innerType = type.substr(4, type.length() - 5);
            size_t commaPos = innerType.find(',');
            if (commaPos != std::string::npos)
            {
                std::string keyType = innerType.substr(0, commaPos);
                std::string valueType = innerType.substr(commaPos + 1);
                return "std::map<" + mapTypeToCpp(keyType) + ", " + mapTypeToCpp(valueType) + ">";
            }
        }

        // For custom types, return as snake_case
        return toLowerSnakeCase(type);
    }

    std::string getHeaderFile(const std::string & dataType) {
        if (dataType == "int" || dataType == "string" || dataType == "bool" || dataType == "float")
        {
            return "";
        }
        else if (dataType.starts_with("Map<") && dataType.ends_with(">"))
        {
            return "map";
        }
        else if (dataType.starts_with("list<") && dataType.ends_with(">"))
        {
            // Handle templated List types like List<MyType>
            std::string innerType = dataType.substr(5, dataType.length() - 6);
            return getHeaderFile(innerType);
        }

        // For custom types, assume they are in the network namespace
        return "network/" + toLowerSnakeCase(dataType) + ".h";
    }

    std::string filterName(const std::string &name)
    {
        if (name.starts_with("Map<") && name.ends_with(">"))
        {
            // Handle Map types like Map<Key, Value>
            std::string innerType = name.substr(4, name.length() - 5);
            size_t commaPos = innerType.find(',');
            if (commaPos != std::string::npos)
            {
                std::string keyType = innerType.substr(0, commaPos);
                std::string valueType = innerType.substr(commaPos + 1);
                return "Map(" + filterName(keyType) + ", " + filterName(valueType) + ")";
            }
        }
        return name; // Return as is for other types
    }

    std::vector<std::string> tokenize(const std::string &content)
    {
        std::vector<std::string> tokens;
        std::string current;

        for (size_t i = 0; i < content.length(); ++i)
        {
            char c = content[i];

            // Handle generic types like Map<Key,Value>
            if (std::isalpha(c)) {
                current += c;
                while (i + 1 < content.length() && (std::isalnum(content[i + 1]) || content[i + 1] == '_'))
                {
                    current += content[++i];
                }

                // Check if it's a generic type
                if (i + 1 < content.length() && content[i + 1] == '<')
                {
                    current += '<';
                    ++i;
                    int angleDepth = 1;
                    while (i + 1 < content.length() && angleDepth > 0)
                    {
                        ++i;
                        char next = content[i];
                        current += next;
                        if (next == '<')
                            angleDepth++;
                        else if (next == '>')
                            angleDepth--;
                    }
                }

                tokens.push_back(trim(current));
                current.clear();
            }
            else if (c == '{' || c == '}' || c == '(' || c == ')' || c == ';' || c == ',')
            {
                if (!current.empty())
                {
                    tokens.push_back(trim(current));
                    current.clear();
                }
                tokens.push_back(std::string(1, c));
            }
            else if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            {
                if (!current.empty())
                {
                    tokens.push_back(trim(current));
                    current.clear();
                }
            }
            else if (c == '#')
            {
                if (!current.empty())
                {
                    tokens.push_back(trim(current));
                    current.clear();
                }
                while (i < content.length() && content[i] != '\n')
                {
                    ++i;
                }
            }
            else
            {
                current += c;
            }
        }

        if (!current.empty())
        {
            tokens.push_back(trim(current));
        }

        // Debug print (optional)
        std::cout << "First 20 tokens: ";
        for (size_t i = 0; i < std::min((size_t)20, tokens.size()); ++i)
        {
            std::cout << "'" << tokens[i] << "' ";
        }
        std::cout << std::endl;

        return tokens;
    }


public:
    void parseSchema(const std::string &schemaContent)
    {
        std::cout << "Starting to parse schema..." << std::endl;

        std::vector<std::string> tokens = tokenize(schemaContent);

        std::cout << "Found " << tokens.size() << " tokens" << std::endl;

        for (size_t i = 0; i < tokens.size(); ++i)
        {
            std::string token = tokens[i];
            std::cout << "Processing token: " << token << std::endl;

            if (token == "message")
            {
                if (i + 4 < tokens.size() && tokens[i + 2] == "(" && tokens[i + 4] == ")")
                {
                    std::string name = tokens[i + 1];
                    std::cout << "Found message definition: " << name << std::endl;
                    int id = std::stoi(tokens[i + 3]);
                    std::cout << "Message ID: " << id << std::endl;
                    std::string parent = "";

                    i += 4; // Skip to after the )

                    // Check for extends clause - this is optional
                    if (i + 2 < tokens.size() && tokens[i + 1] == "extends")
                    {
                        parent = tokens[i + 2];
                        std::cout << "Found extends clause: " << parent << std::endl;
                        i += 2; // Skip "extends" and parent name
                    }

                    // Expect opening brace
                    if (i + 1 < tokens.size() && tokens[i + 1] == "{")
                    {
                        i++; // Skip the {

                        std::cout << "Found message: " << name << " (ID: " << id << ", Parent: '" << parent << "')" << std::endl;

                        messages[name] = Message(name, id, parent);
                        Message *currentMessage = &messages[name];

                        // Parse fields until closing brace
                        i++; // Move to first token after {
                        while (i < tokens.size() && tokens[i] != "}")
                        {
                            if (i + 2 < tokens.size() && tokens[i + 2] == ";")
                            {
                                std::string type = tokens[i];
                                std::string fieldName = tokens[i + 1];

                                std::cout << "  Found field: " << type << " " << fieldName << std::endl;
                                currentMessage->fields.emplace_back(type, fieldName);

                                i += 3; // Skip type, name, semicolon
                            }
                            else
                            {
                                i++; // Skip unknown token
                            }
                        }

                        if (i < tokens.size() && tokens[i] == "}")
                        {
                            // Don't increment i here! The for loop will do it
                            std::cout << "Finished parsing message: " << name << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Error: Expected '{' after message declaration for " << name << std::endl;
                    }
                }
                else
                {
                    std::cout << "Error: Invalid message syntax at token " << i << std::endl;
                }
            }
        }

        std::cout << "Parsing completed. Found " << messages.size() << " messages." << std::endl;
    }

    void customDataStructures(std::ofstream &file)
    {
        // Example custom data structure: Map<Key, Value>
        file << "struct Map(Key, Value) {\n";
        file << "  entries @0 :List(Entry);\n";
        file << "  struct Entry {\n";
        file << "    key @0 :Key;\n";
        file << "    value @1 :Value;\n";
        file << "  }\n";
        file << "}\n\n";
    }

    void generateCapnpFile(const std::string &outputDir)
    {
        std::filesystem::create_directories(outputDir + "/messages/src");

        std::ofstream file(outputDir + "/messages/src/network_msg.capnp");

        file << "@0xecd3d69fdf10c2c3;\n";
        file << "using Cxx = import \"/capnp/c++.capnp\";\n";
        file << "$Cxx.namespace(\"curious::message\");\n\n";

        // Generate structs in dependency order
        std::vector<std::string> orderedMessages;

        // First, add messages without parents
        for (const auto &[name, msg] : messages)
        {
            if (msg.parent.empty())
            {
                orderedMessages.push_back(name);
            }
        }

        // For each message type create a enum with its ID as its value
        file << "enum MessageType {\n";
        file << "  unknown @0;\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "  " << enumName << " @" << msg.id << ";\n";
        }
        file << "}\n\n";

        // Then add messages with parents (multiple passes to handle deep inheritance)
        bool addedAny = true;
        while (addedAny)
        {
            addedAny = false;
            for (const auto &[name, msg] : messages)
            {
                if (!msg.parent.empty() &&
                    std::find(orderedMessages.begin(), orderedMessages.end(), name) == orderedMessages.end() &&
                    std::find(orderedMessages.begin(), orderedMessages.end(), msg.parent) != orderedMessages.end())
                {
                    orderedMessages.push_back(name);
                    addedAny = true;
                }
            }
        }

        for (const std::string &msgName : orderedMessages)
        {
            const Message &msg = messages.at(msgName);
            file << "struct " << filterName(msg.name) << " {\n";

            std::vector<Field> allFields;

            if (!msg.parent.empty()) {
                std::function<void(const std::string &)> collectParentFields = [&](const std::string &parentName) {
                    if (messages.count(parentName) == 0) return;
                    const Message &parent = messages.at(parentName);
                    if (!parent.parent.empty()) {
                        collectParentFields(parent.parent);
                    }
                    for (const Field &field : parent.fields) {
                        allFields.push_back(field);
                    }
                };
                collectParentFields(msg.parent);
            }

            // Add own fields
            for (const Field &field : msg.fields)
            {
                allFields.push_back(field);
            }

            // Generate field definitions with indices
            for (size_t i = 0; i < allFields.size(); ++i)
            {
                const Field &field = allFields[i];
                file << "  " << toCamelCase(field.name) << " @" << i << " : "
                     << mapTypeToCapnp(field.type) << ";\n";
            }

            file << "}\n\n";
        }

        file.close();
        std::cout << "Generated: " << outputDir << "/messages/src/network_msg.capnp\n";
    }

    void generateMessageTypeEnum(const std::string &outputDir)
    {
        std::filesystem::create_directories(outputDir + "/include/network");
        std::ofstream file(outputDir + "/include/network/message_type.h");
        file << "#pragma once\n";
        file << "#include <string>\n";
        file << "#include <cstdint>\n\n";
        file << "namespace curious::net {\n\n";
        file << "enum class message_type : int {\n";
        file << "  unknown = 0,\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "  " << enumName << " = " << msg.id << ",\n";
        }
        file << "};\n\n";
        file << "inline std::string toString(message_type type) {\n";
        file << "  switch (type) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "    case message_type::" << enumName << ": return \"" << name << "\";\n";
        }
        file << "    default: return \"Unknown\";\n";
        file << "  }\n";
        file << "}\n\n";
        file << "inline message_type fromString(const std::string& str) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "  if (str == \"" << name << "\") return message_type::" << enumName << ";\n";
        }
        file << "  return message_type::unknown;\n";
        file << "}\n\n";
        file << "inline message_type fromId(int id) {\n";
        file << "  switch (id) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "    case " << msg.id << ": return message_type::" << enumName << ";\n";
        }
        file << "    default: return message_type::unknown;\n";
        file << "  }\n";
        file << "}\n\n";

        // toCapnp and fromCapnp functions
        file << "inline void toCapnp(message_type type, curious::message::NetworkMessage::Builder& builder) {\n";
        file << "  switch (type) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);

            std::string capnpEnumName = toAllUpperPreserveUnderscore(toLowerSnakeCase(name));
            file << "    case message_type::" << enumName << ": builder.setMsgType(curious::message::MessageType::" << capnpEnumName << "); break;\n";
        }
        file << "    default: builder.setMsgType(curious::message::MessageType::UNKNOWN); break;\n";
        file << "  }\n";
        file << "}\n\n";
        file << "inline message_type fromCapnp(const curious::message::NetworkMessage::Reader& reader) {\n";
        file << "  switch (reader.getMsgType()) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            std::string capnpEnumName = toAllUpperPreserveUnderscore(toLowerSnakeCase(name));
            file << "    case curious::message::MessageType::" << capnpEnumName << ": return message_type::" << enumName << ";\n";
        }
        file << "    default: return message_type::unknown;\n";
        file << "  }\n";
        file << "}\n\n";
        
        // Generate fromCapnp from curious::message::MessageType
        file << "inline message_type fromCapnpType(curious::message::MessageType type) {\n";
        file << "  switch (type) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            std::string capnpEnumName = toAllUpperPreserveUnderscore(toLowerSnakeCase(name));
            file << "    case curious::message::MessageType::" << capnpEnumName
            << ": return message_type::" << enumName << ";\n";
        }
        file << "    default: return message_type::unknown;\n";
        file << "  }\n";
        file << "}\n\n";
        
        file << "}  // namespace curious::net\n";
    }

    void generateHeaderFile(const std::string &outputDir, const std::string &messageName) {
        const Message &msg = messages.at(messageName);
        std::string className = toLowerSnakeCase(messageName);

        std::filesystem::create_directories(outputDir + "/include/network");
        std::string path = outputDir + "/include/network/" + className + ".h";

        std::string preGenerated, postGenerated;
        bool hasExistingFile = std::filesystem::exists(path);

        if (hasExistingFile) {
            std::ifstream existingFile(path);
            std::string line;
            bool inGenerated = false;

            while (std::getline(existingFile, line)) {
                if (line.find("#generated start") != std::string::npos) {
                    inGenerated = true;
                    continue;
                }
                if (line.find("#generated end") != std::string::npos) {
                    while (std::getline(existingFile, line)) {
                        postGenerated += line + "\n";
                    }
                    break;
                }

                if (!inGenerated) {
                    preGenerated += line + "\n";
                }
            }

            existingFile.close();
        }

        std::ostringstream generatedBlock;

        // Collect fields
        std::vector<Field> allFields, childFields;
        if (!msg.parent.empty() && messages.count(msg.parent)) {
            const Message &parentMsg = messages.at(msg.parent);
            for (const Field &field : parentMsg.fields) {
                allFields.push_back(field);
            }
        }
        for (const Field &field : msg.fields) {
            allFields.push_back(field);
            childFields.push_back(field);
        }

        // Begin generated block
        generatedBlock << "  // #generated start\n";
        generatedBlock << "protected:\n";
        for (const Field &field : childFields) {
            std::string cppType = mapTypeToCpp(field.type);
            std::string memberName = "_" + toCamelCase(toLowerSnakeCase(field.name));
            generatedBlock << "  " << cppType << " " << memberName << ";\n";
        }

        generatedBlock << "\npublic:\n";
        generatedBlock << "  " << className << "() {\n";
        generatedBlock << "    // Initialize default values if needed\n";
        for (const Field &field : allFields) {
            std::string cppType = mapTypeToCpp(field.type);
            std::string memberName = "_" + toCamelCase(toLowerSnakeCase(field.name));
            std::string fieldName = toCamelCase(field.name);
            fieldName[0] = std::toupper(fieldName[0]);
            if (cppType == "std::string") {
                generatedBlock << "    " << memberName << " = \"\";\n";
            } else if (cppType == "std::vector<uint8_t>") {
                generatedBlock << "    " << memberName << " = {};\n";
            } else if (cppType == "bool") {
                generatedBlock << "    " << memberName << " = false;\n";
            } else if (cppType == "int8_t" || cppType == "int16_t" || cppType == "int32_t" || cppType == "int64_t" || cppType == "int") {
                generatedBlock << "    " << memberName << " = 0;\n";
            } else if (cppType == "uint8_t" || cppType == "uint16_t" || cppType == "uint32_t" || cppType == "uint64_t" || cppType == "uint") {
                generatedBlock << "    " << memberName << " = 0;\n";
            } else if (cppType == "float") {
                generatedBlock << "    " << memberName << " = 0.0f;\n";
            } else if (cppType == "double") {
                generatedBlock << "    " << memberName << " = 0.0;\n";
            } 
            else {
                generatedBlock << "    // Initialize custom type " << cppType << " if needed\n";
            }
        }
        generatedBlock << "    " << " _msgType = message_type::"+toCamelCase(className)+";\n";
        generatedBlock << "  }\n\n";

        for (const Field &field : allFields) {
            std::string cppType = mapTypeToCpp(field.type);
            std::string memberName = "_" + toCamelCase(toLowerSnakeCase(field.name));
            std::string fieldName = toCamelCase(field.name);
            fieldName[0] = std::toupper(fieldName[0]);
            std::string getterName = "get" + fieldName;
            std::string setterName = "set" + fieldName;

            // Getter
            if (cppType == "std::string") {
                generatedBlock << "  const " << cppType << "& " << getterName << "() const { return " << memberName << "; }\n";
            } else {
                generatedBlock << "  " << cppType << " " << getterName << "() const { return " << memberName << "; }\n";
            }

            // Setter
            if (cppType == "std::string") {
                generatedBlock << "  void " << setterName << "(const " << cppType << "& value) { " << memberName << " = value; }\n";
            } else {
                generatedBlock << "  void " << setterName << "(" << cppType << " value) { " << memberName << " = value; }\n";
            }
            generatedBlock << "\n";
        }

        // Cap'n Proto and serialization functions
        if (messageName == "NetworkMessage") {
            generatedBlock << "  virtual ~" << className << "() = default;\n";
            generatedBlock << "  // Message type\n";
        }
        generatedBlock << "  // Cap'n Proto conversion functions\n";
        generatedBlock << "  void toCapnp(curious::message::" << msg.name << "::Builder& builder) const;\n";
        generatedBlock << "  static " << className << " fromCapnp(const curious::message::" << msg.name << "::Reader& reader);\n\n";
        generatedBlock << "  // Serialization helpers\n";
        generatedBlock << "  std::string serialize() const;\n";
        generatedBlock << "  static " << className << " deserialize(const std::string& data);\n";
        generatedBlock << "  // #generated end\n";

        if (hasExistingFile) {
            std::ofstream file(path);
            file << preGenerated;
            file << generatedBlock.str();
            file << postGenerated;
            std::cout << "Updated: " << path << "\n";
            return;
        }

        // Fresh generation
        std::ofstream file(path);
        file << "#pragma once\n";
        file << "#include <messages/network_msg.capnp.h>\n";
        file << "#include <string>\n";
        file << "#include <cstdint>\n";
        file << "#include <capnp/message.h>\n";
        file << "#include <capnp/serialize.h>\n\n";

        if (!msg.parent.empty())
            file << "#include <network/" << toLowerSnakeCase(msg.parent) << ".h>\n\n";

        for (const Field &field : allFields) {
            std::string headerFileName = getHeaderFile(field.type);
            if (!headerFileName.empty()) {
                file << "#include <" << headerFileName << ">\n";
            }
        }

        file << "\nnamespace curious::net {\n\n";
        if (!msg.parent.empty())
            file << "class " << className << " : public " << toLowerSnakeCase(msg.parent) << " {\n";
        else
            file << "class " << className << " {\n";

        file << generatedBlock.str();
        file << "};\n\n";
        file << "}  // namespace curious::net\n";

        file.close();
        std::cout << "Generated: " << path << "\n";
    }

    void generateCppFile(const std::string &outputDir, const std::string &messageName)
    {
        const Message &msg = messages.at(messageName);
        std::string className = toLowerSnakeCase(messageName);

        std::filesystem::create_directories(outputDir + "/network/src/readonly");

        std::ofstream file(outputDir + "/network/src/readonly/__" + className + "__.cpp");

        // Collect all fields including inherited ones
        std::vector<Field> allFields;

        // Add parent fields first
        if (!msg.parent.empty() && messages.count(msg.parent))
        {
            const Message &parentMsg = messages.at(msg.parent);
            for (const Field &field : parentMsg.fields)
            {
                allFields.push_back(field);
            }
        }

        // Add own fields
        for (const Field &field : msg.fields)
        {
            allFields.push_back(field);
        }

        file << "#include <network/" << className << ".h>\n";
        file << "#include <capnp/message.h>\n";
        file << "#include <capnp/serialize.h>\n";
        file << "#include <kj/std/iostream.h>\n\n";

        file << "namespace curious::net {\n\n";

        // Generate toCapnp implementation
        file << "void " << className << "::toCapnp(curious::message::" << msg.name << "::Builder& builder) const {\n";
        file << "  builder.setMsgType(curious::message::MessageType::" << toAllUpperPreserveUnderscore(toLowerSnakeCase(messageName)) << ");\n";
        for (const Field &field : allFields)
        {
            std::string baseName = toCamelCase(field.name);
            baseName[0] = std::toupper(baseName[0]);
            std::string memberName = "_" + toCamelCase(field.name);

            if (field.type == "string" || field.type == "int" || field.type == "bool" || field.type == "float")
            {
                file << "  builder.set" << baseName << "(" << memberName << ");\n";
            }
            else if (baseName == "MsgType") {
                continue; // Skip MsgType, handled separately
            }
            else
            {
                // Custom type - assume it has its own toCapnp method
                std::cout << "Processing field: " << field.name << " of type: " << field.type << std::endl;
                if (field.type.starts_with("list<") && field.type.ends_with(">"))
                {
                    file << "  auto " << baseName << "Builder = builder.init" << baseName << "("+memberName+".size());\n";
                    file << "  for (auto i=0; i<"+memberName+".size(); i++) {\n";
                    file << "    auto itemBuilder = " << baseName << "Builder[i];\n";
                    file << "    "+memberName+"[i].toCapnp(itemBuilder);\n";
                    file << "  }\n";
                }
                else {
                    file << "  auto " << baseName << "Builder = builder.init" << baseName << "();\n";
                    file << "  " << memberName << ".toCapnp(" << baseName << "Builder);\n";
                }
            }
        }
        file << "}\n\n";

        // Generate fromCapnp implementation
        file << className << " " << className << "::fromCapnp(const curious::message::" << msg.name << "::Reader& reader) {\n";
        file << "  " << className << " obj;\n";
        for (const Field &field : allFields)
        {
            std::string baseName = toCamelCase(field.name);
            baseName[0] = std::toupper(baseName[0]);
            std::string memberName = "_" + toCamelCase(field.name);

            if (field.type == "string" || field.type == "int" || field.type == "bool" || field.type == "float")
            {
                file << "  obj." << memberName << " = reader.get" << baseName << "();\n";
            }
            else if (baseName == "MsgType") {
                file << "  obj." << memberName << " = fromCapnpType(reader.get" << baseName << "());\n";
                continue;
            }
            else
            {
                if (field.type.starts_with("list<") && field.type.ends_with(">"))
                {
                    // Handle list types
                    file << "  auto " << baseName << "Reader = reader.get" << baseName << "();\n";
                    file << "  obj." << memberName << ".clear();\n";
                    file << "  obj." << memberName << ".reserve(" << baseName << "Reader.size());\n";
                    file << "  for (auto i=0; i<" << baseName << "Reader.size(); i++) {\n";
                    file << "    obj." << memberName << "[i] =";
                    std::string cppType = mapTypeToCpp(field.type.substr(5, field.type.length() - 6));
                    file << cppType << "::fromCapnp(" << baseName << "Reader[i]);\n";
                    file << "  }\n";
                }
                else if (field.type == "Map<Key, Value>")
                {
                    // Handle Map types
                    file << "  auto " << baseName << "Reader = reader.get" << baseName << "();\n";
                    file << "  for (auto& entry : " << baseName << "Reader) {\n";
                    file << "    obj." << memberName << ".emplace(entry.getKey(), ";
                    std::string valueType = field.type.substr(4, field.type.length() - 5);
                    file << mapTypeToCpp(valueType) << "::fromCapnp(entry.getValue()));\n";
                    file << "  }\n";
                }
                else
                {
                    std::string cppType = mapTypeToCpp(field.type);
                    file << "  obj." << memberName << " = " << cppType << "::fromCapnp(reader.get" << baseName << "());\n";
                }
            }
        }
        file << "  return obj;\n";
        file << "}\n\n";

        // Generate serialize implementation
        file << "std::string " << className << "::serialize() const {\n";
        file << "  ::capnp::MallocMessageBuilder message;\n";
        file << "  auto builder = message.initRoot<curious::message::" << msg.name << ">();\n";
        file << "  toCapnp(builder);\n";
        file << "  \n";
        file << "  kj::VectorOutputStream output;\n";
        file << "  capnp::writeMessage(output, message);\n";
        file << "  auto data = output.getArray();\n";
        file << "  return std::string(reinterpret_cast<const char*>(data.begin()), data.size());\n";
        file << "}\n\n";

        // Generate deserialize implementation
        file << className << " " << className << "::deserialize(const std::string& data) {\n";
        file << "  kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));\n";
        file << "  ::capnp::InputStreamMessageReader message(input);\n";
        file << "  auto reader = message.getRoot<curious::message::" << msg.name << ">();\n";
        file << "  return fromCapnp(reader);\n";
        file << "}\n\n";

        file << "}  // namespace curious::net\n";

        file.close();
        if (!std::filesystem::exists(outputDir + "/network/src/"+ className + ".cpp"))
        {
            std::ofstream file(outputDir + "/network/src/" + className + ".cpp");
            file << "#include <network/" << className << ".h>\n";
            file << "namespace curious::net {\n";
            file << "// Write your custom methods here \n\n";
            file << "}\n\n";
            file.close();
            std::cout << "Generated: " << outputDir << "/network/src/" + className + ".cpp\n";
        }
        
        std::cout << "Generated: " << outputDir << "/network/src/readonly/__" + className + "__.cpp" << std::endl;
    }

    void generateFactoryBuilderFile(const std::string &outputDir)
    {
        std::filesystem::create_directories(outputDir + "/include/network");
        std::ofstream file(outputDir + "/include/network/factory_builder.h");

        file << "#pragma once\n";
        file << "#include <network/message_type.h>\n";
        file << "#include <memory>\n";
        file << "#include <string>\n\n";

        for (const auto &[name, msg] : messages)
        {
            std::string headerFileName = getHeaderFile(msg.name);
            if (!headerFileName.empty())
            {
                file << "#include <" << headerFileName << ">\n";
            }
        }

        file << "namespace curious::net {\n\n";

        file << "class FactoryBuilder {\n";
        file << "public:\n";
        file << "  static std::shared_ptr<network_message> createMessage(message_type type) {\n";
        file << "    switch (type) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "      case message_type::" << enumName << ": return std::make_shared<" << toLowerSnakeCase(name) << ">();\n";
        }
        file << "      default: return nullptr; // Unknown message type\n";
        file << "    }\n";
        file << "  }\n\n";
        file << "  static std::shared_ptr<network_message> createMessage(const std::string &typeName) {\n";
        file << "    message_type type = fromString(typeName);\n";
        file << "    return createMessage(type);\n";
        file << "  }\n\n";
        file << "  static std::shared_ptr<network_message> createMessage(int id) {\n";
        file << "    message_type type = fromId(id);\n";
        file << "    return createMessage(type);\n";
        file << "  }\n\n";
        // fromCapnp function to create a message from a Cap'n Proto reader
        file << "  static std::shared_ptr<network_message> fromCapnp(capnp::MessageReader &reader) {\n";
        file << "    auto msgType = curious::net::fromCapnp(reader.getRoot<curious::message::NetworkMessage>());\n";
        file << "    switch (msgType) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            file << "      case message_type::" << enumName << ": {\n";
            file << "        auto typedMsg = std::make_shared<" << toLowerSnakeCase(name) << ">("+toLowerSnakeCase(name)+"::fromCapnp(reader.getRoot<curious::message::" << msg.name << ">()));\n";
            file << "        return typedMsg;\n";
            file << "      }\n";
        }
        file << "      default: return nullptr; // Unknown message type\n";
        file << "    }\n";
        file << "  }\n\n";

        // Call childs toCapnp function given type and a shared_ptr<network_message>
        file << "  static void toCapnp(capnp::MallocMessageBuilder& builder, const std::shared_ptr<network_message>& msg) {\n";
        file << "    switch (msg->getMsgType()) {\n";
        for (const auto &[name, msg] : messages)
        {
            std::string enumName = toCamelCase(name);
            enumName[0] = std::tolower(enumName[0]);
            std::string capnpEnumName = toAllUpperPreserveUnderscore(toLowerSnakeCase(name));
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
        file << "};\n\n";


        file << "}  // namespace curious::net\n";

        file.close();
    }

    void generateAllFiles(const std::string &outputDir)
    {
        generateCapnpFile(outputDir);
        generateMessageTypeEnum(outputDir);
        generateFactoryBuilderFile(outputDir);
        for (const auto &[name, msg] : messages)
        {
            generateHeaderFile(outputDir, name);
            generateCppFile(outputDir, name);
        }
    }

    void printParsedMessages()
    {
        std::cout << "\nParsed messages:\n";
        for (const auto &[name, msg] : messages)
        {
            std::cout << "  " << name << " (ID: " << msg.id << ", Parent: " << msg.parent << ")\n";
            for (const Field &field : msg.fields)
            {
                std::cout << "    " << field.type << " " << field.name << "\n";
            }
        }
    }
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <schema_file> <output_directory>\n";
        return 1;
    }

    std::string schemaFile = argv[1];
    std::string outputDir = argv[2];

    // Read schema file
    std::ifstream file(schemaFile);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open schema file: " << schemaFile << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string schemaContent = buffer.str();
    file.close();

    // Parse and generate
    BracketSchemaParser parser;
    parser.parseSchema(schemaContent);
    parser.printParsedMessages();
    parser.generateAllFiles(outputDir);

    std::cout << "Schema parsing and file generation completed successfully!\n";
    return 0;
}