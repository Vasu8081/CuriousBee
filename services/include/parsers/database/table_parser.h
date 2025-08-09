#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <parsers/database/types.h>
#include <parsers/database/odb_generator.h>
#include <base/logger.h>   // <-- Don't forget to include your logger!

namespace parser {

class TableDefinitionParser {
public:
    struct ParsedColumn {
        std::string name;
        std::string type;
        std::vector<std::string> attributes;
        std::map<std::string, std::string> options;
    };
    
    struct ParsedTable {
        std::string name;
        std::vector<std::string> attributes;
        std::map<std::string, std::string> options;
        std::vector<ParsedColumn> columns;
        std::string parent;
    };

private:
    std::string content_;
    size_t position_;
    int line_number_;
    
    // Parsing state
    std::vector<ParsedTable> parsed_tables_;
    std::map<std::string, std::string> global_options_;

    static std::string snake(const std::string& in) {
        std::string out;
        out.reserve(in.size() * 2);
        for (size_t i = 0; i < in.size(); ++i) {
            char c = in[i];
            if (std::isupper(static_cast<unsigned char>(c))) {
                if (i > 0 && (std::islower(static_cast<unsigned char>(in[i-1])) || std::isdigit(static_cast<unsigned char>(in[i-1]))))
                    out.push_back('_');
                out.push_back(static_cast<char>(std::tolower(c)));
            } else if (c == ' ') {
                out.push_back('_');
            } else {
                out.push_back(c);
            }
        }
        return out;
    }

    static std::string default_join_table(const std::string& a, const std::string& b) {
        // Keep deterministic order: this_table first (so User->Group => user_group*s*)
        return snake(a) + "_" + snake(b) + "s"; // optional plural 's' to match your style; drop if you prefer exact a_b
    }

    static std::string id_col(const std::string& tableName) {
        return snake(tableName) + "_id";
    }

public:
    explicit TableDefinitionParser(const std::string& content) 
        : content_(content), position_(0), line_number_(1) {}

    bool parse() {
        LOG_INFO << "Starting parsing of table definitions" << go;
        try {
            skipWhitespace();
            
            while (position_ < content_.length()) {
                if (parseGlobalOption()) {
                    continue;
                }
                
                if (parseTable()) {
                    continue;
                }
                
                // Skip unknown content
                if (position_ < content_.length()) {
                    LOG_INFO << "Skipping unknown character at pos=" << position_ << " char='" << content_[position_] << "' line=" << line_number_ << go;
                    position_++;
                }
            }
            
            LOG_INFO << "Parsing completed successfully with " << parsed_tables_.size() << " tables parsed" << go;
            return true;
        } catch (const std::exception& e) {
            LOG_INFO << "Parse error at line " << line_number_ << ": " << e.what() << go;
            return false;
        }
    }

    Schema generateSchema(const std::string& schema_name = "generated_schema") {
        LOG_INFO << "Generating schema object from parsed tables. Schema name: " << schema_name << go;
        Schema schema(schema_name);

        // Set global database options
        if (global_options_.find("database") != global_options_.end()) {
            std::string db_type = global_options_["database"];
            LOG_INFO << "Database type set as: " << db_type << go;
            if (db_type == "mysql") schema.database_options.database_type = DatabaseType::MySQL;
            else if (db_type == "postgresql") schema.database_options.database_type = DatabaseType::PostgreSQL;
            else if (db_type == "sqlite") schema.database_options.database_type = DatabaseType::SQLite;
            else if (db_type == "oracle") schema.database_options.database_type = DatabaseType::Oracle;
            else if (db_type == "mssql") schema.database_options.database_type = DatabaseType::MSSQL;
        }
        
        if (global_options_.find("charset") != global_options_.end()) {
            schema.database_options.charset = global_options_["charset"];
            LOG_INFO << "Database charset set as: " << schema.database_options.charset << go;
        }
        
        // Convert parsed tables to schema tables
        int table_id = 1;
        for (const auto& parsed_table : parsed_tables_) {
            LOG_INFO << "Converting parsed table: " << parsed_table.name << go;
            Table table = convertToTable(parsed_table, table_id++);
            schema.tables.push_back(table);
        }
        inferAndValidate(schema);

        LOG_INFO << "Schema generation complete. Total tables: " << schema.tables.size() << go;
        return schema;
    }

