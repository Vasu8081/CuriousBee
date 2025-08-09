// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <filesystem>

#include <database/database.h>
#include <database/models.h>

using std::string;
namespace fs = std::filesystem;

// ---------- small utils ----------
static std::string trim(std::string s) {
    auto notspace = [](int ch){ return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notspace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notspace).base(), s.end());
    return s;
}

static std::vector<std::string> split_csv(const std::string& s) {
    std::vector<std::string> out;
    std::string token;
    std::istringstream iss(s);
    while (std::getline(iss, token, ',')) out.push_back(trim(token));
    if (out.size()==1 && out[0].empty()) out.clear();
    return out;
}

static long long to_ll(const std::string& s, long long def = 0) {
    try { return std::stoll(s); } catch (...) { return def; }
}

static bool to_bool(const std::string& s, bool def = false) {
    std::string v = s; std::transform(v.begin(), v.end(), v.begin(), ::tolower);
    if (v=="true" || v=="1" || v=="yes" || v=="y") return true;
    if (v=="false"|| v=="0" || v=="no"  || v=="n") return false;
    return def;
}

static std::string bool_str(bool b){ return b ? "true" : "false"; }

// Launch $EDITOR with initial text, return final text or empty on error/cancel
static std::string edit_in_editor(const std::string& initial) {
    char tmpl[] = "/tmp/dbeditXXXXXX";
    int fd = mkstemp(tmpl);
    if (fd == -1) { std::cerr << "Failed to create temp file.\n"; return {}; }
    close(fd);
    std::string path = tmpl;
    { std::ofstream ofs(path); ofs << initial; }

    const char* ed = std::getenv("VISUAL");
    if (!ed || !*ed) ed = std::getenv("EDITOR");
    if (!ed || !*ed) ed = "nano";
    std::string cmd = std::string(ed) + " " + path;
    if (std::system(cmd.c_str()) != 0) {
        if (std::system((std::string("vi ") + path).c_str()) != 0) {
            std::remove(path.c_str());
            return {};
        }
    }

    std::ifstream ifs(path);
    std::stringstream buffer; buffer << ifs.rdbuf();
    std::string result = buffer.str();
    std::remove(path.c_str());
    return result;
}

static std::map<std::string, std::string> parse_kv(const std::string& text) {
    std::map<std::string, std::string> out;
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line[0]=='#' || line[0]==';') continue;
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string k = trim(line.substr(0,pos));
        std::string v = trim(line.substr(pos+1));
        if (!v.empty() && v.front()=='"' && v.back()=='"' && v.size()>=2)
            v = v.substr(1, v.size()-2);
        out[k] = v;
    }
    return out;
}

// ---------- render templates for editing ----------
static std::string render_user(const model::User* u) {
    std::ostringstream oss;
    oss << "# Edit the fields and save. Leave empty to keep (on edit).\n";
    oss << "id = "            << (u ? std::to_string(u->getId()) : "0") << "\n";
    oss << "username = "      << (u ? u->getUsername() : "") << "\n";
    oss << "email = "         << (u ? u->getEmail() : "") << "\n";
    oss << "password_hash = " << (u ? u->getPasswordHash() : "") << "\n";
    oss << "is_active = "     << (u ? bool_str(u->getIsActive()) : "true") << "\n";
    // relationships as CSV of IDs
    oss << "groups = ";
    if (u) {
        bool first = true;
        for (auto& g : u->getGroups()) {
            if (!first) oss << ", ";
            first = false;
            oss << g->getId();
        }
    }
    oss << "\n";
    oss << "entitlements = ";
    if (u) {
        bool first = true;
        for (auto& e : u->getEntitlements()) {
            if (!first) oss << ", ";
            first = false;
            oss << e->getId();
        }
    }
    oss << "\n";
    return oss.str();
}

static std::string render_group(const model::Group* g) {
    std::ostringstream oss;
    oss << "# Edit the fields and save. Leave empty to keep (on edit).\n";
    oss << "id = "          << (g ? std::to_string(g->getId()) : "0") << "\n";
    oss << "name = "        << (g ? g->getName() : "") << "\n";
    oss << "description = " << (g && g->getDescription() ? *g->getDescription() : "") << "\n";
    oss << "users = \n"; // CSV of user ids (optional seed)
    oss << "entitlements = ";
    if (g) {
        bool first = true;
        for (auto& e : g->getEntitlements()) {
            if (!first) oss << ", ";
            first = false;
            oss << e->getId();
        }
    }
    oss << "\n";
    return oss.str();
}

static std::string render_entitlement(const model::Entitlement* e) {
    std::ostringstream oss;
    oss << "# Edit the fields and save. Leave empty to keep (on edit).\n";
    oss << "id = "          << (e ? std::to_string(e->getId()) : "0") << "\n";
    oss << "code = "        << (e ? e->getCode() : "") << "\n";
    oss << "description = " << (e && e->getDescription() ? *e->getDescription() : "") << "\n";
    return oss.str();
}

