#include <parsers/database/odb_generator.h>
#include <parsers/database/table_parser.h>
#include <iostream>

using namespace parser;

// Main function to demonstrate usage

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_directory>" << std::endl;
        std::cerr << "Example: " << argv[0] << " tables.def ./generated" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_dir = argv[2];
    
    try {
        // Read input file
        std::ifstream file(input_file);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file: " << input_file << std::endl;
            return 1;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Parse table definitions
        parser::TableDefinitionParser parser(content);
        if (!parser.parse()) {
            std::cerr << "Error: Failed to parse table definitions" << std::endl;
            return 1;
        }
        
        // Generate schema
        parser::Schema schema = parser.generateSchema("MySchema");
        
        std::cout << "Parsed " << schema.tables.size() << " tables:" << std::endl;
        for (const auto& table : schema.tables) {
            std::cout << "  - " << table.name << " (" << table.columns.size() << " columns)" << std::endl;
        }
        
        // Generate ODB files
        parser::ODBGenerator generator(output_dir);
        generator.setNamespace("model");
        
        bool success = generator.generateSchema(schema);
        
        if (success) {
            std::cout << "Successfully generated ODB files in: " << output_dir << std::endl;
        } else {
            std::cerr << "Error: Failed to generate ODB files" << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}