    void inferAndValidate(Schema& schema) {
        // Build table map
        std::map<std::string, Table*> tmap;
        for (auto& t : schema.tables) tmap[t.name] = &t;

        // 1) Infer M2M defaults and validate relationships
        for (auto& t : schema.tables) {
            for (auto& c : t.columns) {
                if (c.relationship == RelationshipType::ManyToMany) {
                    // related_table may be omitted; fall back to type
                    if (!c.related_table.has_value() || c.related_table->empty()) {
                        c.related_table = c.type; // DSL uses the type token for the target
                    }
                    if (!c.related_table.has_value() || !tmap.count(*c.related_table)) {
                        LOG_INFO << "Error: ManyToMany column " << t.name << "." << c.name
                                << " references unknown table '" << (c.related_table ? *c.related_table : std::string("<none>")) 
                                << "'" << go;
                        throw std::runtime_error("Unknown related_table in many_to_many");
                    }

                    // join_table default
                    if (!c.join_table.has_value() || c.join_table->empty()) {
                        c.join_table = default_join_table(t.name, *c.related_table);
                        LOG_INFO << "Inferred join_table for " << t.name << "." << c.name
                                << " = " << *c.join_table << go;
                    }
                    // join_column default
                    if (!c.join_column.has_value() || c.join_column->empty()) {
                        c.join_column = id_col(t.name);
                        LOG_INFO << "Inferred join_column for " << t.name << "." << c.name
                                << " = " << *c.join_column << go;
                    }
                    // inverse_join_column default
                    if (!c.inverse_join_column.has_value() || c.inverse_join_column->empty()) {
                        c.inverse_join_column = id_col(*c.related_table);
                        LOG_INFO << "Inferred inverse_join_column for " << t.name << "." << c.name
                                << " = " << *c.inverse_join_column << go;
                    }
                    // inverse_field sanity check
                    if (c.inverse_field.has_value()) {
                        Table* other = tmap[*c.related_table];
                        bool found_inverse = false;
                        for (auto& oc : other->columns) {
                            if (oc.name == *c.inverse_field) {
                                found_inverse = true;
                                if (oc.relationship != RelationshipType::ManyToMany) {
                                    LOG_INFO << "Error: inverse_field " << other->name << "." << oc.name
                                            << " is not many_to_many" << go;
                                    throw std::runtime_error("inverse_field kind mismatch");
                                }
                                break;
                            }
                        }
                        if (!found_inverse) {
                            LOG_INFO << "Error: inverse_field " << *c.inverse_field << " not found on table "
                                    << other->name << go;
                            throw std::runtime_error("inverse_field missing");
                        }
                    }
                } else if (c.relationship == RelationshipType::ManyToOne ||
                        c.relationship == RelationshipType::OneToOne ||
                        c.relationship == RelationshipType::OneToMany) {
                    // Basic related table check
                    if (!c.related_table.has_value() || c.related_table->empty()) {
                        // Common shorthand: type token is the target
                        c.related_table = c.type;
                    }
                    if (!c.related_table.has_value() || !tmap.count(*c.related_table)) {
                        LOG_INFO << "Error: Relationship column " << t.name << "." << c.name
                                << " references unknown table '" << (c.related_table ? *c.related_table : std::string("<none>")) 
                                << "'" << go;
                        throw std::runtime_error("Unknown related_table");
                    }
                }
            }
        }

        // 2) Populate referenced_by for FK-like relations (not m2m; those live on a join table)
        for (auto& t : schema.tables) {
            for (auto& c : t.columns) {
                if (c.relationship == RelationshipType::ManyToOne ||
                    c.relationship == RelationshipType::OneToOne) {
                    // Owning side is typically ManyToOne / the side with the FK.
                    const std::string& target = *c.related_table;
                    auto it = tmap.find(target);
                    if (it != tmap.end()) {
                        ForeignKeyRef ref;
                        ref.referring_table = t.name;
                        ref.referring_column = c.name; // logical field; physical FK column might be inferred later
                        ref.relationship = c.relationship;
                        ref.inverse_field_name = c.inverse_field;
                        ref.cascade_on_delete = c.cascade_type; // reuse
                        it->second->referenced_by.push_back(std::move(ref));
                    }
                }
            }
        }

        // 3) Final sanity: not_null/nullable already normalized; ensure m2m join bits exist
        for (auto& t : schema.tables) {
            for (auto& c : t.columns) {
                if (c.relationship == RelationshipType::ManyToMany) {
                    if (!c.join_table || !c.join_column || !c.inverse_join_column) {
                        LOG_INFO << "Error: Incomplete many_to_many at " << t.name << "." << c.name << go;
                        throw std::runtime_error("Incomplete many_to_many");
                    }
                }
            }
        }
    }

