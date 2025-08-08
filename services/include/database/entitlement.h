#pragma once
// Generated ODB header file
// Table: Entitlement

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>
#include <database/db_object.h>

#include <odb/core.hxx>
#include <odb/database.hxx>


namespace model {


#pragma db object table("entitlement")

/**
 * Entitlement entity class
 */
class Entitlement : public db_object {
public:
    // Constructors
    Entitlement();
    Entitlement(const Entitlement& other);
    Entitlement(Entitlement&& other) noexcept;
    ~Entitlement();

    // Assignment operators
    Entitlement& operator=(const Entitlement& other);
    Entitlement& operator=(Entitlement&& other) noexcept;

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
    #pragma db member id auto
    long long id_;

    #pragma db member not_null unique
    std::string code_;

    #pragma db member
    std::optional<std::string> description_;

};

} // namespace model

