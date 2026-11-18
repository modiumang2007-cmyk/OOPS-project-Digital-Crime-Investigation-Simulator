# Product Requirements Document — CaseFile

**Product:** CaseFile — Digital Crime Investigation Simulator
**Version:** 1.0 (course submission build)
**Course:** Object-Oriented Programming, Semester III · IIIT Vadodara
**Language / build:** C++17, STL only, CMake or plain `g++`
**Status legend:** ✅ implemented · ⚠️ partial · 🔲 not implemented

---

## 1. Overview

CaseFile is a console-based case management application that models how an investigative agency tracks cases, suspects, witnesses, evidence and investigators. It enforces a case lifecycle (`OPEN → UNDER_INVESTIGATION → CLOSED`) and treats different evidence types and investigator specializations as polymorphic classes instead of branching on type tags.

It doubles as a teaching artifact: every design decision (why a class is abstract, why a destructor is virtual, why a guard lives inside `Case`) is meant to be defensible in a viva.

## 2. Problem statement

Investigations involve many linked entities, each with different behavior and strict workflow rules (e.g. a closed case must not change). Tracking this by hand, or in one large procedural program with conditionals per evidence type, is error-prone and does not scale when new evidence or investigator types appear. There is no lightweight, extensible model that enforces the rules automatically.

## 3. Goals and non-goals

### Goals
1. Model the investigation domain with inheritance, polymorphism, abstraction and encapsulation.
2. Enforce lifecycle rules inside the domain classes, not by UI convention.
3. Make adding a new evidence type or investigator specialization a matter of adding a class.
4. Persist cases to a file so work can be saved and resumed.
5. Report failures through typed custom exceptions rather than silent failure.

### Non-goals (v1)
GUI · database backend · multi-user login / roles · network access · cryptographic integrity guarantees.

## 4. Users

| Persona | Description | Needs |
|---|---|---|
| **Investigator / case officer** | Single console user who runs the whole workflow | Create cases, log evidence, run analysis, assign investigators, report, close |
| **Course evaluator** | Reads the code and diagrams, asks about design choices | Clear OOP structure, traceable design rationale, runnable demo |
| **Future contributor** | Extends the system | Obvious extension points (new evidence/investigator subclass) |

## 5. Scope

**In scope:** case lifecycle, three evidence types with type-specific analysis, two investigator types with specialization-specific procedures, investigator assignment with a caseload limit, suspect and witness records, keyword search, report generation, file save/load, console UI.

**Out of scope:** see non-goals.

## 6. Functional requirements

### 6.1 Case management
| ID | Requirement | Status |
|---|---|---|
| FR-1 | Create a case with a non-empty title; assign a unique ID (`CASE-n`) and open date; initial status `OPEN` | ✅ (date is a fixed string — see §11) |
| FR-2 | List all cases with status, evidence count and suspect count | ✅ |
| FR-3 | Auto-promote a case to `UNDER_INVESTIGATION` when an investigator is assigned | ✅ |
| FR-4 | Allow only the transitions `OPEN → UNDER_INVESTIGATION → CLOSED`; reject others | ✅ |
| FR-5 | Close a case only if an investigator is assigned | ✅ (enforced in the menu layer) |
| FR-6 | Reject every mutating operation on a `CLOSED` case with `CaseClosedException` | ✅ (add suspect/witness/evidence, remove evidence, assign, status change) |

### 6.2 People
| ID | Requirement | Status |
|---|---|---|
| FR-7 | Add suspects (ID, name, personal details) to a case | ✅ |
| FR-8 | Add witnesses (ID, name, statement summary) to a case | ✅ |
| FR-9 | Register investigators of type Cyber or Field (roster capped at 12) | ✅ |
| FR-10 | Assign one investigator to a case; reject if the investigator already has 5 active cases | ✅ |

