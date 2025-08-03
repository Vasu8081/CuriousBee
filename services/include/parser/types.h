#pragma once
#include <string>
#include <vector>

namespace parser {

struct Field {
    std::string type;
    std::string name;

    Field(const std::string &t, const std::string &n)
        : type(t), name(n) {}
};

struct Message {
    std::string name;
    int id = -1;
    std::string parent;
    std::vector<Field> fields;

    Message() = default;
    Message(const std::string &n, int i, const std::string &p = "")
        : name(n), id(i), parent(p) {}
};

}  // namespace parser
