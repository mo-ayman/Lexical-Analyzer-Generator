#ifndef DEFINITION_H
#define DEFINITION_H

#include <string>

class Definition {
    bool isTerminal;
    std::string name;

public:
    explicit Definition(std::string newName);
    Definition(std::string newName, bool newIsTerminal);

    bool getIsTerminal() const;
    const std::string& getName() const;

    size_t hash() const noexcept;
    bool operator==(const Definition& other) const noexcept;
};

// Specialization of std::hash<Definition>
template <>
struct std::hash<Definition> {
    size_t operator()(const Definition& obj) const noexcept;
};

#endif // DEFINITION_H