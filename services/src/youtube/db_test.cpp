// main.cpp

#include <iostream>
#include <database/database.h>
#include <database/user.h>

int main() {
    try {
        model::Database db(
            "curioususer",
            "curiouspass",
            "curiousbytes",
            "localhost",
            5432
        );

        std::cout << "Database connection established successfully!\n";

        // Optional: Test a CRUD operation (list all users)
        auto users = db.get_all<model::User>();
        std::cout << "Fetched " << users.size() << " users from the database.\n";
        for (const auto& user : users) {
            std::cout << "User: " << user->getUsername() << std::endl;
        }

    } catch (const std::exception& ex) {
        std::cerr << "Database connection failed: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}