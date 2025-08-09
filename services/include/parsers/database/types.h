#pragma once
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <set>

namespace parser {

//----------------------------------------------
// Enums
//----------------------------------------------
enum class RelationshipType {
    None,
    OneToOne,
    OneToMany,
    ManyToOne,
    ManyToMany
};

enum class CallbackType {
    PrePersist,
    PostPersist,
    PreUpdate,
    PostUpdate,
    PreErase,
    PostErase,
    PreLoad,
    PostLoad
};

enum class DatabaseType {
    MySQL,
    PostgreSQL,
    SQLite,
    Oracle,
    MSSQL,
    Generic
};

enum class InheritanceStrategy {
    None,
    TablePerClass,
    TablePerSubclass,
    Polymorphic
};

enum class CascadeType {
    None,
    Persist,
    Update,
    Delete,
    All
};

//----------------------------------------------
// Container mapping options
//----------------------------------------------
struct ContainerOptions {
    bool is_container = false;
    std::string container_type; // vector, list, set, map, multiset, multimap, etc.
    std::optional<std::string> table_name;
    std::optional<std::string> id_column;
    std::optional<std::string> index_column;
    std::optional<std::string> value_column;
    std::optional<std::string> key_column; // for maps
    std::optional<size_t> capacity;
    bool is_unordered = false;
    bool is_ordered = false;
    std::optional<std::string> order_by;
};

//----------------------------------------------
// Callback information
//----------------------------------------------
struct CallbackInfo {
    CallbackType type;
    std::string function_name;
    bool is_const = false;
    std::optional<std::string> condition;
};

//----------------------------------------------
// Type mapping options
//----------------------------------------------
struct TypeMapping {
    std::optional<std::string> value_type; // for wrapper types
    std::optional<std::string> null_value;
    std::optional<std::string> null_default;
    bool is_wrapper = false;
    bool is_composite = false;
    std::optional<std::string> discriminator; // for polymorphic types
    std::optional<std::string> discriminator_value;
};

//----------------------------------------------
// Query and view support
//----------------------------------------------
struct QueryInfo {
    std::string name;
    std::string query;
    std::vector<std::string> parameters;
    bool is_native = false;
    std::optional<std::string> result_type;
};

struct ViewInfo {
    std::string name;
    std::string definition;
    std::vector<std::string> columns;
    bool is_materialized = false;
};

//----------------------------------------------
// Session options
//----------------------------------------------
struct SessionOptions {
    bool session_required = false;
    std::optional<std::string> session_type;
    bool thread_safe = false;
    std::optional<std::string> connection_factory;
};

//----------------------------------------------
// Index definition
//----------------------------------------------
struct IndexInfo {
    std::string name;
    std::vector<std::string> columns;
    bool is_unique = false;
    bool is_partial = false;
    std::optional<std::string> condition; // for partial indexes
    std::optional<std::string> method; // btree, hash, etc.
    std::optional<std::string> tablespace;
};

//----------------------------------------------
// Constraint definition
//----------------------------------------------
struct ConstraintInfo {
    std::string name;
    std::string type; // check, unique_together, etc.
    std::vector<std::string> columns;
    std::optional<std::string> condition;
    std::optional<std::string> deferrable;
};

//----------------------------------------------
// Foreign key reference from another table
//----------------------------------------------
struct ForeignKeyRef {
    std::string referring_table;
    std::string referring_column;
    RelationshipType relationship = RelationshipType::None;
    std::optional<std::string> inverse_field_name;
    CascadeType cascade_on_delete = CascadeType::None;
    CascadeType cascade_on_update = CascadeType::None;
};

//----------------------------------------------
// Schema evolution support
//----------------------------------------------
struct SchemaVersion {
    int version = 1;
    std::vector<std::string> migration_scripts;
    std::optional<std::string> description;
    std::optional<std::string> upgrade_function;
    std::optional<std::string> downgrade_function;
};

//----------------------------------------------
// Database-specific options
//----------------------------------------------
struct DatabaseOptions {
    DatabaseType database_type = DatabaseType::Generic;
    std::optional<std::string> engine; // for MySQL (InnoDB, MyISAM, etc.)
    std::optional<std::string> charset;
    std::optional<std::string> collation;
    std::map<std::string, std::string> custom_options; // database-specific options
    std::optional<std::string> tablespace;
    std::optional<std::string> partition_by;
};

//----------------------------------------------
// Column definition (enhanced)
//----------------------------------------------
struct Column {
    std::string name;
    std::string type;

    // Basic field attributes
    bool is_primary_key = false;
    bool is_auto_increment = false;
    bool is_unique = false;
    bool is_not_null = false;
    bool is_nullable = false;
    bool is_transient = false;
    bool is_lazy = false;
    bool is_embedded = false;
    bool is_version = false;
    bool is_indexed = false;
    bool is_pointer = false;

    // Advanced ODB attributes
    bool is_abstract = false;
    bool is_session_required = false;
    bool is_inverse = false;
    bool is_readonly = false;
    bool is_optimistic_lock = false;
    bool is_discriminator = false;
    
    // Container support
    ContainerOptions container;
    
    // Type mapping
    TypeMapping type_mapping;

    // Overrides and metadata
    std::optional<std::string> default_value;
    enum class DefaultKind { None, Bool, Int, Float, String, Expression };
    DefaultKind default_kind = DefaultKind::None;
    std::optional<long long> default_int;
    std::optional<double> default_float;
    std::optional<bool> default_bool;

