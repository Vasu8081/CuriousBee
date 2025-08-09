#pragma once

// Generated ODB header file
// Table: User
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
// Forward declarations
class Entitlement;
class Group;

#pragma db object table("user")
/**
 * User entity class
 */
class User : public db_object {
    friend class odb::access;

public:
    // Constructors
    User() = default;
    User(const User& other) = default;
    User(User&& other) noexcept = default;
    ~User() = default;

    // Assignment operators
    User& operator=(const User& other) = default;
    User& operator=(User&& other) noexcept = default;

    // Accessors
    const long long& getId() const { return id_; }
    void setId(const long long& value) { id_ = value; }

    const std::string& getUsername() const { return username_; }
    void setUsername(const std::string& value) { username_ = value; }

    const std::string& getEmail() const { return email_; }
    void setEmail(const std::string& value) { email_ = value; }

    const std::string& getPasswordHash() const { return password_hash_; }
    void setPasswordHash(const std::string& value) { password_hash_ = value; }

    const bool& getIsActive() const { return is_active_; }
    void setIsActive(const bool& value) { is_active_ = value; }

    const std::vector<std::shared_ptr<Group>>& getGroups() const { return groups_; }
    void setGroups(const std::vector<std::shared_ptr<Group>>& value) { groups_ = value; }

    const std::vector<std::shared_ptr<Entitlement>>& getEntitlements() const { return entitlements_; }
    void setEntitlements(const std::vector<std::shared_ptr<Entitlement>>& value) { entitlements_ = value; }

    // Equality operators (inline)
    bool operator==(const User& other) const {
        return (id_ == other.id_) && (username_ == other.username_) && (email_ == other.email_) && (password_hash_ == other.password_hash_) && (is_active_ == other.is_active_);
    }
    bool operator!=(const User& other) const { return !(*this == other); }

private:
    #pragma db member id auto column("id") type("BIGINT")
    long long id_;

    #pragma db member column("username") type("VARCHAR(50)") not_null unique
    std::string username_;

    #pragma db member column("email") type("VARCHAR(100)") not_null unique
    std::string email_;

    #pragma db member column("password_hash") type("VARCHAR(255)") not_null
    std::string password_hash_;

    #pragma db member column("is_active") type("BOOLEAN") not_null default(true)
    bool is_active_;

    #pragma db member table("user_groups") id_column("user_id") value_column("group_id")
    std::vector<std::shared_ptr<Group>> groups_;

    #pragma db member table("user_entitlements") id_column("user_id") value_column("entitlement_id")
    std::vector<std::shared_ptr<Entitlement>> entitlements_;

};
} // namespace model
