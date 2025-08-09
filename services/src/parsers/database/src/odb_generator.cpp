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

    const std::string inc_dir = joinPath(output_dir_, "include/database");
    const std::string src_dir = joinPath(output_dir_, "src/database/src");
    if (!createDirectoryIfNotExists(inc_dir) || !createDirectoryIfNotExists(src_dir)) {
        LOG_ERR << "Failed to create include/src subdirectories under: " << output_dir_ << go;
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
        
        std::string header_filename = joinPath(output_dir_+"/include/database/", toLowerCase(table.name) + ".h");
        std::string source_filename = joinPath(output_dir_+"/src/database/src/", toLowerCase(table.name) + ".cpp");
        
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
    
    // Includes (includes #pragma once)
    oss << generateIncludes(table, db_options);
    
    // Namespace begin
    oss << generateNamespaceBegin();
    
    // Forward declarations
    oss << generateForwardDeclarations(table);
    
    // ODB pragmas (before class declaration)
    oss << generateTablePragmas(table, db_options);
    
    // Class declaration
    oss << generateClassDeclaration(table);
    
    // Namespace end
    oss << generateNamespaceEnd();
    
    return oss.str();
}

std::string ODBGenerator::generateSourceFile(const Table& table, const DatabaseOptions& db_options) {
    LOG_DBG << "Generating source file for table: " << table.name << go;
    
    std::ostringstream oss;
    
    // Include header
    oss << "#include <database/" << toLowerCase(table.name) << ".h>\n\n";
    
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

bool ODBGenerator::generateCompleteSchema(const Schema& schema) {
    LOG_INFO << "Generating complete schema with proper relationships: " << schema.name << go;
    
    // First pass: Generate individual table headers
    bool success = true;
    for (const auto& table : schema.tables) {
        if (!generateTable(table, schema.database_options)) {
            success = false;
        }
    }
    
    // Second pass: Generate a combined schema validation file
    if (success) {
        std::string validation_content = generateSchemaValidation(schema);
        std::string validation_filename = joinPath(output_dir_ + "/include/database/", "schema_validation.h");
        if (!writeToFile(validation_filename, validation_content)) {
            success = false;
        }
    }
    
    return success;
}

std::string ODBGenerator::generateSchemaValidation(const Schema& schema) {
    std::ostringstream oss;
    
    oss << "#pragma once\n\n";
    oss << "// Generated schema validation header\n";
    oss << "// Schema: " << schema.name << " version: " << schema.version << "\n\n";
    
    // Include all entity headers
    for (const auto& table : schema.tables) {
        oss << "#include <database/" << toLowerCase(table.name) << ".h>\n";
    }
    
    oss << "\nnamespace " << namespace_ << " {\n\n";
    oss << "// Forward declarations complete - all entities available\n";
    oss << "// This header ensures all relationships can be properly resolved\n\n";
    
    // Add relationship validation
    oss << "template<typename Database>\n";
    oss << "bool validateSchemaRelationships(Database& db) {\n";
    oss << "    // Add your schema validation logic here\n";
    oss << "    return true;\n";
    oss << "}\n\n";
    
    oss << "} // namespace " << namespace_ << "\n";
    
    return oss.str();
}

std::string ODBGenerator::generateIncludes(const Table& table, const DatabaseOptions& db_options) {
    LOG_DBG << "Generating includes for table: " << table.name << go;
    
    std::ostringstream oss;
    oss << "#pragma once\n\n";
    oss << "// Generated ODB header file\n";
    oss << "// Table: " << table.name << "\n";
    oss << "// DO NOT MODIFY - This file is auto-generated\n\n";
    
    // Standard includes
    oss << "#include <string>\n";
    oss << "#include <vector>\n";
    oss << "#include <memory>\n";
    oss << "#include <optional>\n";
    oss << "#include <chrono>\n";
    
    // Check what additional containers we need
    bool needs_list = false, needs_set = false, needs_map = false, needs_unordered = false;
    
    for (const auto& column : table.columns) {
        if (column.container.is_container) {
            if (column.container.container_type == "list") needs_list = true;
            else if (column.container.container_type == "set" || column.container.container_type == "multiset") needs_set = true;
            else if (column.container.container_type == "map" || column.container.container_type == "multimap") needs_map = true;
            else if (column.container.container_type.find("unordered") != std::string::npos) needs_unordered = true;
        }
    }
    
    if (needs_list) oss << "#include <list>\n";
    if (needs_set) oss << "#include <set>\n";
    if (needs_map) oss << "#include <map>\n";
    if (needs_unordered) {
        oss << "#include <unordered_set>\n";
        oss << "#include <unordered_map>\n";
    }
    
    oss << "\n";
    
    // FIXED: Only include core ODB headers, NOT database-specific ones
    oss << "#include <odb/core.hxx>\n";
    
    // Base class include
    oss << "#include <database/db_object.h>\n\n";
    
    return oss.str();
}

std::string ODBGenerator::generateForwardDeclarations(const Table& table) {
    std::set<std::string> fwd_decls;
    
    // Parent class
    if (table.parent.has_value()) {
        fwd_decls.insert(toPascalCase(*table.parent));
    }
    
    // Referenced types in relationships
    for (const auto& column : table.columns) {
        if (column.relationship != RelationshipType::None && column.related_table.has_value()) {
            fwd_decls.insert(toPascalCase(*column.related_table));
        }
    }
    
    // Remove self reference
    fwd_decls.erase(toPascalCase(table.name));
    
    if (fwd_decls.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "// Forward declarations\n";
    for (const auto& type : fwd_decls) {
        oss << "class " << type << ";\n";
    }
    oss << "\n";
    
    return oss.str();
}

std::string ODBGenerator::mapCppTypeToDbType(const std::string& /*ignored*/, const Column& column) {
    std::string t = toLowerCase(column.type);

    // Treat DB/DSL type names first
    if (t == "varchar" || t == "string" || t == "char") {
        if (column.max_length && *column.max_length > 0) {
            return "VARCHAR(" + std::to_string(*column.max_length) + ")";
        }
        return "TEXT";
    }
    if (t == "text") return "TEXT";
    if (t == "bool" || t == "boolean") return "BOOLEAN";
    if (t == "bigint" || t == "long" || t == "long long") return "BIGINT";
    if (t == "int" || t == "integer") return "INTEGER";
    if (t == "smallint" || t == "short") return "SMALLINT";
    if (t == "float" || t == "real") return "REAL";
    if (t == "double" || t == "numeric" || t == "decimal") {
        if (column.precision && column.scale)
            return "DECIMAL(" + std::to_string(*column.precision) + "," + std::to_string(*column.scale) + ")";
        return "DOUBLE PRECISION";
    }
    if (t == "timestamp" || t == "datetime") return "TIMESTAMP";
    if (t == "date") return "DATE";
    if (t == "time") return "TIME";
    if (t == "blob" || t == "binary") return "BYTEA";

    // Fallback: if the C++ type is string and max_length known, use VARCHAR(n)
    // Otherwise TEXT to be safe.
    if (column.max_length && *column.max_length > 0) {
        return "VARCHAR(" + std::to_string(*column.max_length) + ")";
    }
    return "TEXT";
}

bool ODBGenerator::isOptionalType(const std::string& cpp_type) {
    return cpp_type.find("std::optional<") != std::string::npos;
}

template<typename T>
std::string ODBGenerator::safeOptionalAccess(const std::optional<T>& opt, const std::string& default_val) {
    return opt.has_value() ? std::string(opt.value()) : default_val;
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
        oss << " : public " << toPascalCase(*table.parent) << ", public db_object";
    }
    else if (table.is_abstract) {
        oss << " : public db_object"; // Abstract class inherits from db_object
    } else {
        oss << " : public db_object"; // Regular class inherits from db_object
    }
    
    oss << " {\n";
    
    // FIXED: Add friend class odb::access for private member access
    oss << indent_ << "friend class odb::access;\n\n";
    
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
        oss << indent_ << class_name << "& operator=(" << class_name << "&& other) noexcept = default;\n\n";
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
        // if (column.default_value.has_value()) {
        //     oss << " = " << *column.default_value;
        // }
        
        oss << ";\n\n";
    }
    
    return oss.str();
}

