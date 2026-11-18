#ifndef PERSON_H
#define PERSON_H

#include <string>

// Simple domain records.  Encapsulation: private fields with getters.
// Person is kept as a "closed" concept — Suspect and Witness both
// share an id + name shape, but inheritance here would add nothing
// over composition (a "has-a" beats "is-a" for flat records), so we
// model them as two small concrete classes.

class Suspect {
    std::string suspectId_;
    std::string name_;
    std::string personalDetails_;

public:
    Suspect(const std::string& id, const std::string& name,
            const std::string& details)
        : suspectId_(id), name_(name), personalDetails_(details) {}

    std::string getId() const { return suspectId_; }
    std::string getName() const { return name_; }
    std::string getDetails() const { return personalDetails_; }

    // Searchable — matches id, name, or details
    bool search(const std::string& keyword) const {
        return suspectId_.find(keyword) != std::string::npos ||
               name_.find(keyword) != std::string::npos ||
               personalDetails_.find(keyword) != std::string::npos;
    }
};

class Witness {
    std::string witnessId_;
    std::string name_;
    std::string statementMetadata_;

public:
    Witness(const std::string& id, const std::string& name,
            const std::string& statement)
        : witnessId_(id), name_(name), statementMetadata_(statement) {}

    std::string getId() const { return witnessId_; }
    std::string getName() const { return name_; }
    std::string getStatement() const { return statementMetadata_; }

    bool search(const std::string& keyword) const {
        return witnessId_.find(keyword) != std::string::npos ||
               name_.find(keyword) != std::string::npos ||
               statementMetadata_.find(keyword) != std::string::npos;
    }
};

#endif // PERSON_H