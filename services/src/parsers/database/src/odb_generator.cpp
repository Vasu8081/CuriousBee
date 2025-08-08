#include <parsers/database/odb_generator.h>
//----------------------------------------------
// Implementation
//----------------------------------------------

using namespace parser;

ODBGenerator::ODBGenerator(const std::string& output_directory)
    : output_dir_(output_directory)
    , namespace_("model")
    , include_guard_prefix_("GENERATED_")
    , indent_("    ") {
    LOG_DBG << "ODBGenerator initialized with output directory: " << output_directory << go;
}

void ODBGenerator::setOutputDirectory(const std::string& directory) {
    output_dir_ = directory;
    LOG_INFO << "Output directory set to: " << directory << go;
}

void ODBGenerator::setNamespace(const std::string& ns) {
    namespace_ = ns;
    LOG_INFO << "Namespace set to: " << ns << go;
}

void ODBGenerator::setIncludeGuardPrefix(const std::string& prefix) {
    include_guard_prefix_ = prefix;
    LOG_INFO << "Include guard prefix set to: " << prefix << go;
}

void ODBGenerator::setIndentation(const std::string& indent) {
    indent_ = indent;
    LOG_DBG << "Indentation set to: '" << indent << "'" << go;
}

bool ODBGenerator::generateSchema(const Schema& schema) {
    LOG_INFO << "Generating schema: " << schema.name << " version: " << schema.version << go;
    
    if (!createDirectoryIfNotExists(output_dir_)) {
        LOG_ERR << "Failed to create output directory: " << output_dir_ << go;
        return false;
    }
    
    bool success = true;
    for (const auto& table : schema.tables) {
        if (!generateTable(table, schema.database_options)) {
            LOG_ERR << "Failed to generate table: " << table.name << go;
            success = false;
        }
    }
    
    // Generate schema-level files (views, queries, etc.)
    if (!schema.global_views.empty() || !schema.global_queries.empty()) {
        // TODO: Implement global views and queries generation
        LOG_WARN << "Global views and queries generation not yet implemented" << go;
    }
    
    LOG_INFO << "Schema generation " << (success ? "completed successfully" : "completed with errors") << go;
    return success;
}

bool ODBGenerator::generateTable(const Table& table, const DatabaseOptions& db_options) {
    LOG_INFO << "Generating table: " << table.name << go;
    
    if (!validateTable(table)) {
        LOG_ERR << "Table validation failed for: " << table.name << go;
        return false;
    }
    
    try {
        std::string header_content = generateHeaderFile(table, db_options);
        std::string source_content = generateSourceFile(table, db_options);
        
        std::string header_filename = joinPath(output_dir_, toLowerCase(table.name) + ".h");
        std::string source_filename = joinPath(output_dir_, toLowerCase(table.name) + ".cpp");
        
        bool header_success = writeToFile(header_filename, header_content);
        bool source_success = writeToFile(source_filename, source_content);
        
        if (header_success && source_success) {
            LOG_INFO << "Successfully generated files for table: " << table.name << go;
            return true;
        } else {
            LOG_ERR << "Failed to write files for table: " << table.name << go;
            return false;
        }
    } catch (const std::exception& e) {
        LOG_ERR << "Exception while generating table " << table.name << ": " << e.what() << go;
        return false;
    }
}

bool ODBGenerator::generateAllTables(const std::vector<Table>& tables, const DatabaseOptions& db_options) {
    LOG_INFO << "Generating " << tables.size() << " tables" << go;
    
    bool success = true;
    for (const auto& table : tables) {
        if (!generateTable(table, db_options)) {
            success = false;
        }
    }
    
    return success;
}