    const std::vector<ParsedTable>& getParsedTables() const {
        return parsed_tables_;
    }

private:
    bool parseGlobalOption() {
        skipWhitespace();
        
        // Look for @option_name = value
        if (position_ < content_.length() && content_[position_] == '@') {
            position_++; // Skip @
            
            std::string option_name = parseIdentifier();
            if (option_name.empty()) return false;
            
            skipWhitespace();
            if (position_ >= content_.length() || content_[position_] != '=') return false;
            position_++; // Skip =
            
            skipWhitespace();
            std::string value = parseStringOrIdentifier();
            
            global_options_[option_name] = value;
            LOG_INFO << "Parsed global option @" << option_name << " = " << value << " (line " << line_number_ << ")" << go;
            return true;
        }
        
        return false;
    }

    bool parseTable() {
        skipWhitespace();
        
        // Look for "Table" keyword
        if (!matchKeyword("Table")) {
            return false;
        }
        
        ParsedTable table;
        
        // Parse table name
        skipWhitespace();
        table.name = parseIdentifier();
        if (table.name.empty()) {
            LOG_INFO << "Error: Expected table name after 'Table' at line " << line_number_ << go;
            throw std::runtime_error("Expected table name after 'Table'");
        }
        LOG_INFO << "Found table: " << table.name << " (line " << line_number_ << ")" << go;

        skipWhitespace();
        
        // Parse optional inheritance
        if (position_ < content_.length() && content_[position_] == ':') {
            position_++; // Skip ':'
            skipWhitespace();
            table.parent = parseIdentifier();
            LOG_INFO << "  Table " << table.name << " inherits from " << table.parent << go;
            skipWhitespace();
        }
        
        // Parse optional attributes
        if (position_ < content_.length() && content_[position_] == '[') {
            parseAttributes(table.attributes, table.options);
            LOG_INFO << "  Table " << table.name << " attributes: [" 
                     << (table.attributes.empty() ? "none" : "") 
                     << join(table.attributes, ", ")
                     << "] options: [" << joinMap(table.options) << "]" << go;
            skipWhitespace();
        }
        
        // Expect opening brace
        if (position_ >= content_.length() || content_[position_] != '{') {
            LOG_INFO << "Error: Expected '{' after table name at line " << line_number_ << go;
            throw std::runtime_error("Expected '{' after table name");
        }
        position_++; // Skip '{'
        
        // Parse table contents
        while (position_ < content_.length()) {
            skipWhitespace();
            
            if (content_[position_] == '}') {
                position_++; // Skip '}'
                break;
            }
            
            // Parse column
            ParsedColumn column;
            if (parseColumn(column)) {
                LOG_INFO << "    Column: " << column.name << " type: " << column.type
                         << " attrs: [" << join(column.attributes, ", ")
                         << "] opts: [" << joinMap(column.options) << "]" << go;
                table.columns.push_back(column);
            } else {
                LOG_INFO << "Error: Expected column definition or '}' at line " << line_number_ << go;
                throw std::runtime_error("Expected column definition or '}'");
            }
        }
        
        parsed_tables_.push_back(table);
        LOG_INFO << "Table " << table.name << " parsed successfully with " << table.columns.size() << " columns" << go;
        return true;
    }