// ---------- apply edited KV to objects ----------
static void apply_user(model::Database& db, model::User& u, const std::map<std::string,std::string>& kv, bool /*is_new*/) {
    if (auto it = kv.find("username"); it != kv.end()) u.setUsername(it->second);
    if (auto it = kv.find("email"); it != kv.end()) u.setEmail(it->second);
    if (auto it = kv.find("password_hash"); it != kv.end()) u.setPasswordHash(it->second);
    if (auto it = kv.find("is_active"); it != kv.end()) u.setIsActive(to_bool(it->second, true));

    // relationships: groups, entitlements
    if (auto it = kv.find("groups"); it != kv.end()) {
        std::vector<std::shared_ptr<model::Group>> groups;
        for (auto& s : split_csv(it->second)) {
            long long gid = to_ll(s, 0);
            if (gid > 0) {
                if (auto g = db.find_by_id<model::Group>(gid)) groups.push_back(g);
            }
        }
        u.setGroups(groups);
    }
    if (auto it = kv.find("entitlements"); it != kv.end()) {
        std::vector<std::shared_ptr<model::Entitlement>> ents;
        for (auto& s : split_csv(it->second)) {
            long long eid = to_ll(s, 0);
            if (eid > 0) {
                if (auto e = db.find_by_id<model::Entitlement>(eid)) ents.push_back(e);
            }
        }
        u.setEntitlements(ents);
    }
}

static void apply_group(model::Database& db, model::Group& g, const std::map<std::string,std::string>& kv, bool /*is_new*/) {
    if (auto it = kv.find("name"); it != kv.end()) g.setName(it->second);
    if (auto it = kv.find("description"); it != kv.end()) {
        auto v = trim(it->second);
        if (v.empty()) g.setDescription(boost::none);
        else g.setDescription(v);
    }
    if (auto it = kv.find("entitlements"); it != kv.end()) {
        std::vector<std::shared_ptr<model::Entitlement>> ents;
        for (auto& s : split_csv(it->second)) {
            long long eid = to_ll(s, 0);
            if (eid > 0) {
                if (auto e = db.find_by_id<model::Entitlement>(eid)) ents.push_back(e);
            }
        }
        g.setEntitlements(ents);
    }
    if (auto it = kv.find("users"); it != kv.end()) {
        std::vector<std::shared_ptr<model::User>> users;
        for (auto& s : split_csv(it->second)) {
            long long uid = to_ll(s, 0);
            if (uid > 0) {
                if (auto u = db.find_by_id<model::User>(uid)) users.push_back(u);
            }
        }
        g.setUsers(users);
    }
}

static void apply_entitlement(model::Entitlement& e, const std::map<std::string,std::string>& kv, bool /*is_new*/) {
    if (auto it = kv.find("code"); it != kv.end()) e.setCode(it->second);
    if (auto it = kv.find("description"); it != kv.end()) {
        auto v = trim(it->second);
        if (v.empty()) e.setDescription(boost::none);
        else e.setDescription(v);
    }
}

// ---------- CRUD screens ----------
static void list_users(model::Database& db) {
    auto rows = db.get_all<model::User>();
    std::cout << "Users (" << rows.size() << "):\n";
    for (auto& u : rows) {
        std::cout << "  id=" << u->getId()
                  << " username=" << u->getUsername()
                  << " email=" << u->getEmail()
                  << " active=" << bool_str(u->getIsActive())
                  << " groups=" << u->getGroups().size()
                  << " entitlements=" << u->getEntitlements().size()
                  << "\n";
    }
}

static void list_groups(model::Database& db) {
    auto rows = db.get_all<model::Group>();
    std::cout << "Groups (" << rows.size() << "):\n";
    for (auto& g : rows) {
        std::cout << "  id=" << g->getId()
                  << " name=" << g->getName()
                  << " desc=" << (g->getDescription() ? *g->getDescription() : "")
                  << " users=" << g->getUsers().size()
                  << " entitlements=" << g->getEntitlements().size()
                  << "\n";
    }
}

static void list_entitlements(model::Database& db) {
    auto rows = db.get_all<model::Entitlement>();
    std::cout << "Entitlements (" << rows.size() << "):\n";
    for (auto& e : rows) {
        std::cout << "  id=" << e->getId()
                  << " code=" << e->getCode()
                  << " desc=" << (e->getDescription() ? *e->getDescription() : "")
                  << "\n";
    }
}

