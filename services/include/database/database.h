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

    // // Generic upsert (insert if id not set, else update)
    // template<typename T>
    // void add_update(std::shared_ptr<T>& obj);

    // template<typename T>
    // void delete_by_id(long long id);

    // // Generic get by id
    // template<typename T>
    // std::shared_ptr<T> find_by_id(long long id);

    // // Find by a column, e.g. username/email; returns ALL matches
    // template<typename T, typename V>
    // std::vector<std::shared_ptr<T>> find_by_column(const std::string& column, const V& value);

    // // Find one by a column; returns the first match or nullptr
    // template<typename T, typename V>
    // std::shared_ptr<T> find_one_by_column(const std::string& column, const V& value);

    // // List all rows
    // template<typename T>
    // std::vector<std::shared_ptr<T>> get_all();

    // // Transaction utility (optional, for bulk ops)
    // template<typename Func>
    // void transaction(Func&& func);

    template<typename T>
    void add_update(std::shared_ptr<T>& obj) {
        static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
        odb::transaction t(db_->begin());
        if (obj->getId() == 0) {
            db_->persist(obj); // insert
        } else {
            db_->update(obj); // update
        }
        t.commit();
    }

    template<typename T>
    void delete_by_id(long long id) {
        static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
        odb::transaction t(db_->begin());
        db_->erase<T>(id);
        t.commit();
    }

    template<typename T>
    std::shared_ptr<T> find_by_id(long long id) {
        static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
        odb::transaction t(db_->begin());
        std::shared_ptr<T> obj = db_->find<T>(id);
        t.commit();
        return obj;
    }

    // Column query
    template<typename T, typename V>
    std::vector<std::shared_ptr<T>> find_by_column(const std::string& column, const V& value) {
        static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
        using query = odb::query<T>;
        using result = odb::result<T>;

        std::vector<std::shared_ptr<T>> results;
        odb::transaction t(db_->begin());
        std::string qstr = column + " = :value";
        result r(db_->query<T>(query(qstr, value)));
        for (auto it = r.begin(); it != r.end(); ++it)
            results.push_back(std::make_shared<T>(*it));
        t.commit();
        return results;
    }

    template<typename T, typename V>
    std::shared_ptr<T> find_one_by_column(const std::string& column, const V& value) {
        auto vec = find_by_column<T, V>(column, value);
        if (!vec.empty())
            return vec.front();
        return nullptr;
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> get_all() {
        static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
        std::vector<std::shared_ptr<T>> results;
        odb::transaction t(db_->begin());
        odb::result<T> r(db_->query<T>());
        for (auto it = r.begin(); it != r.end(); ++it)
            results.push_back(std::make_shared<T>(*it));
        t.commit();
        return results;
    }

    // Transaction utility
    template<typename Func>
    void transaction(Func&& func) {
        odb::transaction t(db_->begin());
        func(*db_);
        t.commit();
    }

private:
    std::unique_ptr<odb::database> db_;
};

} // namespace model