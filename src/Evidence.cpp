#include "Evidence.h"
#include "Exceptions.h"
#include <algorithm>
#include <sstream>
#include <iostream>

// ─────────────────────── Evidence (abstract) ───────────────────────

Evidence::Evidence(const std::string& id, const std::string& desc,
                   const std::string& collector, EvidenceType type)
    : evidenceId_(id), description_(desc), collectedBy_(collector),
      type_(type), isCritical_(false), analyzed_(false), analysisResult_("Pending") {
    if (id.empty() || desc.empty() || collector.empty()) {
        throw InvalidEvidenceException("ID, description, and collector must be non-empty.");
    }
    checksum_ = computeChecksum(desc + collector);
}

// ─── Polymorphic dispatch entry point ───
// analyzeEvidence() is the public entry point; it delegates to the
// pure-virtual typeSpecificAnalysis() implemented in each derived
// class — this is the **Template Method** pattern riding on
// polymorphism.

void Evidence::analyzeEvidence() {
    typeSpecificAnalysis();       // virtual dispatch
    analyzed_ = true;
    std::cout << "  [Evidence " << evidenceId_ << "] Analysis complete.\n";
}

void Evidence::setAnalysisResult(const std::string& r) { analysisResult_ = r; }

bool Evidence::search(const std::string& keyword) const {
    std::string lower_desc = description_;
    std::string lower_id   = evidenceId_;
    auto toLower = [](std::string& s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    };
    toLower(lower_desc);
    toLower(lower_id);
    std::string kw = keyword;
    toLower(kw);
    return lower_desc.find(kw) != std::string::npos ||
           lower_id.find(kw) != std::string::npos;
}

// ─── Simple checksum (djb2-inspired) ───
uint32_t Evidence::computeChecksum(const std::string& data) {
    uint32_t hash = 5381;
    for (unsigned char c : data) {
        hash = ((hash << 5) + hash) + c;   // hash * 33 + c
    }
    return hash;
}

// ─── Getters ───
std::string Evidence::getId() const { return evidenceId_; }
std::string Evidence::getDescription() const { return description_; }
std::string Evidence::getCollectedBy() const { return collectedBy_; }
EvidenceType Evidence::getType() const { return type_; }
bool Evidence::isCriticalEvidence() const { return isCritical_; }
bool Evidence::isAnalyzed() const { return analyzed_; }
std::string Evidence::getAnalysisResult() const { return analysisResult_; }
uint32_t Evidence::getChecksum() const { return checksum_; }

void Evidence::setCritical(bool c) { isCritical_ = c; }
void Evidence::setDescription(const std::string& desc) { description_ = desc; }

std::string Evidence::getTypeString() const {
    switch (type_) {
        case EvidenceType::DIGITAL:      return "Digital";
        case EvidenceType::PHYSICAL:     return "Physical";
        case EvidenceType::TESTIMONIAL:  return "Testimonial";
    }
    return "Unknown";
}

// ─────────────────── DigitalEvidence ───────────────────

DigitalEvidence::DigitalEvidence(const std::string& id, const std::string& desc,
                                 const std::string& collector, const std::string& metadata)
    : Evidence(id, desc, collector, EvidenceType::DIGITAL), metadataSnapshot_(metadata) {}

// ─── Polymorphism: type-specific analysis ───
void DigitalEvidence::typeSpecificAnalysis() {
    std::cout << "  >> [DIGITAL] Scanning metadata snapshot (" << metadataSnapshot_.size()
              << " bytes) …\n";
    std::cout << "  >> File-system artifacts cross-referenced.\n";
    setAnalysisResult("Digital analysis complete — metadata: " + metadataSnapshot_);
}

bool DigitalEvidence::verifyIntegrity() {
    uint32_t current = computeChecksum(getDescription() + getCollectedBy());
    bool ok = (current == getChecksum());
    std::cout << "  [Integrity] " << getId() << ": "
              << (ok ? "PASS" : "FAIL — data may have been tampered") << "\n";
    return ok;
}

std::string DigitalEvidence::getReportContent() const {
    return "[Digital Evidence " + getId() + "] " + getDescription() +
           " | Collected by: " + getCollectedBy() +
           " | Metadata: " + metadataSnapshot_ +
           " | Analysis: " + getAnalysisResult();
}

std::string DigitalEvidence::getMetadata() const { return metadataSnapshot_; }

// ─────────────────── PhysicalEvidence ───────────────────

PhysicalEvidence::PhysicalEvidence(const std::string& id, const std::string& desc,
                                   const std::string& collector, const std::string& labResults)
    : Evidence(id, desc, collector, EvidenceType::PHYSICAL), labResults_(labResults) {}

void PhysicalEvidence::typeSpecificAnalysis() {
    std::cout << "  >> [PHYSICAL] Running lab-test protocol …\n";
    std::cout << "  >> Lab results: " << labResults_ << "\n";
    setAnalysisResult("Physical analysis complete — lab: " + labResults_);
}

bool PhysicalEvidence::verifyIntegrity() {
    uint32_t current = computeChecksum(getDescription() + getCollectedBy());
    bool ok = (current == getChecksum());
    std::cout << "  [Integrity] " << getId() << ": "
              << (ok ? "PASS" : "FAIL — chain-of-custody broken") << "\n";
    return ok;
}

std::string PhysicalEvidence::getReportContent() const {
    return "[Physical Evidence " + getId() + "] " + getDescription() +
           " | Collected by: " + getCollectedBy() +
           " | Lab Results: " + labResults_ +
           " | Analysis: " + getAnalysisResult();
}

std::string PhysicalEvidence::getLabResults() const { return labResults_; }

// ─────────────────── TestimonialEvidence ───────────────────

TestimonialEvidence::TestimonialEvidence(const std::string& id, const std::string& desc,
                                         const std::string& collector, const std::string& witness,
                                         double credibility)
    : Evidence(id, desc, collector, EvidenceType::TESTIMONIAL),
      credibilityScore_(credibility), witnessName_(witness) {}

void TestimonialEvidence::typeSpecificAnalysis() {
    std::cout << "  >> [TESTIMONIAL] Evaluating credibility of witness \"" << witnessName_ << "\" …\n";
    std::cout << "  >> Credibility score: " << credibilityScore_ << " / 10.0\n";
    std::string result = "Testimonial analysis complete — score: " +
                         std::to_string(credibilityScore_);
    if (credibilityScore_ < 3.0) result += " [LOW — flagged for review]";
    setAnalysisResult(result);
}

bool TestimonialEvidence::verifyIntegrity() {
    // For testimonial evidence, re-compute the checksum on the statement text
    uint32_t current = computeChecksum(getDescription() + getCollectedBy());
    bool ok = (current == getChecksum());
    std::cout << "  [Integrity] " << getId() << ": "
              << (ok ? "PASS" : "FAIL — statement modified") << "\n";
    return ok;
}

std::string TestimonialEvidence::getReportContent() const {
    return "[Testimonial Evidence " + getId() + "] " + getDescription() +
           " | Witness: " + witnessName_ +
           " | Credibility: " + std::to_string(credibilityScore_) +
           " | Analysis: " + getAnalysisResult();
}

double TestimonialEvidence::getCredibilityScore() const { return credibilityScore_; }
std::string TestimonialEvidence::getWitnessName() const { return witnessName_; }