std::string ODBGenerator::generateHeaderFile(const Table& table, const DatabaseOptions& db_options) {
    LOG_DBG << "Generating header file for table: " << table.name << go;
    
    std::ostringstream oss;
    
    // Include guard
    std::string guard_name = generateIncludeGuard(toLowerCase(table.name) + "_h");
    oss << "#ifndef " << guard_name << "\n";
    oss << "#define " << guard_name << "\n\n";
    
    // Includes
    oss << generateIncludes(table, db_options) << "\n";
    
    // Namespace begin
    oss << generateNamespaceBegin() << "\n";
    
    // ODB pragmas (before class declaration)
    oss << generateTablePragmas(table, db_options) << "\n";
    
    // Class declaration
    oss << generateClassDeclaration(table) << "\n";
    
    // Namespace end
    oss << generateNamespaceEnd() << "\n";
    
    // Include guard end
    oss << "#endif // " << guard_name << "\n";
    
    return oss.str();
}

std::string ODBGenerator::generateSourceFile(const Table& table, const DatabaseOptions& db_options) {
    LOG_DBG << "Generating source file for table: " << table.name << go;
    
    std::ostringstream oss;
    
    // Include header
    oss << "#include \"" << toLowerCase(table.name) << ".h\"\n\n";
    
    // Additional includes for implementation
    oss << "#include <odb/database.hxx>\n";
    oss << "#include <odb/transaction.hxx>\n\n";
    
    // Namespace begin
    oss << generateNamespaceBegin() << "\n";
    
    // Constructor implementations
    oss << generateConstructors(table) << "\n";
    
    // Callback implementations
    oss << generateCallbackDeclarations(table) << "\n";
    
    // Namespace end
    oss << generateNamespaceEnd() << "\n";
    
    return oss.str();
}