### 6.3 Evidence
| ID | Requirement | Status |
|---|---|---|
| FR-11 | Add Digital, Physical or Testimonial evidence with ID, description, collector and type-specific data | ✅ |
| FR-12 | Reject evidence with empty ID, description or collector (`InvalidEvidenceException`) | ✅ |
| FR-13 | Analyze all evidence in a case; each type runs its own analysis via virtual dispatch | ✅ |
| FR-14 | Flag testimonial evidence with credibility below 3.0 for review | ✅ |
| FR-15 | Verify the integrity of one evidence item by recomputing its checksum; report type-specific failure text | ✅ |
| FR-16 | Remove an evidence item from an open case | ✅ |
| FR-17 | Mark evidence as critical | ⚠️ flag exists and is saved/loaded; no menu action sets it and nothing acts on it |

### 6.4 Search and reporting
| ID | Requirement | Status |
|---|---|---|
| FR-18 | Keyword search across case ID/title, suspects, witnesses and evidence; list matching cases | ✅ |
| FR-19 | Generate a case report (case data, investigator, suspects, witnesses, evidence) | ✅ |
| FR-20 | When a report is requested for a case with an investigator and evidence, run the investigator's specialization procedure first | ✅ |
| FR-21 | Persist generated reports | 🔲 `Report::serialize()` exists; nothing writes it |

### 6.5 Persistence
| ID | Requirement | Status |
|---|---|---|
| FR-22 | Save all cases to `cases.txt` on demand | ✅ |
| FR-23 | Load `cases.txt` on startup and on demand, rebuilding the correct evidence subclass from a type tag | ⚠️ works for open cases; see KI-1 |
| FR-24 | Persist the investigator roster and assignments | 🔲 |

## 7. Business rules

1. A `CLOSED` case is read-only.
2. A case cannot close without an assigned investigator.
3. An investigator may hold at most **5** active cases.
4. Evidence must have an ID, description and collector.
5. Credibility scores are clamped to 0.0–10.0 on entry.
6. Evidence integrity is checked by comparing a stored djb2 checksum (over description + collector) with a recomputed one.

## 8. Non-functional requirements

| Area | Requirement |
|---|---|
| Portability | Standard C++17; builds on Linux, macOS and Windows (UTF-8 console set on Windows) |
| Dependencies | STL only |
| Build quality | Compiles cleanly with `-Wall -Wextra -Wpedantic` |
| Memory safety | Ownership via `unique_ptr` (evidence) and `shared_ptr` (investigators); virtual base destructors |
| Extensibility | A new evidence or investigator type must not require edits to `Case`'s traversal logic |
| Error handling | Domain failures use typed exceptions derived from `std::exception` |
| Usability | Numbered menu with prompts; invalid selections produce a message (known exception: KI-3) |

## 9. Architecture summary

| Layer | Files | Responsibility |
|---|---|---|
| Domain | `Case`, `Evidence` (+3), `Investigator` (+2), `Suspect`, `Witness`, `Report`, `Exceptions.h` | Rules, state, polymorphic behavior |
| Application | `main.cpp` | Menu, ID generation, containers, persistence |
| Storage | `cases.txt` | Pipe-delimited text file |

**Key patterns:** Template Method (`Evidence::analyzeEvidence` → `typeSpecificAnalysis`), polymorphic ownership (`vector<unique_ptr<Evidence>>`), guard method (`requireOpen`), operator overloading (`Case::operator==`, `operator+=`, `operator<<` for `Report`).

See `docs/class-diagram.png`, `docs/object-diagram.png`, `docs/use-case-diagram.png`, `docs/crc-cards.md`, `docs/noun-analysis.md`.

### Save-file format (`cases.txt`)
```
<case count>
<caseId>|<title>|<status 0-2>|<openDate>|<closeDate>
<suspectCount> <witnessCount> <evidenceCount>
<suspect lines: id|name|details> …
<witness lines: id|name|statement> …
<evidence header: type 0-2|id|description|collectedBy|critical 0/1|checksum>
<evidence detail: metadata | labResults | witnessName|score>
```

## 10. Acceptance criteria

