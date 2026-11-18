#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <limits>

#include "Exceptions.h"

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP for UTF-8 box-drawing chars
#endif

// We need the implementation units' declarations
#include "Evidence.h"
#include "Investigator.h"
#include "Person.h"
#include "Case.h"
#include "Report.h"

// ───────────────────────── App state ─────────────────────────
// The investigation system gets two containers:
//   cases:        map<caseId, Case>  — keyed lookup by id
//   investigators: vector<shared_ptr<Investigator>>
// The map gives O(log n) id-based access; the vector lets the user
// pick an investigator by index from the roster.

std::map<std::string, Case> g_cases;
std::vector<std::shared_ptr<Investigator>> g_investigators;
int g_nextId = 1;

// ───────────────────────── Small utilities ─────────────────────────

std::string now() { return "2026-09-16"; }               // simplified date

int readChoice() {
    int n;
    std::cin >> n;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return n;
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

// ───────────────────────── ID helpers ─────────────────────────

std::string nextCaseId() {
    while (g_cases.count("CASE-" + std::to_string(g_nextId))) ++g_nextId;
    return "CASE-" + std::to_string(g_nextId++);
}
std::string nextEvidenceId(int n) { return "EVID-" + std::to_string(n); }
static int s_evidenceCounter = 1000;
std::string nextEvidenceId() { return "EVID-" + std::to_string(s_evidenceCounter++); }
int nextPersonSerial() {
    static int s_personSerial = 1;
    return s_personSerial++;
}

// ───────────────────────── Menu: create & view ─────────────────────────

void createCase() {
    std::string title = readLine("  Case title: ");
    if (title.empty()) { std::cout << "  Title cannot be empty.\n"; return; }
    std::string id = nextCaseId();
    g_cases.emplace(std::piecewise_construct,
                    std::forward_as_tuple(id),
                    std::forward_as_tuple(id, title, now()));
    std::cout << "  Created case " << id << " — \"" << title << "\"\n";
}

void listCases() {
    if (g_cases.empty()) {
        std::cout << "  No cases on record.\n";
        return;
    }
    std::cout << "\n  ── Current Cases ──\n";
    for (const auto& kv : g_cases) {
        const std::string& id = kv.first;
        const Case& c = kv.second;
        std::cout << "  [" << id << "] " << c.getTitle()
                  << "  Status: " << c.getStatusString()
                  << "  Evidence: " << c.evidenceCount()
                  << "  Suspects: " << c.suspectCount() << "\n";
    }
}

Case* selectCase() {
    if (g_cases.empty()) {
        std::cout << "  No cases exist yet. Create one first (menu 1).\n";
        return nullptr;
    }
    listCases();
    std::string id = readLine("  Enter case ID: ");
    auto it = g_cases.find(id);
    if (it == g_cases.end()) {
        std::cout << "  Case not found: " << id << "\n";
        return nullptr;
    }
    return &it->second;
}

// ───────────────────────── Menu: add suspect / witness ─────────────────────────

void addSuspect(Case& c) {
    std::string name = readLine("  Suspect name: ");
    std::string det  = readLine("  Personal details: ");
    c.addSuspect(Suspect("SUS-" + std::to_string(nextPersonSerial()),
                         name.empty() ? "Unknown" : name, det));
}

void addWitness(Case& c) {
    std::string name = readLine("  Witness name: ");
    std::string stmt = readLine("  Statement summary: ");
    c.addWitness(Witness("WIT-" + std::to_string(nextPersonSerial()),
                         name.empty() ? "Unknown" : name, stmt));
}

// ───────────────────────── Menu: add evidence (polymorphic) ─────────────────────────

void addEvidence(Case& c) {
    std::cout << "\n  Evidence type:\n"
              << "    1. Digital      (e.g. disk image, logs)\n"
              << "    2. Physical     (e.g. weapon, fingerprints)\n"
              << "    3. Testimonial  (e.g. witness statement)\n"
              << "  Select: ";
    int t = readChoice();
    if (t < 1 || t > 3) { std::cout << "  Unknown type.\n"; return; }

    std::string desc    = readLine("  Description: ");
    std::string collector = readLine("  Collected by: ");
    if (desc.empty() || collector.empty()) {
        std::cout << "  Description and collector are required.\n";
        return;
    }
    std::string id = nextEvidenceId();

    std::unique_ptr<Evidence> ev;
    switch (t) {
        case 1: {
            std::string meta = readLine("  Metadata snapshot (e.g. file hash, image path): ");
            ev = std::make_unique<DigitalEvidence>(id, desc, collector, meta);
            break;
        }
        case 2: {
            std::string lab = readLine("  Lab results summary: ");
            ev = std::make_unique<PhysicalEvidence>(id, desc, collector, lab);
            break;
        }
        case 3: {
            std::string witness = readLine("  Witness name: ");
            std::cout << "  Credibility score (0.0 – 10.0): ";
            double score = 5.0;
            if (!(std::cin >> score)) {
                std::cin.clear();
                std::cout << "  Invalid number — using 5.0.\n";
            }
            std::cin.ignore(9999, '\n');
            if (score < 0.0) score = 0.0;
            if (score > 10.0) score = 10.0;
            ev = std::make_unique<TestimonialEvidence>(id, desc, collector, witness, score);
            break;
        }
    }
    try {
        c.addEvidence(std::move(ev));
    } catch (const CaseClosedException& err) {
        std::cout << "  ✖ " << err.what() << "\n";
    }
}

// ───────────────────────── Menu: analyze / verify ─────────────────────────

void analyzeEvidence(Case& c) {
    try {
        c.analyzeAllEvidence();
    } catch (const CaseClosedException& err) {
        std::cout << "  ✖ " << err.what() << "\n";
    }
}

void verifyEvidence(Case& c) {
    c.reportAllEvidence();
    std::string id = readLine("  Evidence ID to verify: ");
    c.verifyEvidence(id);
}

// ───────────────────────── Menu: assign investigator ─────────────────────────

void registerInvestigator() {
    if (g_investigators.size() >= 12) {
        std::cout << "  Roster full.\n";
        return;
    }
    std::cout << "\n  Investigator type:\n    1. Cyber\n    2. Field\n  Select: ";
    int t = readChoice();
    if (t < 1 || t > 2) { std::cout << "  Unknown type.\n"; return; }
    std::string name = readLine("  Investigator name: ");
    std::string id = "INV-" + std::to_string(nextPersonSerial());
    if (t == 1) g_investigators.push_back(std::make_shared<CyberInvestigator>(id, name));
    else        g_investigators.push_back(std::make_shared<FieldInvestigator>(id, name));
    std::cout << "  Registered " << name << " as "
              << g_investigators.back()->getTypeString() << " investigator.\n";
}

void assignInvestigator(Case& c) {
    if (g_investigators.empty()) {
        std::cout << "  No investigators registered. Register one first.\n";
        return;
    }
    std::cout << "\n  ── Investigator Roster ──\n";
    for (size_t i = 0; i < g_investigators.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << g_investigators[i]->getName()
                  << " (" << g_investigators[i]->getTypeString() << ")"
                  << "  Caseload: " << g_investigators[i]->getActiveCaseload()
                  << (g_investigators[i]->canAcceptCase() ? "" : "  [FULL]") << "\n";
    }
    std::cout << "  Select investigator: ";
    int idx = readChoice() - 1;
    if (idx < 0 || idx >= static_cast<int>(g_investigators.size())) {
        std::cout << "  Bad choice.\n";
        return;
    }
    try {
        c.assignInvestigator(g_investigators[idx]);
        g_investigators[idx]->incrementCaseload();
    } catch (const InvalidInvestigatorException& err) {
        std::cout << "  ✖ " << err.what() << "\n";
    } catch (const CaseClosedException& err) {
        std::cout << "  ✖ " << err.what() << "\n";
    }
}

// ───────────────────────── Menu: report / close ─────────────────────────

void generateReport(Case& c) {
    if (c.getInvestigator() && c.evidenceCount() > 0) {
        // Polymorphism: invoke the investigator's own workflow
        c.getInvestigator()->investigate();
        std::cout << "\n";
    }
    Report r("RPT-" + std::to_string(nextPersonSerial()), now(), c);
    std::cout << "\n" << r.getContent() << "\n";
}

void closeCase(Case& c) {
    // Closure contract: needs an assigned investigator
    try {
        if (!c.hasInvestigator()) {
            std::cout << "  Assign an investigator before closing this case.\n";
            return;
        }
        c.closeCase();
    } catch (const CaseClosedException& err) {
        std::cout << "  ✖ " << err.what() << "\n";
    }
}

// ───────────────────────── Search ─────────────────────────

void searchAll() {
    std::string kw = readLine("  Search keyword: ");
    bool found = false;
    for (const auto& kv : g_cases) {
        const std::string& id = kv.first;
        const Case& c = kv.second;
        if (c.search(kw)) {
            std::cout << "  ▶ " << id << " — " << c.getTitle() << "\n";
            found = true;
        }
    }
    std::cout << (found ? "" : "  No matches.\n");
}

// ───────────────────────── Persistence ─────────────────────────
// Saves/loads to a plain-text file. Because Evidence is polymorphic,
// each record carries a type tag so the loader can reconstruct the
// right derived class (re-opening the object through the base-class
// interface — the reverse of normal runtime dispatch).

void saveCases(const std::string& filename) {
    std::ofstream out(filename);
    if (!out) { std::cout << "  Cannot write " << filename << "\n"; return; }
    out << g_cases.size() << "\n";
    for (const auto& kv : g_cases) {
        const std::string& id = kv.first;
        const Case& c = kv.second;
        out << id << "|" << c.getTitle() << "|"
            << static_cast<int>(c.getStatus()) << "|" << c.getOpenDate()
            << "|" << c.getCloseDate() << "\n";
        out << c.suspectCount() << " " << c.witnessCount() << " " << c.evidenceCount() << "\n";
        for (const auto& s : c.getSuspects()) {
            out << s.getId() << "|" << s.getName() << "|" << s.getDetails() << "\n";
        }
        for (const auto& w : c.getWitnesses()) {
            out << w.getId() << "|" << w.getName() << "|" << w.getStatement() << "\n";
        }
        for (const auto& e : c.getEvidence()) {
            out << static_cast<int>(e->getType()) << "|"
                << e->getId() << "|" << e->getDescription() << "|" << e->getCollectedBy()
                << "|" << (e->isCriticalEvidence() ? 1 : 0)
                << "|" << e->getChecksum() << "\n";
            switch (e->getType()) {
                case EvidenceType::DIGITAL:
                    out << dynamic_cast<DigitalEvidence*>(e.get())->getMetadata() << "\n";
                    break;
                case EvidenceType::PHYSICAL:
                    out << dynamic_cast<PhysicalEvidence*>(e.get())->getLabResults() << "\n";
                    break;
                case EvidenceType::TESTIMONIAL: {
                    auto* t = dynamic_cast<TestimonialEvidence*>(e.get());
                    out << t->getWitnessName() << "|" << t->getCredibilityScore() << "\n";
                    break;
                }
            }
        }
    }
    out.close();
    std::cout << "  Saved " << g_cases.size() << " case(s) to " << filename << ".\n";
}

void loadCases(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) { std::cout << "  No save file " << filename << " — starting fresh.\n"; return; }
    size_t n;
    in >> n; in.ignore(9999, '\n');
    g_cases.clear();
    for (size_t i = 0; i < n; ++i) {
        std::string id, title, openDate, closeDate;
        int status;
        std::string line;
        std::getline(in, line);
        std::stringstream ss(line);
        std::getline(ss, id, '|');
        std::getline(ss, title, '|');
        ss >> status; ss.ignore(1);
        std::getline(ss, openDate, '|');
        std::getline(ss, closeDate, '|');
        Case c(id, title, openDate);
        // Rebuild the status through the validated public API.
        if (status == 1) c.changeStatus(CaseStatus::UNDER_INVESTIGATION);
        else if (status == 2) {
            // Need to be UNDER_INVESTIGATION before closing.
            c.changeStatus(CaseStatus::UNDER_INVESTIGATION);
            c.closeCase();        // no investigator guard applied at load time
        }
        in >> std::ws;
        size_t nSus, nWit, nEv;
        in >> nSus >> nWit >> nEv; in.ignore(9999, '\n');
        for (size_t s = 0; s < nSus; ++s) {
            std::getline(in, line); std::stringstream s2(line);
            std::string sid, sname, sdet;
            std::getline(s2, sid, '|'); std::getline(s2, sname, '|'); std::getline(s2, sdet, '|');
            c.addSuspect(Suspect(sid, sname, sdet));
        }
        for (size_t w = 0; w < nWit; ++w) {
            std::getline(in, line); std::stringstream s3(line);
            std::string wid, wname, wstmt;
            std::getline(s3, wid, '|'); std::getline(s3, wname, '|'); std::getline(s3, wstmt, '|');
            c.addWitness(Witness(wid, wname, wstmt));
        }
        for (size_t e = 0; e < nEv; ++e) {
            // Format: type|id|description|collectedBy|isCritical|checksum
            std::getline(in, line); std::stringstream s5(line);
            int t; std::string eid, edesc, ecolby; int crit; uint32_t savedChecksum;
            s5 >> t; s5.ignore(1);
            std::getline(s5, eid, '|');
            std::getline(s5, edesc, '|');
            std::getline(s5, ecolby, '|');
            s5 >> crit; s5.ignore(1);
            s5 >> savedChecksum;    // preserved for integrity re-check after load
            std::unique_ptr<Evidence> ev;
            switch (t) {
                case 0: {
                    std::string meta; std::getline(in, meta);
                    ev = std::make_unique<DigitalEvidence>(eid, edesc, ecolby, meta);
                    break;
                }
                case 1: {
                    std::string lab; std::getline(in, lab);
                    ev = std::make_unique<PhysicalEvidence>(eid, edesc, ecolby, lab);
                    break;
                }
                case 2: {
                    std::string witness; double score;
                    std::getline(in, line); std::stringstream s6(line);
                    std::getline(s6, witness, '|'); s6 >> score;
                    ev = std::make_unique<TestimonialEvidence>(eid, edesc, ecolby, witness, score);
                    break;
                }
            }
            if (crit) ev->setCritical(true);
            c.addEvidence(std::move(ev));
        }
        g_cases.emplace(id, std::move(c));
    }
    in.close();
    std::cout << "  Loaded " << g_cases.size() << " case(s) from " << filename << ".\n";
}

