#ifndef REPORT_H
#define REPORT_H

#include <string>
#include "Case.h"
#include <ostream>


// Report aggregates a Case's state into a printable summary.
// It is a separate class so that report formatting logic has a
// single home (Separation of Concerns) rather than living in Case.

class Report {
    std::string reportId_;
    std::string generatedTimestamp_;
    std::string content_;

public:
    Report(const std::string& id, const std::string& timestamp, const Case& c);
    Report(const std::string& id, const std::string& timestamp, const std::string& content);

    // ─── Getters (Encapsulation) ───
    std::string getId() const;
    std::string getTimestamp() const;
    std::string getContent() const;

    // Persisted as plain text
    std::string serialize() const;
    static Report deserialize(const std::string& line);
};

// Overload << so std::cout << report; prints the formatted report
inline std::ostream& operator<<(std::ostream& os, const Report& r) {
    os << r.getContent();
    return os;
}
#endif // REPORT_H