| # | Scenario | Expected result |
|---|---|---|
| AC-1 | Create a case, add Digital evidence, register a Field investigator, assign, analyze | Status becomes `UNDER_INVESTIGATION`; digital analysis output is printed |
| AC-2 | Add evidence to a closed case | `CaseClosedException` message shown; evidence count unchanged |
| AC-3 | Assign an investigator already at 5 cases | `InvalidInvestigatorException` message shown; case not promoted |
| AC-4 | Close a case with no investigator | Refused with a prompt to assign one |
| AC-5 | Add evidence with empty description | Rejected; nothing added |
| AC-6 | Analyze a case holding all three evidence types | Each item prints its own type-specific analysis in one loop |
| AC-7 | Save, restart, load an **open** case | Same title, status, suspects, witnesses and evidence |
| AC-8 | Search a keyword present only in a witness statement | The owning case is listed |

The scripted run in `demo-input.txt` exercises AC-1, AC-6 (digital only) and a close.

## 11. Known issues and gaps

Found by compiling and running the current code.

| ID | Issue | Impact | Suggested fix |
|---|---|---|---|
| KI-1 | `loadCases` sets a case to `CLOSED` **before** re-adding its suspects/witnesses/evidence, so `requireOpen()` throws. Reproduced: close a case that has evidence, save, restart → `terminate called after throwing 'CaseClosedException'` | Any saved closed case with children crashes startup | Load children first, then apply status; or add a private restore path |
| KI-2 | Evidence ID counter starts at 1000 and person serials at 1 on every launch | After loading, new items reuse IDs (reproduced: two `EVID-1000` in one case) | Seed counters from loaded data or persist them |
| KI-3 | Registering an investigator with an empty name throws `std::invalid_argument`, which `main` does not catch | Program aborts | Validate in `registerInvestigator` or catch the exception |
| KI-4 | Investigators and assignments are not saved | After reload, cases show no investigator | Persist the roster and a case→investigator ID |
| KI-5 | Analysis results and `analyzed_` flag are not saved; the stored checksum is read but ignored (recomputed from loaded text) | Tampering with `cases.txt` is not detectable | Restore the saved checksum into `Evidence` |
| KI-6 | `|` inside free text corrupts the save file; no escaping | Data loss on reload | Escape delimiter or use a structured format |
| KI-7 | Caseload is incremented in `main.cpp`, never decremented on close or reassignment | Caseload only grows; reassigning leaks the old investigator's count | Move caseload accounting into `Case::assignInvestigator`/`closeCase` |
| KI-8 | `now()` returns a fixed date; close date is the literal `"now"` | Dates are not real | Use `<chrono>` |
| KI-9 | `Suspect`/`Witness` search is case-sensitive; `Evidence` search is not | Inconsistent results | Share one case-insensitive helper |
| KI-10 | `TestimonialEvidence` stores a witness *name* string, not a link to a `Witness` | No referential integrity | Store a witness ID |

## 12. Success metrics

- All eight acceptance criteria pass.
- Adding a fourth evidence type changes no line of `Case::analyzeAllEvidence`.
- Zero compiler warnings with `-Wall -Wextra -Wpedantic` (met in the current build).
- Each pillar — abstraction, inheritance, polymorphism, encapsulation, exception handling, STL — can be pointed to in a specific file and function.

## 13. Risks and assumptions

| Risk / assumption | Mitigation |
|---|---|
| Plain-text persistence is fragile (KI-1, KI-6) | Treat KI-1 as the first fix; consider a versioned format |
| Single-user assumption | Roles are out of scope; revisit only if multi-user is added |
| Hardcoded dates make the demo non-reproducible for real use | Replace `now()` |

## 14. Roadmap (post-v1)

1. Fix KI-1, KI-2, KI-3 (stability).
2. Persist investigators, assignments and reports.
3. Mark-critical action and escalation of critical findings.
4. Real timestamps and an audit trail of status changes.
5. Unit tests for the guard, caseload and polymorphic dispatch.
6. Additional evidence types (e.g. financial, biometric) to prove the extension story.

## 15. Team

| Name | Role |
|---|---|
| Umang Modi | Team leader |
| Riya Vasoya | Team member |
| Vedant Songire | Team member |
| Pratyush Pandit | Team member |
| Kunal Patni | Team member |