// ───────────────────────── Main loop ─────────────────────────

void showMenu() {
    std::cout << "\n"
              << "  ╔══════════════════════════════════════════════╗\n"
              << "  ║  CaseFile — Crime Investigation Simulator     ║\n"
              << "  ╚══════════════════════════════════════════════╝\n"
              << "   1. Create New Case\n"
              << "   2. Add Suspect / Witness to Case\n"
              << "   3. Add Evidence to Case\n"
              << "   4. Analyze Evidence\n"
              << "   5. Verify Evidence Integrity\n"
              << "   6. Assign Investigator\n"
              << "   7. Generate Case Report\n"
              << "   8. View All Cases\n"
              << "   9. Close Case\n"
              << "  10. Search\n"
              << "  11. Register Investigator\n"
              << "  12. Remove Evidence\n"
              << "  13. Save to file\n"
              << "  14. Load from file\n"
              << "   0. Exit\n"
              << "  ─────────────────────────────────────────────\n"
              << "  Choice: ";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "  Welcome to CaseFile — Digital Crime Investigation Simulator\n"
              << "  (OOP demo: Abstraction • Inheritance • Polymorphism • Encapsulation)\n";

    loadCases("cases.txt");

    int choice;
    do {
        showMenu();
        choice = readChoice();
        Case* c = nullptr;
        switch (choice) {
            case 0: break;
            case 1: createCase(); break;
            case 2:
                if ((c = selectCase())) {
                    std::cout << "  1. Suspect\n  2. Witness\n  Select: ";
                    int t = readChoice();
                    if (t == 1) addSuspect(*c);
                    else if (t == 2) addWitness(*c);
                    else std::cout << "  Bad option.\n";
                }
                break;
            case 3:
                if ((c = selectCase())) addEvidence(*c);
                break;
            case 4:
                if ((c = selectCase())) analyzeEvidence(*c);
                break;
            case 5:
                if ((c = selectCase())) verifyEvidence(*c);
                break;
            case 6:
                if ((c = selectCase())) assignInvestigator(*c);
                break;
            case 7:
                if ((c = selectCase())) generateReport(*c);
                break;
            case 8: listCases(); break;
            case 9:
                if ((c = selectCase())) closeCase(*c);
                break;
            case 10: searchAll(); break;
            case 11: registerInvestigator(); break;
            case 12:
                if ((c = selectCase())) {
                    c->reportAllEvidence();
                    std::string id = readLine("  Evidence ID to remove: ");
                    try { c->removeEvidence(id); }
                    catch (const CaseClosedException& err) { std::cout << "  ✖ " << err.what() << "\n"; }
                }
                break;
            case 13: saveCases("cases.txt"); break;
            case 14: loadCases("cases.txt"); break;
            default: std::cout << "  Unknown choice.\n";
        }
    } while (choice != 0);

    std::cout << "  Goodbye. Investigation records kept secure.\n";
    return 0;
}