    bool parseColumn(ParsedColumn& column) {
        skipWhitespace();
        
        // Parse column name
        column.name = parseIdentifier();
        if (column.name.empty()) {
            return false;
        }
        
        skipWhitespace();
        
        // Expect ':'
        if (position_ >= content_.length() || content_[position_] != ':') {
            LOG_INFO << "Error: Expected ':' after column name " << column.name << " at line " << line_number_ << go;
            throw std::runtime_error("Expected ':' after column name");
        }
        position_++; // Skip ':'
        
        skipWhitespace();
        
        // Parse column type
        column.type = parseIdentifier();
        if (column.type.empty()) {
            LOG_INFO << "Error: Expected column type after ':' for column " << column.name << " at line " << line_number_ << go;
            throw std::runtime_error("Expected column type after ':'");
        }
        
        skipWhitespace();
        
        // Parse optional attributes
        if (position_ < content_.length() && content_[position_] == '[') {
            parseAttributes(column.attributes, column.options);
        }
        
        skipWhitespace();
        if (position_ < content_.length() && content_[position_] == ';') {
            position_++; // Skip ';'
        } else {
            LOG_INFO << "Error: Expected ';' after column " << column.name
                    << " (line " << line_number_ << ")" << go;
            throw std::runtime_error("Expected ';' after column");
        }
        
        return true;
    }

    void parseAttributes(std::vector<std::string>& attributes, std::map<std::string, std::string>& options) {
        if (position_ >= content_.length() || content_[position_] != '[') {
            return;
        }
        position_++; // Skip '['
        
        while (position_ < content_.length() && content_[position_] != ']') {
            skipWhitespace();
            
            std::string attr = parseIdentifier();
            if (attr.empty()) break;
            
            skipWhitespace();
            
            // Check for key=value pair
            if (position_ < content_.length() && content_[position_] == '=') {
                position_++; // Skip '='
                skipWhitespace();
                std::string value = parseStringOrIdentifier();
                options[attr] = value;
            } else {
                attributes.push_back(attr);
            }
            
            skipWhitespace();
            
            // Skip comma if present
            if (position_ < content_.length() && content_[position_] == ',') {
                position_++;
            }
        }
        
        if (position_ < content_.length() && content_[position_] == ']') {
            position_++; // Skip ']'
        }
    }

    std::string parseIdentifier() {
        skipWhitespace();
        
        if (position_ >= content_.length() || 
            (!std::isalpha(content_[position_]) && content_[position_] != '_')) {
            return "";
        }
        
        std::string identifier;
        while (position_ < content_.length() && 
               (std::isalnum(content_[position_]) || content_[position_] == '_')) {
            identifier += content_[position_++];
        }
        
        return identifier;
    }

    std::string parseStringOrIdentifier() {
        skipWhitespace();
        
        if (position_ >= content_.length()) {
            return "";
        }
        
        // Parse quoted string
        if (content_[position_] == '"' || content_[position_] == '\'') {
            char quote = content_[position_++];
            std::string str;
            
            while (position_ < content_.length() && content_[position_] != quote) {
                if (content_[position_] == '\\' && position_ + 1 < content_.length()) {
                    position_++; // Skip backslash
                    switch (content_[position_]) {
                        case 'n': str += '\n'; break;
                        case 't': str += '\t'; break;
                        case 'r': str += '\r'; break;
                        case '\\': str += '\\'; break;
                        case '"': str += '"'; break;
                        case '\'': str += '\''; break;
                        default: str += content_[position_]; break;
                    }
                } else {
                    str += content_[position_];
                }
                position_++;
            }
            
            if (position_ < content_.length()) {
                position_++; // Skip closing quote
            }
            
            return str;
        }
        
        if (std::isdigit(content_[position_]) || content_[position_] == '-') {
            std::string num;
            if (content_[position_] == '-') num += content_[position_++];
            while (position_ < content_.length() && std::isdigit(content_[position_])) {
                num += content_[position_++];
            }
            // Optionally handle float/double:
            if (position_ < content_.length() && content_[position_] == '.') {
                num += content_[position_++];
                while (position_ < content_.length() && std::isdigit(content_[position_])) {
                    num += content_[position_++];
                }
            }
            return num;
        }
        // Parse identifier
        return parseIdentifier();
    }