static std::string renderDefaultLiteral(DatabaseType dbt, const Column& column) {
    // Falls back to string literal if default_kind wasn't set by parser upgrades
    if (!column.default_value.has_value()) return "";

    auto q = [](const std::string& s){
        std::string out = "\"";
        for (char c: s) { if (c=='\"') out += "\\\""; else out += c; }
        out += "\"";
        return out;
    };

    switch (column.default_kind) {
        case Column::DefaultKind::Bool:
            return std::string(" default(") + (*column.default_bool ? "true" : "false") + ")";
        case Column::DefaultKind::Int:
            return " default(" + std::to_string(*column.default_int) + ")";
        case Column::DefaultKind::Float: {
            std::ostringstream oss; oss << " default(" << *column.default_float << ")";
            return oss.str();
        }
        case Column::DefaultKind::Expression: {
            // Expect sql(expr) convention from parser
            const std::string& v = *column.default_value;
            // strip sql( ... )
            if (v.rfind("sql(", 0) == 0 && v.size() >= 5 && v.back() == ')') {
                return " default(" + v.substr(4, v.size()-5) + ")";
            }
            return " default(" + v + ")";
        }
        case Column::DefaultKind::String:
        default:
            // Quote as SQL string
            return " default(" + q(*column.default_value) + ")";
    }
}

