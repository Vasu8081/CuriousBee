#pragma once

#include <memory>
#include <string>
#include <vector>
#include <database/models.h>

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

    // returns std::shared_ptr<T>, copying from ODB’s pointer (usually T*)
    template<typename T>
    std::shared_ptr<T> find_by_id(long long id) {
        static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
        odb::transaction t(db_->begin());
        auto p = db_->find<T>(id);                 // ODB pointer (T* by default)
        t.commit();
        if (!p) return nullptr;
        return std::make_shared<T>(*p);            // copy into shared_ptr for uniform API
    }

    // Preferred, type-safe “find by”
    template<typename T, typename Expr>
    std::vector<std::shared_ptr<T>> query_where(const Expr& e) {
        static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
        std::vector<std::shared_ptr<T>> out;
        odb::transaction t(db_->begin());
        odb::result<T> r = db_->query<T>(e);       // e.g., (Q::email == "a@b.com")
        for (const T& v : r) out.emplace_back(std::make_shared<T>(v));
        t.commit();
        return out;
    }

    template<typename T, typename Expr>
    std::shared_ptr<T> find_one_where(const Expr& e) {
        auto v = query_where<T>(e);
        return v.empty() ? nullptr : v.front();
    }

    // Convenience: “find_by(column == value)”
    template<typename T, typename Col, typename V>
    std::vector<std::shared_ptr<T>> find_by(const Col& col, const V& value) {
        return query_where<T>(col == value);
    }

    template<typename T, typename Col, typename V>
    std::shared_ptr<T> find_one_by(const Col& col, const V& value) {
        return find_one_where<T>(col == value);
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> find_where_sql(const std::string& where_sql) {
        std::vector<std::shared_ptr<T>> out;
        odb::transaction t(db_->begin());
        odb::result<T> r = db_->query<T>(odb::query<T>(where_sql)); // single string
        for (const T& v : r) out.emplace_back(std::make_shared<T>(v));
        t.commit();
        return out;
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