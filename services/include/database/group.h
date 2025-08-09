#pragma once

// Generated ODB header file
// Table: Group
// DO NOT MODIFY - This file is auto-generated

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>

#include <odb/core.hxx>
#include <database/db_object.h>

namespace model {
// Forward declarations
class Entitlement;
class User;

#pragma db object table("group")
/**
 * Group entity class
 */
class Group : public db_object {
    friend class odb::access;

public:
    // Constructors
    Group();
    Group(const Group& other);
    Group(Group&& other) noexcept;
    ~Group();

    // Assignment operators
    Group& operator=(const Group& other);
    Group& operator=(Group&& other) noexcept = default;

    // Accessors
    const long long& getId() const { return id_; }
    void setId(const long long& value) { id_ = value; }

    const std::string& getName() const { return name_; }
    void setName(const std::string& value) { name_ = value; }

    const std::optional<std::string>& getDescription() const { return description_; }
    void setDescription(const std::optional<std::string>& value) { description_ = value; }

    const std::vector<std::shared_ptr<User>>& getUsers() const { return users_; }
    void setUsers(const std::vector<std::shared_ptr<User>>& value) { users_ = value; }

    const std::vector<std::shared_ptr<Entitlement>>& getEntitlements() const { return entitlements_; }
    void setEntitlements(const std::vector<std::shared_ptr<Entitlement>>& value) { entitlements_ = value; }

    // Equality operators
    bool operator==(const Group& other) const;
    bool operator!=(const Group& other) const;

private:
    #pragma db member id auto column("id") type("BIGINT")
    long long id_;

    #pragma db member column("name") type("VARCHAR(50)") not_null unique
    std::string name_;

    #pragma db member column("description") type("TEXT") null
    std::optional<std::string> description_;

    #pragma db member inverse(groups_)
    std::vector<std::shared_ptr<User>> users_;

    #pragma db member table("group_entitlements") id_column("group_id") value_column("entitlement_id")
    std::vector<std::shared_ptr<Entitlement>> entitlements_;

};
} // namespace model
