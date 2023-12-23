#ifndef DEFINITION_H
#define DEFINITION_H

#include <string>

class Definition {
    bool isTerminal;
    std::string name;

public:
    static const std::string EPSILON;
    static const std::string DOLLAR;

    explicit Definition(std::string newName);

    Definition(std::string newName, bool newIsTerminal);


    [[nodiscard]] bool getIsTerminal() const;

    [[nodiscard]] const std::string& getName() const;

    [[nodiscard]] size_t hash() const noexcept;


    bool operator==(const Definition& other) const noexcept;

    static Definition * getEpsilon();
    static Definition * getDollar();

};



// Specialization of std::hash<Definition>

template<>
struct std::hash<Definition> {
    size_t operator()(const Definition& obj) const noexcept;

};


#endif // DEFINITION_H