template <typename T>
static void generic_delete(model::Database& db, const char* what) {
    std::cout << "Enter " << what << " id to delete: ";
    std::string s; std::getline(std::cin, s);
    long long id = to_ll(s, 0);
    if (id <= 0) { std::cout << "Invalid id.\n"; return; }
    try {
        db.delete_by_id<T>(id);
        std::cout << "Deleted.\n";
    } catch (const std::exception& ex) {
        std::cout << "Delete failed: " << ex.what() << "\n";
    }
}

static void add_or_edit_user(model::Database& db, long long edit_id = 0) {
    std::shared_ptr<model::User> u;
    if (edit_id > 0) u = db.find_by_id<model::User>(edit_id);
    std::string text = render_user(u.get());
    auto edited = edit_in_editor(text);
    if (edited.empty()) return;
    auto kv = parse_kv(edited);

    std::shared_ptr<model::User> obj = u ? u : std::make_shared<model::User>();
    apply_user(db, *obj, kv, !u);
    db.add_update(obj);
    std::cout << (u ? "Updated" : "Created") << " user id=" << obj->getId() << "\n";
}

static void add_or_edit_group(model::Database& db, long long edit_id = 0) {
    std::shared_ptr<model::Group> g;
    if (edit_id > 0) g = db.find_by_id<model::Group>(edit_id);
    std::string text = render_group(g.get());
    auto edited = edit_in_editor(text);
    if (edited.empty()) return;
    auto kv = parse_kv(edited);

    std::shared_ptr<model::Group> obj = g ? g : std::make_shared<model::Group>();
    apply_group(db, *obj, kv, !g);
    db.add_update(obj);
    std::cout << (g ? "Updated" : "Created") << " group id=" << obj->getId() << "\n";
}

static void add_or_edit_entitlement(model::Database& db, long long edit_id = 0) {
    std::shared_ptr<model::Entitlement> e;
    if (edit_id > 0) e = db.find_by_id<model::Entitlement>(edit_id);
    std::string text = render_entitlement(e.get());
    auto edited = edit_in_editor(text);
    if (edited.empty()) return;
    auto kv = parse_kv(edited);

    std::shared_ptr<model::Entitlement> obj = e ? e : std::make_shared<model::Entitlement>();
    apply_entitlement(*obj, kv, !e);
    db.add_update(obj);
    std::cout << (e ? "Updated" : "Created") << " entitlement id=" << obj->getId() << "\n";
}

template <typename T>
static void generic_view(model::Database& db, const char* what) {
    std::cout << "Enter " << what << " id to view: ";
    std::string s; std::getline(std::cin, s);
    long long id = to_ll(s, 0);
    auto obj = db.find_by_id<T>(id);
    if (!obj) { std::cout << "Not found.\n"; return; }

    if constexpr (std::is_same_v<T, model::User>) {
        std::cout << "User{id=" << obj->getId()
                  << ", username=" << obj->getUsername()
                  << ", email=" << obj->getEmail()
                  << ", active=" << bool_str(obj->getIsActive())
                  << ", groups=" << obj->getGroups().size()
                  << ", entitlements=" << obj->getEntitlements().size()
                  << "}\n";
    } else if constexpr (std::is_same_v<T, model::Group>) {
        std::cout << "Group{id=" << obj->getId()
                  << ", name=" << obj->getName()
                  << ", description=" << (obj->getDescription() ? *obj->getDescription() : "")
                  << ", users=" << obj->getUsers().size()
                  << ", entitlements=" << obj->getEntitlements().size()
                  << "}\n";
    } else if constexpr (std::is_same_v<T, model::Entitlement>) {
        std::cout << "Entitlement{id=" << obj->getId()
                  << ", code=" << obj->getCode()
                  << ", description=" << (obj->getDescription() ? *obj->getDescription() : "")
                  << "}\n";
    }
}

// ---- “Find by” using safe SQL fragment (works with dynamic column names) ----
template <typename T>
static void generic_find_by(model::Database& db, const char* what) {
    std::cout << "Column name: ";
    std::string col; std::getline(std::cin, col);
    std::cout << "Value: ";
    std::string val; std::getline(std::cin, val);

    // simple equality; quote single quotes in value
    std::string esc; esc.reserve(val.size());
    for (char c: val) { esc += (c=='\'' ? "''" : std::string(1,c)); }
    std::string where = col + " = '" + esc + "'";

    auto rows = db.find_where_sql<T>(where);
    std::cout << "Found " << rows.size() << " " << what << "(s)\n";
    for (auto& x : rows) {
        if constexpr (std::is_same_v<T, model::User>) {
            std::cout << "  id=" << x->getId() << " username=" << x->getUsername() << "\n";
        } else if constexpr (std::is_same_v<T, model::Group>) {
            std::cout << "  id=" << x->getId() << " name=" << x->getName() << "\n";
        } else if constexpr (std::is_same_v<T, model::Entitlement>) {
            std::cout << "  id=" << x->getId() << " code=" << x->getCode() << "\n";
        }
    }
}