    std::optional<std::string> cpp_type_override;
    std::optional<std::string> db_type_override;
    std::optional<std::string> column_name_override;
    std::optional<std::string> access_modifier; // public, private, protected
    std::optional<std::string> comment;
    std::optional<std::string> documentation;

    // Size and precision
    std::optional<size_t> max_length;
    std::optional<int> precision;
    std::optional<int> scale;

    // Relationship
    RelationshipType relationship = RelationshipType::None;
    std::optional<std::string> related_table;
    std::optional<std::string> related_column;
    std::optional<std::string> inverse_field;
    std::optional<std::string> join_table; // for many-to-many
    std::optional<std::string> join_column;
    std::optional<std::string> inverse_join_column;

    // Foreign key options
    bool is_foreign_key = false;
    std::optional<std::string> fk_name;
    std::optional<std::string> on_delete;
    std::optional<std::string> on_update;
    CascadeType cascade_type = CascadeType::None;

    // Fetch and caching
    std::optional<std::string> fetch_type; // eager, lazy, extra_lazy
    std::optional<std::string> cache_strategy;
    std::optional<std::string> cache_region;

    // Validation
    std::vector<std::string> validators;
    std::optional<std::string> validation_message;

    // Callbacks
    std::vector<CallbackInfo> callbacks;

    // Query support
    bool is_query_column = false;
    std::optional<std::string> formula; // computed columns
    
    // Audit trail
    bool is_created_timestamp = false;
    bool is_updated_timestamp = false;
    bool is_created_by = false;
    bool is_updated_by = false;

    // Constructors
    Column() = default;
    Column(const std::string& t, const std::string& n)
        : name(n), type(t) {}
};

//----------------------------------------------
// Table definition (enhanced)
//----------------------------------------------
struct Table {
    std::string name;
    int id = -1;

    // Inheritance
    std::optional<std::string> parent;
    InheritanceStrategy inheritance_strategy = InheritanceStrategy::None;
    bool is_polymorphic = false;
    std::optional<std::string> discriminator_column;
    std::optional<std::string> discriminator_value;
    std::optional<std::string> discriminator_type;

    // Output control
    std::optional<std::string> custom_class_name;
    std::optional<std::string> table_name_override;
    std::optional<std::string> schema_name;
    std::optional<std::string> catalog_name;
    bool is_view = false;
    bool is_abstract = false;
    bool is_immutable = false;

    // Session and transaction
    SessionOptions session_options;
    bool requires_transaction = false;

    // Database options
    DatabaseOptions database_options;

    // Contents
    std::vector<Column> columns;
    std::vector<ForeignKeyRef> referenced_by;

    // Advanced indexing and constraints
    std::vector<IndexInfo> indexes;
    std::vector<std::vector<std::string>> composite_indexes; // kept for backward compatibility
    std::vector<ConstraintInfo> constraints;
    std::vector<std::string> unique_together; // groups of columns that must be unique together

    // Query and view support
    std::vector<QueryInfo> named_queries;
    std::vector<ViewInfo> views;
    std::optional<std::string> default_query;

    // Callbacks (table-level)
    std::vector<CallbackInfo> callbacks;

    // Caching
    bool is_cacheable = false;
    std::optional<std::string> cache_strategy; // read_only, nonstrict_read_write, read_write, transactional
    std::optional<std::string> cache_region;
    std::optional<int> cache_timeout;

    // Auditing
    bool is_audited = false;
    std::optional<std::string> audit_table_name;
    std::vector<std::string> audited_columns;

    // Partitioning
    std::optional<std::string> partition_strategy;
    std::optional<std::string> partition_column;
    std::vector<std::string> partition_values;

    // Security
    std::vector<std::string> read_roles;
    std::vector<std::string> write_roles;
    std::vector<std::string> delete_roles;

    // Schema evolution
    SchemaVersion schema_version;

    // Custom SQL
    std::optional<std::string> custom_create_sql;
    std::optional<std::string> custom_drop_sql;
    std::vector<std::string> custom_ddl_statements;

    // Metadata
    std::optional<std::string> description;
    std::optional<std::string> documentation;
    std::map<std::string, std::string> custom_properties;

    // Generation options
    bool generate_equals = true;
    bool generate_hash = true;
    bool generate_toString = true;
    bool generate_copy_constructor = true;
    bool generate_move_constructor = true;
    bool generate_assignment_operator = true;
    bool generate_default_constructor = true;

    // Constructors
    Table() = default;
    Table(const std::string& n, int i, const std::string& p = "")
        : name(n), id(i), parent(p.empty() ? std::nullopt : std::make_optional(p)) {}
};

//----------------------------------------------
// Database schema definition
//----------------------------------------------
struct Schema {
    std::string name;
    std::string version;
    DatabaseOptions database_options;
    std::vector<Table> tables;
    std::vector<ViewInfo> global_views;
    std::vector<QueryInfo> global_queries;
    std::vector<std::string> custom_types;
    std::vector<std::string> initialization_scripts;
    std::map<std::string, std::string> global_properties;
    
    Schema() = default;
    Schema(const std::string& n, const std::string& v = "1.0") 
        : name(n), version(v) {}
};

}  // namespace parser