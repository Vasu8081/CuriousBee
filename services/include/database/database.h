#pragma once

#include <memory>
#include <string>
#include <vector>
#include <odb/database.hxx>
#include <odb/pgsql/database.hxx>
#include <odb/transaction.hxx>
#include <odb/query.hxx>
#include <odb/result.hxx>
#include <database/db_object.h>

namespace model {

class Database {
public:
    Database(
        const std::string& user,
        const std::string& password,
        const std::string& dbname,
        const std::string& host = "localhost",
        unsigned int port = 5432
    );

    // Generic upsert (insert if id not set, else update)
    template<typename T>
    void add_update(std::shared_ptr<T>& obj);

    template<typename T>
    void delete_by_id(long long id);

    // Generic get by id
    template<typename T>
    std::shared_ptr<T> find_by_id(long long id);

    // Find by a column, e.g. username/email; returns ALL matches
    template<typename T, typename V>
    std::vector<std::shared_ptr<T>> find_by_column(const std::string& column, const V& value);

    // Find one by a column; returns the first match or nullptr
    template<typename T, typename V>
    std::shared_ptr<T> find_one_by_column(const std::string& column, const V& value);

    // List all rows
    template<typename T>
    std::vector<std::shared_ptr<T>> get_all();

    // Transaction utility (optional, for bulk ops)
    template<typename Func>
    void transaction(Func&& func);

private:
    std::unique_ptr<odb::database> db_;
};

} // namespace model