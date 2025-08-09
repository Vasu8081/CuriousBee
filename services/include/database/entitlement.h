#pragma once

// Generated ODB header file
// Table: Entitlement
// DO NOT MODIFY - This file is auto-generated

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <boost/optional.hpp>
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
    Entitlement() = default;
    Entitlement(const Entitlement& other) = default;
    Entitlement(Entitlement&& other) noexcept = default;
    ~Entitlement() = default;

    // Assignment operators
    Entitlement& operator=(const Entitlement& other) = default;
    Entitlement& operator=(Entitlement&& other) noexcept = default;

    // Accessors
    const long long& getId() const { return id_; }
    void setId(const long long& value) { id_ = value; }

    const std::string& getCode() const { return code_; }
    void setCode(const std::string& value) { code_ = value; }

    const boost::optional<std::string>& getDescription() const { return description_; }
    void setDescription(const boost::optional<std::string>& value) { description_ = value; }

    // Equality operators (inline)
    bool operator==(const Entitlement& other) const {
        return (id_ == other.id_) && (code_ == other.code_) && (description_ == other.description_);
    }
    bool operator!=(const Entitlement& other) const { return !(*this == other); }

private:
    #pragma db member id auto column("id") type("BIGINT")
    long long id_;

    #pragma db member column("code") type("VARCHAR(50)") not_null unique
    std::string code_;

    #pragma db member column("description") type("TEXT") null
    boost::optional<std::string> description_;

};
} // namespace model