std::string ODBGenerator::generateAccessors(const Table& table) {
    LOG_DBG << "Generating accessors for table: " << table.name << go;
    
    std::ostringstream oss;
    
    oss << indent_ << "// Accessors\n";
    
    for (const auto& column : table.columns) {
        std::string cpp_type = convertToCppType(column.type, column); // FIXED: removed const&
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
    
    // Skip pragmas for relationship columns - they get handled separately
    if (column.relationship != RelationshipType::None) {
        return generateRelationshipPragmas(column, table);
    }
    
    // Generate pragma for regular columns
    oss << indent_ << "#pragma db member";
    
    // Primary key
    if (column.is_primary_key) {
        oss << " id";
        if (column.is_auto_increment) {
            oss << " auto";
        }
    }
    
    // Column name
    if (column.column_name_override.has_value()) {
        oss << " column(\"" << *column.column_name_override << "\")";
    } else {
        oss << " column(\"" << toSnakeCase(column.name) << "\")";
    }
    
    // FIXED: Better type specification handling
    std::string cpp_type = convertToCppType(column.type, column);
    bool is_optional = cpp_type.find("std::optional<") != std::string::npos;
    
    // Type specification
    if (column.db_type_override.has_value()) {
        oss << " type(\"" << *column.db_type_override << "\")";
    } else {
        std::string db_type = mapCppTypeToDbType(column.type, column);
        if (!db_type.empty()) {
            oss << " type(\"" << db_type << "\")";
        }
    }
    
    // FIXED: Proper null handling for optional types
    if (is_optional) {
        oss << " null";
    } else if (column.is_not_null && !column.is_primary_key) {
        oss << " not_null";
    }
    
    // Unique constraint
    if (column.is_unique) {
        oss << " unique";
    }
    
    // Default value
    {
        std::string d = renderDefaultLiteral(table.database_options.database_type, column);
        if (!d.empty()) oss << d;
    }
    
    // Other attributes
    if (column.is_transient) {
        oss << " transient";
    }
    
    if (column.is_version) {
        oss << " version";
    }
    
    if (column.is_indexed) {
        oss << " index";
    }
    
    if (column.is_lazy) {
        oss << " lazy";
    }
    
    if (column.is_readonly) {
        oss << " readonly";
    }
    
    oss << "\n";
    
    return oss.str();
}

std::string ODBGenerator::generateRelationshipPragmas(const Column& column, const Table& table) {
    if (column.relationship == RelationshipType::None) {
        return "";
    }
    
    std::ostringstream oss;
    
    switch (column.relationship) {
        case RelationshipType::OneToOne:
        case RelationshipType::ManyToOne: {
            oss << indent_ << "#pragma db member";
            if (column.inverse_field) {
                oss << " inverse(" << *column.inverse_field << ")";
            }
            if (column.is_not_null) {
                oss << " not_null";
            }
            if (column.related_column) {
                oss << " column(\"" << *column.related_column << "\")";
            }
            oss << "\n";
            break;
        }
            
        case RelationshipType::OneToMany:
            // One-to-many relationship
            oss << indent_ << "#pragma db member";
            if (column.inverse_field.has_value()) {
                oss << " inverse(" << *column.inverse_field << ")";
            }
            oss << "\n";
            break;
            
        case RelationshipType::ManyToMany:
        {
            oss << indent_ << "#pragma db member";

            if (column.inverse_field.has_value() && !column.inverse_field->empty()) {
                // INVERSE SIDE: only inverse(), no table/id/value here.
                // Use the C++ member name on the other class (your convention: snake + underscore).
                oss << " inverse(" << toSnakeCase(*column.inverse_field) << "_)";
            } else {
                // OWNING SIDE: specify association table/columns.
                const std::string jt = column.join_table.value_or(
                    toSnakeCase(table.name) + "_" + toSnakeCase(*column.related_table) + "s"
                );
                const std::string idc = column.join_column.value_or(toSnakeCase(table.name) + "_id");
                const std::string vc  = column.inverse_join_column.value_or(toSnakeCase(*column.related_table) + "_id");

                oss << " table(\"" << jt << "\")";
                oss << " id_column(\"" << idc << "\")";
                oss << " value_column(\"" << vc << "\")";
            }

            oss << "\n";
            break;
        }
            
        default:
            return "";
    }
    
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
    
    // Container ID column
    if (column.container.id_column.has_value()) {
        oss << " id_column(\"" << *column.container.id_column << "\")";
    }
    
    // Container value column
    if (column.container.value_column.has_value()) {
        oss << " value_column(\"" << *column.container.value_column << "\")";
    }
    
    // Container key column (for maps)
    if (column.container.key_column.has_value()) {
        oss << " key_column(\"" << *column.container.key_column << "\")";
    }
    
    // Container index column
    if (column.container.index_column.has_value()) {
        oss << " index_column(\"" << *column.container.index_column << "\")";
    }
    
    // Container options
    if (column.container.is_unordered) {
        oss << " unordered";
    }
    
    if (column.container.is_ordered && column.container.order_by.has_value()) {
        oss << " order_by(\"" << *column.container.order_by << "\")";
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
    // Use explicit C++ type override if provided
    if (column.cpp_type_override.has_value()) {
        return *column.cpp_type_override;
    }
    
    std::string base_type;
    std::string lower_type = toLowerCase(db_type);
    
    // Handle relationships first - they override everything else
    if (column.relationship != RelationshipType::None && column.related_table.has_value()) {
        std::string related_class = toPascalCase(*column.related_table);
        if (column.relationship == RelationshipType::OneToMany || 
            column.relationship == RelationshipType::ManyToMany) {
            return "std::vector<std::shared_ptr<" + related_class + ">>";
        } else {
            return "std::shared_ptr<" + related_class + ">";
        }
    }
    
    // Basic type mapping
    if (lower_type == "int" || lower_type == "integer") {
        base_type = "int";
    } else if (lower_type == "bigint" || lower_type == "long") {
        base_type = "long long";
    } else if (lower_type == "smallint" || lower_type == "short") {
        base_type = "short";
    } else if (lower_type == "tinyint") {
        base_type = "char";
    } else if (lower_type == "varchar" || lower_type == "text" || lower_type == "string" || lower_type == "char") {
        base_type = "std::string";
    } else if (lower_type == "bool" || lower_type == "boolean") {
        base_type = "bool";
    } else if (lower_type == "float" || lower_type == "real") {
        base_type = "float";
    } else if (lower_type == "double" || lower_type == "numeric" || lower_type == "decimal") {
        base_type = "double";
    } else if (lower_type == "timestamp" || lower_type == "datetime") {
        base_type = "std::chrono::system_clock::time_point";
    } else if (lower_type == "date") {
        base_type = "std::string"; // Use string for date to avoid C++20 dependency
    } else if (lower_type == "time") {
        base_type = "std::string"; // Use string for time to avoid C++20 dependency
    } else if (lower_type == "blob" || lower_type == "binary") {
        base_type = "std::vector<unsigned char>";
    } else if (lower_type == "uuid") {
        base_type = "std::string";
    } else {
        base_type = "std::string"; // Safe fallback for unknown types
    }
    
    // Handle containers for non-relationship columns
    if (column.container.is_container && column.relationship == RelationshipType::None) {
        std::string container_type = column.container.container_type;
        if (container_type == "vector") {
            base_type = "std::vector<" + base_type + ">";
        } else if (container_type == "list") {
            base_type = "std::list<" + base_type + ">";
        } else if (container_type == "set") {
            base_type = "std::set<" + base_type + ">";
        } else if (container_type == "multiset") {
            base_type = "std::multiset<" + base_type + ">";
        } else if (container_type == "unordered_set") {
            base_type = "std::unordered_set<" + base_type + ">";
        } else if (container_type == "map") {
            std::string key_type = "std::string";
            base_type = "std::map<" + key_type + ", " + base_type + ">";
        } else if (container_type == "unordered_map") {
            std::string key_type = "std::string";
            base_type = "std::unordered_map<" + key_type + ", " + base_type + ">";
        }
    }
    
    // FIXED: Better optional handling - only for simple types that are truly nullable
    if (column.is_nullable &&
        !column.is_primary_key &&
        column.relationship == RelationshipType::None &&
        !column.container.is_container &&
        !column.is_auto_increment) {
        base_type = "std::optional<" + base_type + ">";
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
    bool has_sep = (str.find_first_of("_- ") != std::string::npos);
    std::string s = has_sep ? toCamelCase(str) : str;
    if (!s.empty()) {
        s[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
    }
    return s;
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
    
    // FIXED: Better conflicting attributes validation
    if (column.is_not_null && column.is_nullable && !column.default_value.has_value()) {
        LOG_ERR << "Column " << column.name << " cannot be both not_null and nullable without default value" << go;
        return false;
    }
    
    if (column.is_auto_increment) {
        if (!column.is_primary_key) {
            LOG_WARN << "Column " << column.name << " is auto_increment but not primary key" << go;
        }
        
        // Auto increment should be numeric
        std::string lower_type = toLowerCase(column.type);
        if (lower_type != "int" && lower_type != "integer" && lower_type != "bigint" && lower_type != "long") {
            LOG_ERR << "Auto increment column " << column.name << " should be numeric type" << go;
            return false;
        }
    }
    
    // Validate container configuration
    if (column.container.is_container) {
        if (column.container.container_type.empty()) {
            LOG_ERR << "Container column " << column.name << " must specify container_type" << go;
            return false;
        }
        
        // Validate container type
        const std::vector<std::string> valid_containers = {
            "vector", "list", "set", "multiset", "unordered_set", 
            "map", "multimap", "unordered_map"
        };
        
        bool valid_container = false;
        for (const auto& valid_type : valid_containers) {
            if (column.container.container_type == valid_type) {
                valid_container = true;
                break;
            }
        }
        
        if (!valid_container) {
            LOG_ERR << "Invalid container type: " << column.container.container_type << " for column: " << column.name << go;
            return false;
        }
    }
    
    // Validate relationship configuration
    if (column.relationship != RelationshipType::None) {
        if (!column.related_table.has_value()) {
            LOG_ERR << "Relationship column " << column.name << " must specify related_table" << go;
            return false;
        }
        
        // Many-to-many should have join table
        if (column.relationship == RelationshipType::ManyToMany && !column.join_table.has_value()) {
            LOG_WARN << "Many-to-many " << column.name << " has no join_table at generator stage; expecting parser inference to have filled this." << go;
        }
    }
    
    // Validate version column constraints
    if (column.is_version) {
        std::string lower_type = toLowerCase(column.type);
        if (lower_type != "int" && lower_type != "integer" && lower_type != "bigint" && lower_type != "timestamp") {
            LOG_WARN << "Version column " << column.name << " should typically be numeric or timestamp type" << go;
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
                if (column.inverse_field && column.join_table) {
                    LOG_WARN << "Many-to-many '" << column.name
                            << "' has both inverse_field and join_table; "
                                "treating this member as inverse and ignoring table mapping." << go;
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