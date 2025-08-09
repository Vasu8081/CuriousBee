#pragma once

// Generated ODB header file
// Table: Entitlement
// DO NOT MODIFY - This file is auto-generated

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>

#include <odb/core.hxx>
#include <database/db_object.h>

namespace model {
#pragma db object table("entitlement")
/**
 * Entitlement entity class
 */
class Entitlement : public db_object {
    friend class odb::access;

public:
    // Constructors
    Entitlement();
    Entitlement(const Entitlement& other);
    Entitlement(Entitlement&& other) noexcept;
    ~Entitlement();

    // Assignment operators
    Entitlement& operator=(const Entitlement& other);
    Entitlement& operator=(Entitlement&& other) noexcept = default;

    // Accessors
    const long long& getId() const { return id_; }
    void setId(const long long& value) { id_ = value; }

    const std::string& getCode() const { return code_; }
    void setCode(const std::string& value) { code_ = value; }

    const std::optional<std::string>& getDescription() const { return description_; }
    void setDescription(const std::optional<std::string>& value) { description_ = value; }

    // Equality operators
    bool operator==(const Entitlement& other) const;
    bool operator!=(const Entitlement& other) const;

private:
    #pragma db member id auto column("id") type("BIGINT")
    long long id_;

    #pragma db member column("code") type("VARCHAR(50)") not_null unique
    std::string code_;

    #pragma db member column("description") type("TEXT") null
    std::optional<std::string> description_;

};
} // namespace model