// ---------- menus ----------
static int ask_int(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::cout << prompt;
        std::string s; std::getline(std::cin, s);
        int v = 0; try { v = std::stoi(s); } catch (...) { v = lo - 1; }
        if (v >= lo && v <= hi) return v;
        std::cout << "Choose between " << lo << " and " << hi << ".\n";
    }
}

static void user_menu(model::Database& db) {
    for (;;) {
        std::cout << "\n[User]\n"
                     " 1) List\n"
                     " 2) View\n"
                     " 3) Add\n"
                     " 4) Edit\n"
                     " 5) Delete\n"
                     " 6) Find by column\n"
                     " 7) Back\n";
        int c = ask_int("Choose: ", 1, 7);
        if (c==1) list_users(db);
        else if (c==2) generic_view<model::User>(db, "user");
        else if (c==3) add_or_edit_user(db, 0);
        else if (c==4) { std::cout << "Enter id: "; std::string s; std::getline(std::cin,s); add_or_edit_user(db, to_ll(s)); }
        else if (c==5) generic_delete<model::User>(db, "user");
        else if (c==6) generic_find_by<model::User>(db, "user");
        else break;
    }
}

static void group_menu(model::Database& db) {
    for (;;) {
        std::cout << "\n[Group]\n"
                     " 1) List\n"
                     " 2) View\n"
                     " 3) Add\n"
                     " 4) Edit\n"
                     " 5) Delete\n"
                     " 6) Find by column\n"
                     " 7) Back\n";
        int c = ask_int("Choose: ", 1, 7);
        if (c==1) list_groups(db);
        else if (c==2) generic_view<model::Group>(db, "group");
        else if (c==3) add_or_edit_group(db, 0);
        else if (c==4) { std::cout << "Enter id: "; std::string s; std::getline(std::cin,s); add_or_edit_group(db, to_ll(s)); }
        else if (c==5) generic_delete<model::Group>(db, "group");
        else if (c==6) generic_find_by<model::Group>(db, "group");
        else break;
    }
}

static void entitlement_menu(model::Database& db) {
    for (;;) {
        std::cout << "\n[Entitlement]\n"
                     " 1) List\n"
                     " 2) View\n"
                     " 3) Add\n"
                     " 4) Edit\n"
                     " 5) Delete\n"
                     " 6) Find by column\n"
                     " 7) Back\n";
        int c = ask_int("Choose: ", 1, 7);
        if (c==1) list_entitlements(db);
        else if (c==2) generic_view<model::Entitlement>(db, "entitlement");
        else if (c==3) add_or_edit_entitlement(db, 0);
        else if (c==4) { std::cout << "Enter id: "; std::string s; std::getline(std::cin,s); add_or_edit_entitlement(db, to_ll(s)); }
        else if (c==5) generic_delete<model::Entitlement>(db, "entitlement");
        else if (c==6) generic_find_by<model::Entitlement>(db, "entitlement");
        else break;
    }
}

// ---------- main ----------
int main(int argc, char** argv) {
    std::string DB_USER = std::getenv("DB_USER") ? std::getenv("DB_USER") : "curioususer";
    std::string DB_PASS = std::getenv("DB_PASS") ? std::getenv("DB_PASS") : "curiouspass";
    std::string DB_NAME = std::getenv("DB_NAME") ? std::getenv("DB_NAME") : "test_db";
    std::string DB_HOST = std::getenv("DB_HOST") ? std::getenv("DB_HOST") : "localhost";
    unsigned DB_PORT = std::getenv("DB_PORT") ? static_cast<unsigned>(std::stoi(std::getenv("DB_PORT"))) : 5432;

    if (argc >= 4) {
        DB_USER = argv[1];
        DB_PASS = argv[2];
        DB_NAME = argv[3];
        if (argc >= 5) DB_HOST = argv[4];
        if (argc >= 6) DB_PORT = static_cast<unsigned>(std::stoi(argv[5]));
    }

    try {
        model::Database db(DB_USER, DB_PASS, DB_NAME, DB_HOST, DB_PORT);
        std::cout << "Connected to DB '" << DB_NAME << "' at " << DB_HOST << ":" << DB_PORT << "\n";

        for (;;) {
            std::cout << "\n== Menu ==\n"
                         " 1) Users\n"
                         " 2) Groups\n"
                         " 3) Entitlements\n"
                         " 4) Quit\n";
            int c = ask_int("Choose: ", 1, 4);
            if (c==1) user_menu(db);
            else if (c==2) group_menu(db);
            else if (c==3) entitlement_menu(db);
            else break;
        }
        std::cout << "Bye!\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Database error: " << ex.what() << "\n";
        return 1;
    }
}