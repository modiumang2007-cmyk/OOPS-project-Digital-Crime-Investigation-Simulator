#ifndef CASE_H
#define CASE_H

#include <string>
#include <vector>
#include <memory>
#include "Person.h"
#include "Evidence.h"
#include "Investigator.h"
#include "Exceptions.h"

// ─── Encapsulation ───
// Case status is private and can only change through validated
// transitions.  A CLOSED case rejects every mutating operation from
// inside the class — the menu can't bypass it.

enum class CaseStatus { OPEN, UNDER_INVESTIGATION, CLOSED };

class Case {
    std::string caseId_;
    std::string title_;
    CaseStatus status_;
    std::string openDate_;
    std::string closeDate_ = "";

    // ─── Polymorphic storage ───
    // vector<unique_ptr<Evidence>> demonstrates modern C++ ownership
    // while still dispatching through the virtual interface.
    std::vector<std::unique_ptr<Evidence>> evidence_;
    std::vector<Suspect> suspects_;
    std::vector<Witness> witnesses_;
    std::shared_ptr<Investigator> investigator_;

    void requireOpen() const;      // throws CaseClosedException if closed

public:
    Case(const std::string& id, const std::string& title, const std::string& date)
        : caseId_(id), title_(title), status_(CaseStatus::OPEN), openDate_(date) {}

    // ─── Mutators (all guarded by status checks) ───
    void addSuspect(const Suspect& s);
    void addWitness(const Witness& w);
    void addEvidence(std::unique_ptr<Evidence> e);
    void assignInvestigator(std::shared_ptr<Investigator> inv);
    void changeStatus(CaseStatus newStatus);   // validated transition
    void closeCase();                          // → CLOSED, timestamp set
    bool removeEvidence(const std::string& evidenceId);

    // ─── Getters (Encapsulation) ───
    std::string getId() const;
    std::string getTitle() const;
    CaseStatus getStatus() const;
    std::string getStatusString() const;
    std::string getOpenDate() const;
    std::string getCloseDate() const;
    bool hasInvestigator() const;
    std::shared_ptr<Investigator> getInvestigator() const;
    const std::vector<std::unique_ptr<Evidence>>& getEvidence() const;
    const std::vector<Suspect>& getSuspects() const;
    const std::vector<Witness>& getWitnesses() const;
    size_t evidenceCount() const;
    size_t suspectCount() const;
    size_t witnessCount() const;

    // ─── Polymorphic traversal ───
    // Loops over the evidence vector and calls the virtual
    // analyzeEvidence() / verifyIntegrity() on each item.
    void analyzeAllEvidence() const;
    bool verifyEvidence(const std::string& evidenceId) const;
    void reportAllEvidence() const;
    
    // Overload == to compare two Case objects by ID
    bool operator==(const Case& other) const {
        return caseId_ == other.caseId_;
    }

    // Overload += as a shorthand to add evidence
    Case& operator+=(std::unique_ptr<Evidence> e) {
        addEvidence(std::move(e));
        return *this;
    }
    // Searchable
    bool search(const std::string& keyword) const;

    // Reportable — the Report class consumes this
    std::string getReportContent() const;
};

// Utility helpers
std::string statusToString(CaseStatus s);

#endif // CASE_H