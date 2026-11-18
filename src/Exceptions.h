#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

// ─── Abstraction via custom exception hierarchy ───
// All project-specific exceptions derive from std::exception,
// giving callers a single catch-all while preserving type info.

class CaseClosedException : public std::exception {
    std::string message_;
public:
    explicit CaseClosedException(const std::string& caseId)
        : message_("Case " + caseId + " is already CLOSED — operation rejected.") {}
    const char* what() const noexcept override { return message_.c_str(); }
};

class InvalidEvidenceException : public std::exception {
    std::string message_;
public:
    explicit InvalidEvidenceException(const std::string& msg)
        : message_("Invalid evidence: " + msg) {}
    const char* what() const noexcept override { return message_.c_str(); }
};

class InvalidInvestigatorException : public std::exception {
    std::string message_;
public:
    explicit InvalidInvestigatorException(const std::string& msg)
        : message_("Invalid investigator: " + msg) {}
    const char* what() const noexcept override { return message_.c_str(); }
};

#endif // EXCEPTIONS_H
