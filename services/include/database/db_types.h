#pragma once

#include <optional>
#include <string>
#include <memory>
#include <odb/core.hxx>

// For std::optional types, use per-member type specification:
// #pragma db member column("name") type("TEXT")
// 
// For relationship containers with shared_ptr, use per-member value_type specification:
// #pragma db member table("table_name") id_column("id") value_column("ref_id") value_type("BIGINT")

// Forward declarations for model classes
namespace model {
    class User;
    class Group; 
    class Entitlement;
}