    bool matchKeyword(const std::string& keyword) {
        size_t saved_pos = position_;
        
        for (char c : keyword) {
            if (position_ >= content_.length() || 
                std::tolower(content_[position_]) != std::tolower(c)) {
                position_ = saved_pos;
                return false;
            }
            position_++;
        }
        
        // Ensure it's not part of a longer identifier
        if (position_ < content_.length() && 
            (std::isalnum(content_[position_]) || content_[position_] == '_')) {
            position_ = saved_pos;
            return false;
        }
        
        return true;
    }

    void skipWhitespace() {
        while (position_ < content_.length()) {
            char c = content_[position_];
            if (std::isspace(c)) {
                if (c == '\n') line_number_++;
                position_++;
            } else if (c == '/' && position_ + 1 < content_.length() && content_[position_ + 1] == '/') {
                // Skip line comment
                position_ += 2;
                while (position_ < content_.length() && content_[position_] != '\n') {
                    position_++;
                }
            } else if (c == '/' && position_ + 1 < content_.length() && content_[position_ + 1] == '*') {
                // Skip block comment
                position_ += 2;
                while (position_ + 1 < content_.length()) {
                    if (content_[position_] == '*' && content_[position_ + 1] == '/') {
                        position_ += 2;
                        break;
                    }
                    if (content_[position_] == '\n') line_number_++;
                    position_++;
                }
            } else {
                break;
            }
        }
    }

    Table convertToTable(const ParsedTable& parsed_table, int id) {
        Table table(parsed_table.name, id, parsed_table.parent);
        
        // Process table attributes
        for (const auto& attr : parsed_table.attributes) {
            if (attr == "abstract") table.is_abstract = true;
            else if (attr == "polymorphic") table.is_polymorphic = true;
            else if (attr == "cacheable") table.is_cacheable = true;
            else if (attr == "audited") table.is_audited = true;
            else if (attr == "view") table.is_view = true;
            else if (attr == "immutable") table.is_immutable = true;
            else if (attr == "session") table.session_options.session_required = true;
        }
        
        // Process table options
        for (const auto& [key, value] : parsed_table.options) {
            if (key == "table_name") table.table_name_override = value;
            else if (key == "schema") table.schema_name = value;
            else if (key == "class_name") table.custom_class_name = value;
            else if (key == "discriminator_column") table.discriminator_column = value;
            else if (key == "discriminator_value") table.discriminator_value = value;
            else if (key == "description") table.description = value;
            else if (key == "cache_strategy") table.cache_strategy = value;
            else if (key == "inheritance") {
                if (value == "table_per_class") table.inheritance_strategy = InheritanceStrategy::TablePerClass;
                else if (value == "table_per_subclass") table.inheritance_strategy = InheritanceStrategy::TablePerSubclass;
                else if (value == "polymorphic") table.inheritance_strategy = InheritanceStrategy::Polymorphic;
            }
        }
        
        // Convert columns
        for (const auto& parsed_column : parsed_table.columns) {
            Column column = convertToColumn(parsed_column);
            table.columns.push_back(column);
        }
        
        return table;
    }

