#include "Investigator.h"
#include <iostream>
#include <stdexcept>

// ─────────────────────── Investigator (abstract) ───────────────────────

Investigator::Investigator(const std::string& id, const std::string& name,
                           InvestigatorType type)
    : investigatorId_(id), name_(name), type_(type), activeCaseload_(0) {
    if (id.empty() || name.empty()) {
        throw std::invalid_argument("Investigator ID and name must be non-empty.");
    }
}

std::string Investigator::getId() const { return investigatorId_; }
std::string Investigator::getName() const { return name_; }
InvestigatorType Investigator::getType() const { return type_; }
int Investigator::getActiveCaseload() const { return activeCaseload_; }

std::string Investigator::getTypeString() const {
    return (type_ == InvestigatorType::CYBER) ? "Cyber" : "Field";
}

bool Investigator::canAcceptCase() const {
    return activeCaseload_ < MAX_CASELOAD_;
}

void Investigator::incrementCaseload() { ++activeCaseload_; }
void Investigator::decrementCaseload() {
    if (activeCaseload_ > 0) --activeCaseload_;
}

// ─────────────────── CyberInvestigator ───────────────────

CyberInvestigator::CyberInvestigator(const std::string& id, const std::string& name)
    : Investigator(id, name, InvestigatorType::CYBER) {}

void CyberInvestigator::investigate() {
    std::cout << "  [Cyber Investigator " << getName() << "] Initiating digital forensics workflow:\n";
    std::cout << "    1. Imaging suspect drives …\n";
    std::cout << "    2. Extracting network logs …\n";
    std::cout << "    3. Cross-referencing IP addresses with known threat actors …\n";
    std::cout << "    4. Running decryption pass on flagged artifacts …\n";
    std::cout << "    >> Digital forensics scan complete.\n";
}

// ─────────────────── FieldInvestigator ───────────────────

FieldInvestigator::FieldInvestigator(const std::string& id, const std::string& name)
    : Investigator(id, name, InvestigatorType::FIELD) {}

void FieldInvestigator::investigate() {
    std::cout << "  [Field Investigator " << getName() << "] Initiating on-site procedure:\n";
    std::cout << "    1. Securing the crime scene perimeter …\n";
    std::cout << "    2. Documenting physical evidence in situ …\n";
    std::cout << "    3. Collecting witness statements on record …\n";
    std::cout << "    4. Coordinating with forensics lab for chain of custody …\n";
    std::cout << "    >> Field investigation scan complete.\n";
}
