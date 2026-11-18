#include "Report.h"
#include <sstream>

Report::Report(const std::string& id, const std::string& timestamp, const Case& c)
    : reportId_(id), generatedTimestamp_(timestamp),
      content_(c.getReportContent()) {}

Report::Report(const std::string& id, const std::string& timestamp, const std::string& content)
    : reportId_(id), generatedTimestamp_(timestamp), content_(content) {}

std::string Report::getId() const { return reportId_; }
std::string Report::getTimestamp() const { return generatedTimestamp_; }
std::string Report::getContent() const { return content_; }

std::string Report::serialize() const {
    return reportId_ + "|" + generatedTimestamp_ + "|" + content_;
}

Report Report::deserialize(const std::string& line) {
    auto p1 = line.find('|');
    auto p2 = line.find('|', p1 + 1);
    std::string id = line.substr(0, p1);
    std::string ts = line.substr(p1 + 1, p2 - p1 - 1);
    std::string ct = line.substr(p2 + 1);
    return Report(id, ts, ct);
}
