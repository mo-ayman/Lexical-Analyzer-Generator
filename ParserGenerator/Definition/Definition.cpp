#include "definition.h"

// Constructors
Definition::Definition(std::string newName)
    : name(std::move(newName)), isTerminal(true) {}

Definition::Definition(std::string newName, const bool newIsTerminal)
    : name(std::move(newName)), isTerminal(newIsTerminal) {}

// Getters
const std::string& Definition::getName() const {
    return name;
}

bool Definition::getIsTerminal() const {
    return isTerminal;
}

// Hash function
size_t Definition::hash() const noexcept {
    return std::hash<std::string>{}(name) ^ std::hash<bool>{}(isTerminal);
}

// Equality comparison
bool Definition::operator==(const Definition& other) const noexcept {
    return name == other.name && isTerminal == other.isTerminal;
}

// Specialization of std::hash<Definition>
namespace std {
    size_t hash<Definition>::operator()(const Definition& obj) const noexcept {
        return obj.hash();
    }
}