std::string ODBGenerator::generateIncludes(const Table& table, const DatabaseOptions& db_options) {
    LOG_DBG << "Generating includes for table: " << table.name << go;
    
    std::ostringstream oss;
    oss << "// Generated ODB header file\n";
    oss << "// Table: " << table.name << "\n\n";
    
    // Standard includes
    oss << "#include <string>\n";
    oss << "#include <vector>\n";
    oss << "#include <memory>\n";
    oss << "#include <optional>\n";
    oss << "#include <chrono>\n\n";
    
    // ODB includes
    oss << "#include <odb/core.hxx>\n";
    
    // Database-specific includes
    switch (db_options.database_type) {
        case DatabaseType::MySQL:
            oss << "#include <odb/mysql/database.hxx>\n";
            break;
        case DatabaseType::PostgreSQL:
            oss << "#include <odb/pgsql/database.hxx>\n";
            break;
        case DatabaseType::SQLite:
            oss << "#include <odb/sqlite/database.hxx>\n";
            break;
        default:
            oss << "#include <odb/database.hxx>\n";
            break;
    }
    
    // Container includes based on columns
    std::set<std::string> container_includes;
    for (const auto& column : table.columns) {
        if (column.container.is_container) {
            if (column.container.container_type == "vector") {
                container_includes.insert("#include <vector>");
            } else if (column.container.container_type == "list") {
                container_includes.insert("#include <list>");
            } else if (column.container.container_type == "set") {
                container_includes.insert("#include <set>");
            } else if (column.container.container_type == "map") {
                container_includes.insert("#include <map>");
            }
        }
    }
    
    for (const auto& include : container_includes) {
        oss << include << "\n";
    }
    
    if (!container_includes.empty()) {
        oss << "\n";
    }
    
    // Parent class include
    if (table.parent.has_value()) {
        oss << "#include \"" << toLowerCase(*table.parent) << ".h\"\n\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateClassDeclaration(const Table& table) {
    LOG_DBG << "Generating class declaration for table: " << table.name << go;
    
    std::ostringstream oss;
    
    // Class comment
    oss << "/**\n";
    oss << " * " << table.name << " entity class\n";
    if (table.description.has_value()) {
        oss << " * " << *table.description << "\n";
    }
    oss << " */\n";
    
    // Class declaration
    std::string class_name = table.custom_class_name.value_or(toPascalCase(table.name));
    oss << "class " << class_name;
    
    // Inheritance
    if (table.parent.has_value()) {
        oss << " : public " << toPascalCase(*table.parent);
    }
    
    oss << " {\n";
    oss << "public:\n";
    
    // Constructors and destructor
    oss << indent_ << "// Constructors\n";
    oss << indent_ << class_name << "();\n";
    if (table.generate_copy_constructor) {
        oss << indent_ << class_name << "(const " << class_name << "& other);\n";
    }
    if (table.generate_move_constructor) {
        oss << indent_ << class_name << "(" << class_name << "&& other) noexcept;\n";
    }
    oss << indent_ << "~" << class_name << "();\n\n";
    
    // Assignment operators
    if (table.generate_assignment_operator) {
        oss << indent_ << "// Assignment operators\n";
        oss << indent_ << class_name << "& operator=(const " << class_name << "& other);\n";
        oss << indent_ << class_name << "& operator=(" << class_name << "&& other) noexcept;\n\n";
    }
    
    // Accessors
    oss << generateAccessors(table);
    
    // Operators
    if (table.generate_equals || table.generate_hash) {
        oss << generateOperators(table);
    }
    
    oss << "private:\n";
    
    // Member variables with column pragmas
    oss << generateMemberVariables(table);
    
    oss << "};\n";
    
    return oss.str();
}

std::string ODBGenerator::generateMemberVariables(const Table& table) {
    LOG_DBG << "Generating member variables for table: " << table.name << go;
    
    std::ostringstream oss;
    
    for (const auto& column : table.columns) {
        // Generate column pragmas
        std::string pragmas = generateColumnPragmas(column, table);
        if (!pragmas.empty()) {
            oss << pragmas;
        }
        
        // Generate member variable
        std::string cpp_type = convertToCppType(column.type, column);
        std::string member_name = toSnakeCase(column.name) + "_";
        
        oss << indent_ << cpp_type << " " << member_name;
        
        // Default value
        if (column.default_value.has_value()) {
            oss << " = " << *column.default_value;
        }
        
        oss << ";\n\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateAccessors(const Table& table) {
    LOG_DBG << "Generating accessors for table: " << table.name << go;
    
    std::ostringstream oss;
    
    oss << indent_ << "// Accessors\n";
    
    for (const auto& column : table.columns) {
        std::string cpp_type = convertToCppType(column.type, column);
        std::string member_name = toSnakeCase(column.name) + "_";
        std::string accessor_name = toCamelCase(column.name);
        
        // Getter
        oss << indent_ << "const " << cpp_type << "& get" << toPascalCase(accessor_name) << "() const { return " << member_name << "; }\n";
        
        // Setter
        if (!column.is_readonly) {
            oss << indent_ << "void set" << toPascalCase(accessor_name) << "(const " << cpp_type << "& value) { " << member_name << " = value; }\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateOperators(const Table& table) {
    std::ostringstream oss;
    
    if (table.generate_equals) {
        oss << indent_ << "// Equality operators\n";
        std::string class_name = table.custom_class_name.value_or(toPascalCase(table.name));
        oss << indent_ << "bool operator==(const " << class_name << "& other) const;\n";
        oss << indent_ << "bool operator!=(const " << class_name << "& other) const;\n\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateConstructors(const Table& table) {
    std::ostringstream oss;
    std::string class_name = table.custom_class_name.value_or(toPascalCase(table.name));
    
    // Default constructor
    oss << "// Default constructor\n";
    oss << class_name << "::" << class_name << "() {\n";
    oss << indent_ << "// Initialize default values\n";
    oss << "}\n\n";
    
    return oss.str();
}

std::string ODBGenerator::generateCallbackDeclarations(const Table& table) {
    std::ostringstream oss;
    
    for (const auto& callback : table.callbacks) {
        oss << "// " << getCallbackTypeString(callback.type) << " callback\n";
        oss << "void " << callback.function_name << "() ";
        if (callback.is_const) oss << "const ";
        oss << "{\n";
        oss << indent_ << "// TODO: Implement callback logic\n";
        oss << "}\n\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateTablePragmas(const Table& table, const DatabaseOptions& db_options) {
    LOG_DBG << "Generating table pragmas for: " << table.name << go;
    
    std::ostringstream oss;
    
    // Object pragma
    oss << "#pragma db object";
    
    if (table.is_abstract) {
        oss << " abstract";
    }
    
    if (table.session_options.session_required) {
        oss << " session";
    }
    
    // Table name
    if (table.table_name_override.has_value()) {
        oss << " table(\"" << *table.table_name_override << "\")";
    } else if (!table.name.empty()) {
        oss << " table(\"" << toSnakeCase(table.name) << "\")";
    }
    
    // Schema
    if (table.schema_name.has_value()) {
        oss << " schema(\"" << *table.schema_name << "\")";
    }
    
    oss << "\n";
    
    // Inheritance pragmas
    oss << generateInheritancePragmas(table);
    
    // Cache pragmas
    oss << generateCachePragmas(table);
    
    // Index pragmas
    oss << generateIndexPragmas(table);
    
    // Constraint pragmas
    oss << generateConstraintPragmas(table);
    
    // Callback pragmas
    oss << generateCallbackPragmas(table);
    
    return oss.str();
}

std::string ODBGenerator::generateColumnPragmas(const Column& column, const Table& table) {
    LOG_DBG << "Generating column pragmas for: " << column.name << go;
    
    std::ostringstream oss;
    
    oss << indent_ << "#pragma db member";
    
    // Primary key
    if (column.is_primary_key) {
        oss << " id";
        if (column.is_auto_increment) {
            oss << " auto";
        }
    }
    
    // Column name override
    if (column.column_name_override.has_value()) {
        oss << " column(\"" << *column.column_name_override << "\")";
    }
    
    // Type override
    if (column.db_type_override.has_value()) {
        oss << " type(\"" << *column.db_type_override << "\")";
    }
    
    // Null handling
    if (column.is_not_null) {
        oss << " not_null";
    }
    
    // Unique constraint
    if (column.is_unique) {
        oss << " unique";
    }
    
    // Default value
    if (column.default_value.has_value()) {
        oss << " default(" << *column.default_value << ")";
    }
    
    // Transient
    if (column.is_transient) {
        oss << " transient";
    }
    
    // Version
    if (column.is_version) {
        oss << " version";
    }
    
    // Index
    if (column.is_indexed) {
        oss << " index";
    }
    
    oss << "\n";
    
    // Relationship pragmas
    std::string relationship_pragmas = generateRelationshipPragmas(column, table);
    if (!relationship_pragmas.empty()) {
        oss << relationship_pragmas;
    }
    
    // Container pragmas
    std::string container_pragmas = generateContainerPragmas(column);
    if (!container_pragmas.empty()) {
        oss << container_pragmas;
    }
    
    return oss.str();
}

std::string ODBGenerator::generateRelationshipPragmas(const Column& column, const Table& table) {
    if (column.relationship == RelationshipType::None) {
        return "";
    }
    
    std::ostringstream oss;
    
    switch (column.relationship) {
        case RelationshipType::OneToOne:
            oss << indent_ << "#pragma db member one_to_one";
            break;
        case RelationshipType::OneToMany:
            oss << indent_ << "#pragma db member one_to_many";
            break;
        case RelationshipType::ManyToOne:
            oss << indent_ << "#pragma db member many_to_one";
            break;
        case RelationshipType::ManyToMany:
            oss << indent_ << "#pragma db member many_to_many";
            break;
        default:
            return "";
    }
    
    // Inverse field
    if (column.inverse_field.has_value()) {
        oss << " inverse(" << *column.inverse_field << ")";
    }
    
    // Join table for many-to-many
    if (column.relationship == RelationshipType::ManyToMany && column.join_table.has_value()) {
        oss << " table(\"" << *column.join_table << "\")";
    }
    
    oss << "\n";
    
    return oss.str();
}

std::string ODBGenerator::generateContainerPragmas(const Column& column) {
    if (!column.container.is_container) {
        return "";
    }
    
    std::ostringstream oss;
    
    oss << indent_ << "#pragma db member";
    
    // Container table
    if (column.container.table_name.has_value()) {
        oss << " table(\"" << *column.container.table_name << "\")";
    }
    
    // Container options
    if (column.container.is_unordered) {
        oss << " unordered";
    }
    
    if (column.container.capacity.has_value()) {
        oss << " capacity(" << *column.container.capacity << ")";
    }
    
    oss << "\n";
    
    return oss.str();
}

std::string ODBGenerator::generateIndexPragmas(const Table& table) {
    std::ostringstream oss;
    
    for (const auto& index : table.indexes) {
        oss << "#pragma db index";
        
        if (!index.name.empty()) {
            oss << " name(\"" << index.name << "\")";
        }
        
        if (index.is_unique) {
            oss << " unique";
        }
        
        oss << " member(";
        for (size_t i = 0; i < index.columns.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << toSnakeCase(index.columns[i]) << "_";
        }
        oss << ")\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateConstraintPragmas(const Table& table) {
    std::ostringstream oss;
    
    for (const auto& constraint : table.constraints) {
        oss << "#pragma db constraint";
        
        if (!constraint.name.empty()) {
            oss << " name(\"" << constraint.name << "\")";
        }
        
        if (constraint.condition.has_value() && !constraint.condition.value().empty()) {
            oss << " condition(\"" << *constraint.condition << "\")";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateInheritancePragmas(const Table& table) {
    if (!table.parent.has_value()) {
        return "";
    }
    
    std::ostringstream oss;
    
    if (table.is_polymorphic) {
        oss << "#pragma db polymorphic\n";
        
        if (table.discriminator_column.has_value()) {
            oss << "#pragma db discriminator column(\"" << *table.discriminator_column << "\")\n";
        }
        
        if (table.discriminator_value.has_value()) {
            oss << "#pragma db discriminator value(\"" << *table.discriminator_value << "\")\n";
        }
    }
    
    return oss.str();
}

std::string ODBGenerator::generateCallbackPragmas(const Table& table) {
    std::ostringstream oss;
    
    for (const auto& callback : table.callbacks) {
        oss << "#pragma db callback(" << getCallbackTypeString(callback.type) << ", " << callback.function_name << ")\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateCachePragmas(const Table& table) {
    if (!table.is_cacheable) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "#pragma db cache\n";
    
    if (table.cache_strategy.has_value()) {
        oss << "#pragma db cache_strategy(\"" << *table.cache_strategy << "\")\n";
    }
    
    return oss.str();
}

std::string ODBGenerator::generateSessionPragmas(const Table& table) {
    if (!table.session_options.session_required) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "#pragma db session\n";
    
    return oss.str();
}

// Utility function implementations

std::string ODBGenerator::convertToCppType(const std::string& db_type, const Column& column) {
    if (column.cpp_type_override.has_value()) {
        return *column.cpp_type_override;
    }
    
    std::string base_type;
    std::string lower_type = toLowerCase(db_type);
    
    if (lower_type == "int" || lower_type == "integer") {
        base_type = "int";
    } else if (lower_type == "bigint" || lower_type == "long") {
        base_type = "long long";
    } else if (lower_type == "varchar" || lower_type == "text" || lower_type == "string") {
        base_type = "std::string";
    } else if (lower_type == "bool" || lower_type == "boolean") {
        base_type = "bool";
    } else if (lower_type == "float") {
        base_type = "float";
    } else if (lower_type == "double") {
        base_type = "double";
    } else if (lower_type == "timestamp" || lower_type == "datetime") {
        base_type = "std::chrono::time_point<std::chrono::system_clock>";
    } else {
        base_type = db_type; // Use as-is
    }
    
    // Handle containers
    if (column.container.is_container) {
        std::string container_type = column.container.container_type;
        if (container_type == "vector") {
            base_type = "std::vector<" + base_type + ">";
        } else if (container_type == "list") {
            base_type = "std::list<" + base_type + ">";
        } else if (container_type == "set") {
            base_type = "std::set<" + base_type + ">";
        }
    }
    
    // Handle optional/nullable
    if (column.is_nullable && !column.is_not_null) {
        base_type = "std::optional<" + base_type + ">";
    }
    
    // Handle pointers
    if (column.is_pointer) {
        base_type = "std::shared_ptr<" + base_type + ">";
    }
    
    return base_type;
}

std::string ODBGenerator::generateIncludeGuard(const std::string& filename) {
    std::string guard = include_guard_prefix_ + toUpperCase(filename);
    std::replace(guard.begin(), guard.end(), '.', '_');
    std::replace(guard.begin(), guard.end(), '/', '_');
    return guard;
}

std::string ODBGenerator::generateNamespaceBegin() {
    if (namespace_.empty()) {
        return "";
    }
    return "namespace " + namespace_ + " {\n";
}

std::string ODBGenerator::generateNamespaceEnd() {
    if (namespace_.empty()) {
        return "";
    }
    return "} // namespace " + namespace_ + "\n";
}

std::string ODBGenerator::sanitizeIdentifier(const std::string& identifier) {
    std::string result = identifier;
    // Replace invalid characters with underscores
    for (char& c : result) {
        if (!std::isalnum(c) && c != '_') {
            c = '_';
        }
    }
    
    // Ensure it doesn't start with a number
    if (!result.empty() && std::isdigit(result[0])) {
        result = "_" + result;
    }
    
    return result;
}

std::string ODBGenerator::getRelationshipString(RelationshipType type) {
    switch (type) {
        case RelationshipType::OneToOne: return "one_to_one";
        case RelationshipType::OneToMany: return "one_to_many";
        case RelationshipType::ManyToOne: return "many_to_one";
        case RelationshipType::ManyToMany: return "many_to_many";
        default: return "none";
    }
}

std::string ODBGenerator::getCascadeString(CascadeType type) {
    switch (type) {
        case CascadeType::Persist: return "persist";
        case CascadeType::Update: return "update";
        case CascadeType::Delete: return "delete";
        case CascadeType::All: return "all";
        default: return "none";
    }
}

std::string ODBGenerator::getCallbackTypeString(CallbackType type) {
    switch (type) {
        case CallbackType::PrePersist: return "pre_persist";
        case CallbackType::PostPersist: return "post_persist";
        case CallbackType::PreUpdate: return "pre_update";
        case CallbackType::PostUpdate: return "post_update";
        case CallbackType::PreErase: return "pre_erase";
        case CallbackType::PostErase: return "post_erase";
        case CallbackType::PreLoad: return "pre_load";
        case CallbackType::PostLoad: return "post_load";
        default: return "unknown";
    }
}

std::string ODBGenerator::getDatabaseTypeString(DatabaseType type) {
    switch (type) {
        case DatabaseType::MySQL: return "mysql";
        case DatabaseType::PostgreSQL: return "pgsql";
        case DatabaseType::SQLite: return "sqlite";
        case DatabaseType::Oracle: return "oracle";
        case DatabaseType::MSSQL: return "mssql";
        default: return "generic";
    }
}

bool ODBGenerator::writeToFile(const std::string& filename, const std::string& content) {
    LOG_DBG << "Writing file: " << filename << go;
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            LOG_ERR << "Failed to open file for writing: " << filename << go;
            return false;
        }
        
        file << content;
        file.close();
        
        LOG_DBG << "Successfully wrote file: " << filename << " (size: " << content.size() << " bytes)" << go;
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERR << "Exception while writing file " << filename << ": " << e.what() << go;
        return false;
    }
}

bool ODBGenerator::createDirectoryIfNotExists(const std::string& path) {
    LOG_DBG << "Creating directory if not exists: " << path << go;
    
    try {
        if (!std::filesystem::exists(path)) {
            bool result = std::filesystem::create_directories(path);
            if (result) {
                LOG_INFO << "Created directory: " << path << go;
            } else {
                LOG_ERR << "Failed to create directory: " << path << go;
            }
            return result;
        }
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERR << "Exception while creating directory " << path << ": " << e.what() << go;
        return false;
    }
}

std::string ODBGenerator::joinPath(const std::string& base, const std::string& path) {
    std::filesystem::path base_path(base);
    std::filesystem::path file_path(path);
    return (base_path / file_path).string();
}

std::string ODBGenerator::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string ODBGenerator::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ODBGenerator::toCamelCase(const std::string& str) {
    std::string result;
    bool capitalize_next = false;
    
    for (char c : str) {
        if (c == '_' || c == '-' || c == ' ') {
            capitalize_next = true;
        } else {
            if (capitalize_next) {
                result += std::toupper(c);
                capitalize_next = false;
            } else {
                result += std::tolower(c);
            }
        }
    }
    
    return result;
}

std::string ODBGenerator::toPascalCase(const std::string& str) {
    std::string camelCase = toCamelCase(str);
    if (!camelCase.empty()) {
        camelCase[0] = std::toupper(camelCase[0]);
    }
    return camelCase;
}

std::string ODBGenerator::toSnakeCase(const std::string& str) {
    std::string result;
    
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        
        if (std::isupper(c)) {
            if (i > 0 && std::islower(str[i-1])) {
                result += '_';
            }
            result += std::tolower(c);
        } else if (c == '-' || c == ' ') {
            result += '_';
        } else {
            result += c;
        }
    }
    
    return result;
}

bool ODBGenerator::validateTable(const Table& table) {
    LOG_DBG << "Validating table: " << table.name << go;
    
    if (table.name.empty()) {
        LOG_ERR << "Table name cannot be empty" << go;
        return false;
    }
    
    if (table.columns.empty()) {
        LOG_ERR << "Table " << table.name << " must have at least one column" << go;
        return false;
    }
    
    // Check for primary key
    bool has_primary_key = false;
    for (const auto& column : table.columns) {
        if (column.is_primary_key) {
            has_primary_key = true;
            break;
        }
    }
    
    if (!has_primary_key && !table.is_view) {
        LOG_WARN << "Table " << table.name << " does not have a primary key" << go;
    }
    
    // Validate each column
    for (const auto& column : table.columns) {
        if (!validateColumn(column, table)) {
            return false;
        }
    }
    
    // Validate relationships
    if (!validateRelationships(table)) {
        return false;
    }
    
    // Validate constraints
    if (!validateConstraints(table)) {
        return false;
    }
    
    LOG_DBG << "Table " << table.name << " validation passed" << go;
    return true;
}

bool ODBGenerator::validateColumn(const Column& column, const Table& table) {
    if (column.name.empty()) {
        LOG_ERR << "Column name cannot be empty in table " << table.name << go;
        return false;
    }
    
    if (column.type.empty()) {
        LOG_ERR << "Column " << column.name << " in table " << table.name << " must have a type" << go;
        return false;
    }
    
    // Validate conflicting attributes
    if (column.is_not_null && column.is_nullable) {
        LOG_ERR << "Column " << column.name << " cannot be both not_null and nullable" << go;
        return false;
    }
    
    if (column.is_auto_increment && !column.is_primary_key) {
        LOG_WARN << "Column " << column.name << " is auto_increment but not primary key" << go;
    }
    
    // Validate container configuration
    if (column.container.is_container) {
        if (column.container.container_type.empty()) {
            LOG_ERR << "Container column " << column.name << " must specify container_type" << go;
            return false;
        }
    }
    
    return true;
}

bool ODBGenerator::validateRelationships(const Table& table) {
    for (const auto& column : table.columns) {
        if (column.relationship != RelationshipType::None) {
            if (!column.related_table.has_value()) {
                LOG_ERR << "Relationship column " << column.name << " must specify related_table" << go;
                return false;
            }
            
            // Validate many-to-many specific requirements
            if (column.relationship == RelationshipType::ManyToMany) {
                if (!column.join_table.has_value()) {
                    LOG_WARN << "Many-to-many relationship " << column.name << " should specify join_table" << go;
                }
            }
        }
    }
    
    return true;
}

bool ODBGenerator::validateConstraints(const Table& table) {
    // Validate constraint column references
    for (const auto& constraint : table.constraints) {
        for (const auto& col_name : constraint.columns) {
            bool found = false;
            for (const auto& column : table.columns) {
                if (column.name == col_name) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                LOG_ERR << "Constraint " << constraint.name << " references non-existent column " << col_name << go;
                return false;
            }
        }
    }
    
    // Validate index column references
    for (const auto& index : table.indexes) {
        for (const auto& col_name : index.columns) {
            bool found = false;
            for (const auto& column : table.columns) {
                if (column.name == col_name) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                LOG_ERR << "Index " << index.name << " references non-existent column " << col_name << go;
                return false;
            }
        }
    }
    
    return true;
}