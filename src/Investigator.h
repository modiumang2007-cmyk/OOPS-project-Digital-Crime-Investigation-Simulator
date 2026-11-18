#ifndef INVESTIGATOR_H
#define INVESTIGATOR_H

#include <string>

// ─── Abstraction ───
// Investigator is an abstract base class.  Concrete investigation
// procedures differ between Cyber and Field investigators, so
// investigate() is left pure-virtual.

enum class InvestigatorType { CYBER, FIELD };

class Investigator {
    std::string investigatorId_;
    std::string name_;
    InvestigatorType type_;
    int activeCaseload_;
    static const int MAX_CASELOAD_ = 5;

protected:
    Investigator(const std::string& id, const std::string& name,
                 InvestigatorType type);

public:
    virtual ~Investigator() = default;

    // ─── Polymorphic hook ───
    virtual void investigate() = 0;

    // ─── Getters (Encapsulation) ───
    std::string getId() const;
    std::string getName() const;
    InvestigatorType getType() const;
    std::string getTypeString() const;
    int getActiveCaseload() const;

    // ─── Caseload management ───
    bool canAcceptCase() const;
    void incrementCaseload();
    void decrementCaseload();
};

// ─── Inheritance: CyberInvestigator ───
class CyberInvestigator : public Investigator {
public:
    CyberInvestigator(const std::string& id, const std::string& name);
    void investigate() override;
};

// ─── Inheritance: FieldInvestigator ───
class FieldInvestigator : public Investigator {
public:
    FieldInvestigator(const std::string& id, const std::string& name);
    void investigate() override;
};

#endif // INVESTIGATOR_H
