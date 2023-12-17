#ifndef DEFINITION_H
#define DEFINITION_H

#include <string>
#include <functional>

class Definition {
    std::string name;
    bool isTerminal;

public:
    explicit Definition(std::string newName);
    Definition(std::string newName, bool newIsTerminal);

    const std::string& getName() const;
    bool getIsTerminal() const;

    size_t hash() const noexcept;
    bool operator==(const Definition& other) const noexcept;
};

// Specialization of std::hash<Definition>
namespace std {
    template <>
    struct hash<Definition> {
        size_t operator()(const Definition& obj) const noexcept;
    };
}

#endif // DEFINITION_H
