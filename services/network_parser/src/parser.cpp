#include <fstream>
#include <sstream>
#include <iostream>
#include <network_parser/bracket_schema_parser.h>
#include <server/logger.h>

int main(int argc, char* argv[]) {
    using namespace curious::log;

    // if (argc != 3) {
    //     LOG_ERR << "Usage: " << argv[0] << " <schema_file> <output_directory>" << go;
    //     return 1;
    // }

    // const std::string schemaFile = argv[1];
    // const std::string outputDir = argv[2];
    const std::string schemaFile = "/home/curious_bytes/Documents/CuriousBee/services/network/schema/network.dsl";
    const std::string outputDir = "/home/curious_bytes/Documents/CuriousBee/services/";

    std::ifstream file(schemaFile);
    if (!file) {
        LOG_ERR << "Cannot open schema file: " << schemaFile << go;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string schemaContent = buffer.str();

    parser::BracketSchemaParser parser;
    parser.parseSchema(schemaContent);
    parser.printParsedMessages();
    parser.generateAllFiles(outputDir);

    LOG_INFO << "Schema parsing and Cap’n Proto file generation completed!" << go;
    return 0;
}
