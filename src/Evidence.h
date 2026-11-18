#ifndef EVIDENCE_H
#define EVIDENCE_H

#include <string>
#include <cstdint>

// ─── Encapsulation ───
// All fields private, accessed via getters/setters only.

// ─── Abstraction ───
// Evidence is an abstract base class with pure virtual functions.
// Each derived class must implement analyzeEvidence(), verifyIntegrity(),
// and generateReport() differently — the caller never needs to know
// which concrete type it holds.

enum class EvidenceType { DIGITAL, PHYSICAL, TESTIMONIAL };

class Evidence {
    std::string evidenceId_;
    std::string description_;
    std::string collectedBy_;
    EvidenceType type_;
    bool isCritical_;
    bool analyzed_;
    std::string analysisResult_;
    uint32_t checksum_;          // simple integrity checksum

    // ─── Polymorphism hook (pure virtual) ───
    virtual void typeSpecificAnalysis() = 0;

protected:
    // Used by derived classes to set the analysis result string
    void setAnalysisResult(const std::string& r);

public:
    Evidence(const std::string& id, const std::string& desc,
             const std::string& collector, EvidenceType type);
    virtual ~Evidence() = default;           // polymorphic deletion safety

    // ─── Polymorphic interface (pure virtuals) ───
    virtual bool verifyIntegrity() = 0;
    virtual std::string getReportContent() const = 0;

    // ─── Shared concrete method that calls the virtual hook ───
    void analyzeEvidence();

    // ─── Searchable interface ───
    virtual bool search(const std::string& keyword) const;

    // ─── Getters (Encapsulation) ───
    std::string getId() const;
    std::string getDescription() const;
    std::string getCollectedBy() const;
    EvidenceType getType() const;
    bool isCriticalEvidence() const;
    bool isAnalyzed() const;
    std::string getAnalysisResult() const;
    uint32_t getChecksum() const;

    // ─── Setters with validation ───
    void setCritical(bool c);
    void setDescription(const std::string& desc);

    // ─── Utility ───
    std::string getTypeString() const;
    static uint32_t computeChecksum(const std::string& data);
};

// ─── Inheritance: DigitalEvidence ───
class DigitalEvidence : public Evidence {
    std::string metadataSnapshot_;
    void typeSpecificAnalysis() override;

public:
    DigitalEvidence(const std::string& id, const std::string& desc,
                    const std::string& collector, const std::string& metadata);

    bool verifyIntegrity() override;
    std::string getReportContent() const override;
    std::string getMetadata() const;
};

// ─── Inheritance: PhysicalEvidence ───
class PhysicalEvidence : public Evidence {
    std::string labResults_;
    void typeSpecificAnalysis() override;

public:
    PhysicalEvidence(const std::string& id, const std::string& desc,
                     const std::string& collector, const std::string& labResults);

    bool verifyIntegrity() override;
    std::string getReportContent() const override;
    std::string getLabResults() const;
};

// ─── Inheritance: TestimonialEvidence ───
class TestimonialEvidence : public Evidence {
    double credibilityScore_;
    std::string witnessName_;
    void typeSpecificAnalysis() override;

public:
    TestimonialEvidence(const std::string& id, const std::string& desc,
                        const std::string& collector, const std::string& witness,
                        double credibility);

    bool verifyIntegrity() override;
    std::string getReportContent() const override;
    double getCredibilityScore() const;
    std::string getWitnessName() const;
};

#endif // EVIDENCE_H
