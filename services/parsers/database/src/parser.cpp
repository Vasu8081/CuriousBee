#include <parsers/database/odb_generator.h>
#include <iostream>

using namespace parser;

int main() {
    // Create the generator
    ODBGenerator generator("./generated_models");
    generator.setNamespace("models");
    generator.setIncludeGuardPrefix("MODELS_");
    
    // Create a sample schema
    Schema schema("UserManagement", "1.0");
    schema.database_options.database_type = DatabaseType::MySQL;
    schema.database_options.charset = "utf8mb4";
    schema.database_options.collation = "utf8mb4_unicode_ci";
    
    // Create User table
    Table user_table("users", 1);
    user_table.description = "User account information";
    user_table.is_cacheable = true;
    user_table.cache_strategy = "read_write";
    
    // Add columns to user table
    Column id_col("bigint", "id");
    id_col.is_primary_key = true;
    id_col.is_auto_increment = true;
    id_col.is_not_null = true;
    user_table.columns.push_back(id_col);
    
    Column username_col("varchar", "username");
    username_col.max_length = 50;
    username_col.is_unique = true;
    username_col.is_not_null = true;
    username_col.is_indexed = true;
    user_table.columns.push_back(username_col);
    
    Column email_col("varchar", "email");
    email_col.max_length = 255;
    email_col.is_unique = true;
    email_col.is_not_null = true;
    email_col.validators.push_back("email_format");
    user_table.columns.push_back(email_col);
    
    Column password_col("varchar", "password_hash");
    password_col.max_length = 255;
    password_col.is_not_null = true;
    password_col.access_modifier = "private";
    user_table.columns.push_back(password_col);
    
    Column created_at_col("timestamp", "created_at");
    created_at_col.is_created_timestamp = true;
    created_at_col.default_value = "CURRENT_TIMESTAMP";
    user_table.columns.push_back(created_at_col);
    
    Column updated_at_col("timestamp", "updated_at");
    updated_at_col.is_updated_timestamp = true;
    updated_at_col.default_value = "CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP";
    user_table.columns.push_back(updated_at_col);
    
    // Add callbacks
    CallbackInfo pre_persist_callback;
    pre_persist_callback.type = CallbackType::PrePersist;
    pre_persist_callback.function_name = "before_save";
    user_table.callbacks.push_back(pre_persist_callback);
    
    CallbackInfo post_load_callback;
    post_load_callback.type = CallbackType::PostLoad;
    post_load_callback.function_name = "after_load";
    post_load_callback.is_const = true;
    user_table.callbacks.push_back(post_load_callback);
    
    // Add indexes
    IndexInfo username_idx;
    username_idx.name = "idx_username";
    username_idx.columns = {"username"};
    username_idx.is_unique = true;
    user_table.indexes.push_back(username_idx);
    
    // Add to schema
    schema.tables.push_back(user_table);
    
    // Create Profile table with relationship
    Table profile_table("profiles", 2);
    profile_table.description = "User profile information";
    
    // Profile columns
    Column profile_id_col("bigint", "id");
    profile_id_col.is_primary_key = true;
    profile_id_col.is_auto_increment = true;
    profile_table.columns.push_back(profile_id_col);
    
    Column user_id_col("bigint", "user_id");
    user_id_col.is_foreign_key = true;
    user_id_col.is_not_null = true;
    user_id_col.relationship = RelationshipType::OneToOne;
    user_id_col.related_table = "users";
    user_id_col.related_column = "id";
    user_id_col.on_delete = "CASCADE";
    profile_table.columns.push_back(user_id_col);
    
    Column first_name_col("varchar", "first_name");
    first_name_col.max_length = 100;
    first_name_col.is_nullable = true;
    profile_table.columns.push_back(first_name_col);
    
    Column last_name_col("varchar", "last_name");
    last_name_col.max_length = 100;
    last_name_col.is_nullable = true;
    profile_table.columns.push_back(last_name_col);
    
    Column bio_col("text", "bio");
    bio_col.is_nullable = true;
    profile_table.columns.push_back(bio_col);
    
    // Add to schema
    schema.tables.push_back(profile_table);
    
    // Create Post table with container relationships
    Table post_table("posts", 3);
    post_table.description = "User blog posts";
    
    // Post columns
    Column post_id_col("bigint", "id");
    post_id_col.is_primary_key = true;
    post_id_col.is_auto_increment = true;
    post_table.columns.push_back(post_id_col);
    
    Column post_user_id_col("bigint", "user_id");
    post_user_id_col.is_foreign_key = true;
    post_user_id_col.relationship = RelationshipType::ManyToOne;
    post_user_id_col.related_table = "users";
    post_user_id_col.related_column = "id";
    post_user_id_col.inverse_field = "posts";
    post_table.columns.push_back(post_user_id_col);
    
    Column title_col("varchar", "title");
    title_col.max_length = 255;
    title_col.is_not_null = true;
    post_table.columns.push_back(title_col);
    
    Column content_col("text", "content");
    content_col.is_not_null = true;
    post_table.columns.push_back(content_col);
    
    // Tags as container (many-to-many)
    Column tags_col("varchar", "tags");
    tags_col.container.is_container = true;
    tags_col.container.container_type = "vector";
    tags_col.container.table_name = "post_tags";
    tags_col.relationship = RelationshipType::ManyToMany;
    tags_col.join_table = "post_tags";
    post_table.columns.push_back(tags_col);
    
    Column post_created_at_col("timestamp", "created_at");
    post_created_at_col.is_created_timestamp = true;
    post_created_at_col.default_value = "CURRENT_TIMESTAMP";
    post_table.columns.push_back(post_created_at_col);
    
    // Add to schema
    schema.tables.push_back(post_table);
    
    // Create abstract base class example
    Table auditable_table("auditable", 4);
    auditable_table.is_abstract = true;
    auditable_table.description = "Base class for auditable entities";
    
    Column audit_created_by_col("bigint", "created_by");
    audit_created_by_col.is_created_by = true;
    audit_created_by_col.is_nullable = true;
    auditable_table.columns.push_back(audit_created_by_col);
    
    Column audit_updated_by_col("bigint", "updated_by");
    audit_updated_by_col.is_updated_by = true;
    audit_updated_by_col.is_nullable = true;
    auditable_table.columns.push_back(audit_updated_by_col);
    
    Column audit_created_at_col("timestamp", "created_at");
    audit_created_at_col.is_created_timestamp = true;
    audit_created_at_col.default_value = "CURRENT_TIMESTAMP";
    auditable_table.columns.push_back(audit_created_at_col);
    
    Column audit_updated_at_col("timestamp", "updated_at");
    audit_updated_at_col.is_updated_timestamp = true;
    audit_updated_at_col.default_value = "CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP";
    auditable_table.columns.push_back(audit_updated_at_col);
    
    // Add to schema
    schema.tables.push_back(auditable_table);
    
    // Create inherited table
    Table document_table("documents", 5);
    document_table.parent = "auditable";
    document_table.inheritance_strategy = InheritanceStrategy::TablePerClass;
    document_table.description = "Document management with audit trail";
    
    Column doc_id_col("bigint", "id");
    doc_id_col.is_primary_key = true;
    doc_id_col.is_auto_increment = true;
    document_table.columns.push_back(doc_id_col);
    
    Column doc_name_col("varchar", "name");
    doc_name_col.max_length = 255;
    doc_name_col.is_not_null = true;
    document_table.columns.push_back(doc_name_col);
    
    Column doc_path_col("varchar", "file_path");
    doc_path_col.max_length = 500;
    doc_path_col.is_not_null = true;
    document_table.columns.push_back(doc_path_col);
    
    Column doc_size_col("bigint", "file_size");
    doc_size_col.default_value = "0";
    document_table.columns.push_back(doc_size_col);
    
    // Add to schema
    schema.tables.push_back(document_table);
    
    // Generate the schema
    LOG_INFO << "Starting ODB schema generation..." << go;
    
    if (generator.generateSchema(schema)) {
        LOG_INFO << "ODB schema generation completed successfully!" << go;
        std::cout << "Generated files in ./generated_models directory:\n";
        std::cout << "- users.h / users.cpp\n";
        std::cout << "- profiles.h / profiles.cpp\n";
        std::cout << "- posts.h / posts.cpp\n";
        std::cout << "- auditable.h / auditable.cpp\n";
        std::cout << "- documents.h / documents.cpp\n";
    } else {
        LOG_ERR << "ODB schema generation failed!" << go;
        return 1;
    }
    
    // Example of generating a single table
    LOG_INFO << "Generating single table example..." << go;
    
    Table simple_table("simple_entity", 6);
    simple_table.description = "Simple entity for testing";
    
    Column simple_id("int", "id");
    simple_id.is_primary_key = true;
    simple_id.is_auto_increment = true;
    simple_table.columns.push_back(simple_id);
    
    Column simple_name("varchar", "name");
    simple_name.max_length = 100;
    simple_name.is_not_null = true;
    simple_table.columns.push_back(simple_name);
    
    if (generator.generateTable(simple_table, schema.database_options)) {
        LOG_INFO << "Single table generation completed!" << go;
        std::cout << "Generated simple_entity.h / simple_entity.cpp\n";
    } else {
        LOG_ERR << "Single table generation failed!" << go;
    }
    
    return 0;
}