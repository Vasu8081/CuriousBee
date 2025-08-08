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
} // namespace model

// Explicit template instantiations for your concrete types if needed.
// template class model::Database;  // Not always necessary, unless linker issues arise.