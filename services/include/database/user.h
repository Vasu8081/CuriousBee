#pragma once
// Generated ODB header file
// Table: User

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>
#include <database/db_object.h>

#include <odb/core.hxx>
#include <odb/database.hxx>


namespace model {

class Entitlement;
class Group;


#pragma db object table("user")

/**
 * User entity class
 */
class User : public db_object {
public:
    // Constructors
    User();
    User(const User& other);
    User(User&& other) noexcept;
    ~User();

    // Assignment operators
    User& operator=(const User& other);
    User& operator=(User&& other) noexcept;

    // Accessors
    const long long& getId() const { return id_; }
    void setId(const long long& value) { id_ = value; }

    const std::string& getUsername() const { return username_; }
    void setUsername(const std::string& value) { username_ = value; }

    const std::string& getEmail() const { return email_; }
    void setEmail(const std::string& value) { email_ = value; }

    const std::string& getPasswordhash() const { return password_hash_; }
    void setPasswordhash(const std::string& value) { password_hash_ = value; }

    const bool& getIsactive() const { return is_active_; }
    void setIsactive(const bool& value) { is_active_ = value; }

    const std::vector<std::shared_ptr<Group>>& getGroups() const { return groups_; }
    void setGroups(const std::vector<std::shared_ptr<Group>>& value) { groups_ = value; }

    const std::vector<std::shared_ptr<Entitlement>>& getEntitlements() const { return entitlements_; }
    void setEntitlements(const std::vector<std::shared_ptr<Entitlement>>& value) { entitlements_ = value; }

    // Equality operators
    bool operator==(const User& other) const;
    bool operator!=(const User& other) const;

private:
    #pragma db member id auto
    long long id_;

    #pragma db member not_null unique
    std::string username_;

    #pragma db member not_null unique
    std::string email_;

    #pragma db member not_null
    std::string password_hash_;

    #pragma db member not_null default(true)
    bool is_active_ = true;

    #pragma db member
    #pragma db value_type("std::vector<std::shared_ptr<Group>>") table("user_groups") id_column("user_id") value_column("group_id")
    std::vector<std::shared_ptr<Group>> groups_;

    #pragma db member
    #pragma db value_type("std::vector<std::shared_ptr<Entitlement>>") table("user_entitlements") id_column("user_id") value_column("entitlement_id")
    std::vector<std::shared_ptr<Entitlement>> entitlements_;

};

} // namespace model