    Column convertToColumn(const ParsedColumn& parsed_column) {
        Column column(parsed_column.type, parsed_column.name);
        
        // Process column attributes
        for (const auto& attr : parsed_column.attributes) {
            if (attr == "primary_key" || attr == "pk") column.is_primary_key = true;
            else if (attr == "auto_increment" || attr == "auto") column.is_auto_increment = true;
            else if (attr == "unique") column.is_unique = true;
            else if (attr == "not_null") column.is_not_null = true;
            else if (attr == "nullable") column.is_nullable = true;
            else if (attr == "transient") column.is_transient = true;
            else if (attr == "lazy") column.is_lazy = true;
            else if (attr == "embedded") column.is_embedded = true;
            else if (attr == "version") column.is_version = true;
            else if (attr == "indexed") column.is_indexed = true;
            else if (attr == "pointer") column.is_pointer = true;
            else if (attr == "inverse") column.is_inverse = true;
            else if (attr == "readonly") column.is_readonly = true;
            else if (attr == "container") column.container.is_container = true;
            
            // Relationship attributes
            else if (attr == "one_to_one") column.relationship = RelationshipType::OneToOne;
            else if (attr == "one_to_many") column.relationship = RelationshipType::OneToMany;
            else if (attr == "many_to_one") column.relationship = RelationshipType::ManyToOne;
            else if (attr == "many_to_many") column.relationship = RelationshipType::ManyToMany;
        }
        
        // Process column options
        for (const auto& [key, value] : parsed_column.options) {
            if (key == "column_name") column.column_name_override = value;
            else if (key == "db_type") column.db_type_override = value;
            else if (key == "cpp_type") column.cpp_type_override = value;
            else if (key == "default") {
                column.default_value = value;

                // Infer default kind
                auto lower = [](std::string s){ 
                    std::transform(s.begin(), s.end(), s.begin(), ::tolower); 
                    return s;
                };

                std::string lv = lower(value);
                if (lv == "true" || lv == "false") {
                    column.default_kind = Column::DefaultKind::Bool;
                    column.default_bool = (lv == "true");
                } else {
                    // int?
                    char* endptr = nullptr;
                    errno = 0;
                    long long ival = std::strtoll(value.c_str(), &endptr, 10);
                    if (errno == 0 && endptr && *endptr == '\0') {
                        column.default_kind = Column::DefaultKind::Int;
                        column.default_int = ival;
                    } else {
                        // float?
                        errno = 0;
                        char* fend = nullptr;
                        double fval = std::strtod(value.c_str(), &fend);
                        if (errno == 0 && fend && *fend == '\0' && value.find('.') != std::string::npos) {
                            column.default_kind = Column::DefaultKind::Float;
                            column.default_float = fval;
                        } else if (value.rfind("sql(", 0) == 0 && value.size() >= 5 && value.back() == ')') {
                            // sql(expr) wrapper to allow raw expressions (e.g., sql(now()))
                            column.default_kind = Column::DefaultKind::Expression;
                        } else {
                            column.default_kind = Column::DefaultKind::String;
                        }
                    }
                }
            }
            else if (key == "max_length") column.max_length = std::stoul(value);
            else if (key == "precision") column.precision = std::stoi(value);
            else if (key == "scale") column.scale = std::stoi(value);
            else if (key == "comment") column.comment = value;
            
            // Relationship options
            else if (key == "related_table") column.related_table = value;
            else if (key == "related_column") column.related_column = value;
            else if (key == "inverse_field") column.inverse_field = value;
            else if (key == "join_table") column.join_table = value;
            else if (key == "join_column") column.join_column = value;
            else if (key == "inverse_join_column") column.inverse_join_column = value;
            
            // Foreign key options
            else if (key == "fk_name") column.fk_name = value;
            else if (key == "on_delete") column.on_delete = value;
            else if (key == "on_update") column.on_update = value;
            
            // Container options
            else if (key == "container_type") column.container.container_type = value;
            else if (key == "table_name") column.container.table_name = value;
            else if (key == "id_column") column.container.id_column = value;
            else if (key == "value_column") column.container.value_column = value;
            else if (key == "key_column") column.container.key_column = value;
            else if (key == "index_column") column.container.index_column = value;
            else if (key == "capacity") column.container.capacity = std::stoul(value);
            else if (key == "order_by") column.container.order_by = value;
        }
        
        // Handle container flags
        if (column.container.is_container && column.container.container_type.empty()) {
            column.container.container_type = "vector"; // Default container type
        }
        
        return column;
    }

    // --- Utility for logging vector/map in log lines ---
    static std::string join(const std::vector<std::string>& v, const std::string& sep) {
        std::ostringstream oss;
        for (size_t i = 0; i < v.size(); ++i) {
            if (i > 0) oss << sep;
            oss << v[i];
        }
        return oss.str();
    }
    static std::string joinMap(const std::map<std::string, std::string>& m) {
        std::ostringstream oss;
        bool first = true;
        for (const auto& kv : m) {
            if (!first) oss << ", ";
            first = false;
            oss << kv.first << "=" << kv.second;
        }
        return oss.str();
    }
};

} // namespace parser