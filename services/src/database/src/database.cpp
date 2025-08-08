#include <database/database.h>

namespace model {

Database::Database(
    const std::string& user,
    const std::string& password,
    const std::string& dbname,
    const std::string& host,
    unsigned int port
) : db_(std::make_unique<odb::pgsql::database>(
    user, password, dbname, host, port)) {}

template<typename T>
void Database::add_update(std::shared_ptr<T>& obj) {
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
void Database::delete_by_id(long long id) {
    static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
    odb::transaction t(db_->begin());
    db_->erase<T>(id);
    t.commit();
}

template<typename T>
std::shared_ptr<T> Database::find_by_id(long long id) {
    static_assert(std::is_base_of<db_object, T>::value, "T must inherit from db_object");
    odb::transaction t(db_->begin());
    std::shared_ptr<T> obj = db_->find<T>(id);
    t.commit();
    return obj;
}

// Column query
template<typename T, typename V>
std::vector<std::shared_ptr<T>> Database::find_by_column(const std::string& column, const V& value) {
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
std::shared_ptr<T> Database::find_one_by_column(const std::string& column, const V& value) {
    auto vec = find_by_column<T, V>(column, value);
    if (!vec.empty())
        return vec.front();
    return nullptr;
}

template<typename T>
std::vector<std::shared_ptr<T>> Database::get_all() {
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
void Database::transaction(Func&& func) {
    odb::transaction t(db_->begin());
    func(*db_);
    t.commit();
}

} // namespace model

// Explicit template instantiations for your concrete types if needed.
// template class model::Database;  // Not always necessary, unless linker issues arise.