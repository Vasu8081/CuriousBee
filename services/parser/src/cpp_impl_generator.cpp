// #include <parser/cpp_impl_generator.h>
// #include <parser/utils.h>
// #include <server/logger.h>

// #include <fstream>
// #include <filesystem>

// using namespace parser;
// using namespace curious::log;

// CppImplGenerator::CppImplGenerator(const std::map<std::string, Message>& messages)
//     : messages(messages) {}

// void CppImplGenerator::generate(const std::string& outputDir) {
//     std::filesystem::create_directories(outputDir + "/network/src/readonly");

//     for (const auto& [name, msg] : messages) {
//         std::string cppName = utils::toLowerSnakeCase(name);
//         std::string readonlyPath = outputDir + "/network/src/readonly/__" + cppName + "__.cpp";
//         writeReadonlyCppFile(msg, readonlyPath);
//         createEditableStubIfNeeded(msg, outputDir + "/network/src");
//         LOG_INFO << "Generated impl: " << readonlyPath << go;
//     }
// }

// // TODO: writeReadonlyCppFile() and createEditableStubIfNeeded() — derived from original `generateCppFile()`
