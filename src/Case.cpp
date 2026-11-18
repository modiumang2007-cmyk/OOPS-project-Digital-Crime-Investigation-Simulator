#include "Case.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// ─────────────────────── Helpers ───────────────────────

std::string statusToString(CaseStatus s) {
    switch (s) {
        case CaseStatus::OPEN:                return "OPEN";
        case CaseStatus::UNDER_INVESTIGATION: return "UNDER_INVESTIGATION";
        case CaseStatus::CLOSED:              return "CLOSED";
    }
    return "UNKNOWN";
}

// ─── Encapsulation guard ───
// Called before every mutating operation; throws if the case is closed.
void Case::requireOpen() const {
    if (status_ == CaseStatus::CLOSED) {
        throw CaseClosedException(caseId_);
    }
}

// ─────────────────────── Mutators ───────────────────────

void Case::addSuspect(const Suspect& s) {
    requireOpen();
    suspects_.push_back(s);
    std::cout << "  Suspect \"" << s.getName() << "\" added to case " << caseId_ << ".\n";
}

void Case::addWitness(const Witness& w) {
    requireOpen();
    witnesses_.push_back(w);
    std::cout << "  Witness \"" << w.getName() << "\" added to case " << caseId_ << ".\n";
}

void Case::addEvidence(std::unique_ptr<Evidence> e) {
    requireOpen();
    std::cout << "  Evidence " << e->getId() << " (" << e->getTypeString()
              << ") added to case " << caseId_ << ".\n";
    evidence_.push_back(std::move(e));
}

void Case::assignInvestigator(std::shared_ptr<Investigator> inv) {
    requireOpen();
    if (!inv->canAcceptCase()) {
        throw InvalidInvestigatorException(
            inv->getName() + " has reached maximum caseload.");
    }
    investigator_ = inv;
    status_ = CaseStatus::UNDER_INVESTIGATION;      // auto-promote
    std::cout << "  Investigator \"" << inv->getName() << "\" assigned to case "
              << caseId_ << ". Status -> UNDER_INVESTIGATION.\n";
}

// ─── Validated status transitions ───
void Case::changeStatus(CaseStatus newStatus) {
    requireOpen();
    if (newStatus == CaseStatus::UNDER_INVESTIGATION &&
        status_ == CaseStatus::OPEN) {
        status_ = newStatus;
    } else if (newStatus == CaseStatus::CLOSED &&
               status_ == CaseStatus::UNDER_INVESTIGATION) {
        closeCase();
    } else {
        std::cout << "  Invalid status transition.\n";
    }
}

void Case::closeCase() {
    if (status_ == CaseStatus::CLOSED) {
        throw CaseClosedException(caseId_);
    }
    if (status_ != CaseStatus::UNDER_INVESTIGATION) {
        std::cout << "  Cannot close a case that is not UNDER_INVESTIGATION.\n";
        return;
    }
    status_ = CaseStatus::CLOSED;
    closeDate_ = "now";   // simplified timestamp
    std::cout << "  Case " << caseId_ << " has been CLOSED.\n";
}

bool Case::removeEvidence(const std::string& evidenceId) {
    requireOpen();
    auto it = std::find_if(evidence_.begin(), evidence_.end(),
        [&](const std::unique_ptr<Evidence>& e) { return e->getId() == evidenceId; });
    if (it != evidence_.end()) {
        evidence_.erase(it);
        std::cout << "  Evidence " << evidenceId << " removed.\n";
        return true;
    }
    std::cout << "  Evidence " << evidenceId << " not found.\n";
    return false;
}

// ─────────────────────── Getters ───────────────────────

std::string Case::getId() const { return caseId_; }
std::string Case::getTitle() const { return title_; }
CaseStatus Case::getStatus() const { return status_; }
std::string Case::getStatusString() const { return statusToString(status_); }
std::string Case::getOpenDate() const { return openDate_; }
std::string Case::getCloseDate() const { return closeDate_; }
bool Case::hasInvestigator() const { return investigator_ != nullptr; }
std::shared_ptr<Investigator> Case::getInvestigator() const { return investigator_; }
const std::vector<std::unique_ptr<Evidence>>& Case::getEvidence() const { return evidence_; }
const std::vector<Suspect>& Case::getSuspects() const { return suspects_; }
const std::vector<Witness>& Case::getWitnesses() const { return witnesses_; }
size_t Case::evidenceCount() const { return evidence_.size(); }
size_t Case::suspectCount() const { return suspects_.size(); }
size_t Case::witnessCount() const { return witnesses_.size(); }

// ─────────────────────── Polymorphic traversal ───────────────────────

void Case::analyzeAllEvidence() const {
    std::cout << "\n── Analyzing all evidence for case " << caseId_ << " ──\n";
    for (const auto& e : evidence_) {
        // Note: unique_ptr::get() is const-qualified but returns a
        // non-const Evidence*, so virtual dispatch and the mutating
        // analyzeEvidence() both work through this const method.
        e->analyzeEvidence();
    }
}

bool Case::verifyEvidence(const std::string& evidenceId) const {
    for (const auto& e : evidence_) {
        if (e->getId() == evidenceId) {
            return e->verifyIntegrity();
        }
    }
    std::cout << "  Evidence " << evidenceId << " not found.\n";
    return false;
}

void Case::reportAllEvidence() const {
    std::cout << "\n── Evidence Report for case " << caseId_ << " ──\n";
    for (const auto& e : evidence_) {
        std::cout << "  " << e->getReportContent() << "\n";
    }
}

// ─────────────────────── Search ───────────────────────

bool Case::search(const std::string& keyword) const {
    if (caseId_.find(keyword) != std::string::npos ||
        title_.find(keyword) != std::string::npos) {
        return true;
    }
    for (const auto& s : suspects_) {
        if (s.search(keyword)) return true;
    }
    for (const auto& w : witnesses_) {
        if (w.search(keyword)) return true;
    }
    for (const auto& e : evidence_) {
        if (e->search(keyword)) return true;
    }
    return false;
}

// ─────────────────────── Report content ───────────────────────

std::string Case::getReportContent() const {
    std::ostringstream oss;
    oss << "╔══════════════════════════════════════════════════╗\n"
        << "║           CASE REPORT                           ║\n"
        << "╚══════════════════════════════════════════════════╝\n"
        << "Case ID   : " << caseId_ << "\n"
        << "Title     : " << title_ << "\n"
        << "Status    : " << statusToString(status_) << "\n"
        << "Opened    : " << openDate_ << "\n"
        << "Closed    : " << (closeDate_.empty() ? "—" : closeDate_) << "\n\n";

    if (investigator_) {
        oss << "Investigator: " << investigator_->getName()
            << " (" << investigator_->getTypeString() << ")\n\n";
    } else {
        oss << "Investigator: None assigned\n\n";
    }

    oss << "── Suspects (" << suspects_.size() << ") ──\n";
    for (const auto& s : suspects_) {
        oss << "  • " << s.getName() << " [" << s.getId() << "] — " << s.getDetails() << "\n";
    }

    oss << "\n── Witnesses (" << witnesses_.size() << ") ──\n";
    for (const auto& w : witnesses_) {
        oss << "  • " << w.getName() << " [" << w.getId() << "] — " << w.getStatement() << "\n";
    }

    oss << "\n── Evidence (" << evidence_.size() << ") ──\n";
    for (const auto& e : evidence_) {
        oss << "  • " << e->getReportContent() << "\n";
    }

    return oss.str();
}
