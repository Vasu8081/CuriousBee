#ifndef GENERATED_GROUP_H
#define GENERATED_GROUP_H

// Generated ODB header file
// Table: Group

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>

#include <odb/core.hxx>
#include <odb/database.hxx>

namespace model {

class User;
class Entitlement;

#pragma db object table("group")

/**
 * Group entity class
 */
class Group {
public:
    // Constructors
    Group();
    Group(const Group& other);
    Group(Group&& other) noexcept;
    ~Group();

    // Assignment operators
    Group& operator=(const Group& other);
    Group& operator=(Group&& other) noexcept;

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
    #pragma db member id auto
    long long id_;

    #pragma db member not_null unique
    std::string name_;

    #pragma db member
    std::optional<std::string> description_;

    #pragma db member
    #pragma db value_type("std::vector<std::shared_ptr<User>>") table("user_groups")
    std::vector<std::shared_ptr<User>> users_;

    #pragma db member
    #pragma db value_type("std::vector<std::shared_ptr<Entitlement>>") table("group_entitlements") id_column("group_id") value_column("entitlement_id")
    std::vector<std::shared_ptr<Entitlement>> entitlements_;

};

} // namespace model

#endif // GENERATED_GROUP_H
