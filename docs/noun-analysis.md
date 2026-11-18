# Noun–Verb Analysis — CaseFile

**Source material:** `problem-statement.md` (Background, Problem, Proposed Solution, Objectives, Scope) and the console menu in `src/main.cpp` (options 0–14), which is the system's real requirements surface.

**Filters used** (Abbott/Booch four-filter model): **Redundant** · **Vague** · **Attribute** · **Operation**.

---

## 1. Raw candidate nouns (51)

Investigative Agency, Investigation, Case, Case Lifecycle, Case State, State Transition, Workflow Rule, Suspect, Witness, Investigator, Cyber Investigator, Field Investigator, Investigator Specialization, Caseload, Roster, Evidence, Evidence Item, Evidence Type, Digital Evidence, Physical Evidence, Testimonial Evidence, Evidence Integrity, Checksum, Analysis Procedure, Analysis Result, Report, Keyword Search, Keyword, Custom Exception, CaseClosedException, InvalidEvidenceException, InvalidInvestigatorException, Exception, System, Relationship, Class Hierarchy, Case Data, Save File, Console Interface, Menu, Admin, File System, Case ID, Title, Open Date, Description, Collector, Critical Flag, Metadata Snapshot, Lab Results, Credibility Score

---

## 2. Surviving classes (14)

| Class | Why it survives |
|---|---|
| `Case` | Central aggregate with identity, lifecycle and behavior (guarded mutation, polymorphic traversal) |
| `Evidence` | Abstract base — shared data plus the polymorphic contract |
| `DigitalEvidence` | Concrete type with its own analysis logic and state (metadata snapshot) |
| `PhysicalEvidence` | Concrete type with its own analysis logic and state (lab results) |
| `TestimonialEvidence` | Concrete type with its own analysis logic and state (credibility score, witness) |
| `Investigator` | Abstract base — identity, caseload rule, `investigate()` contract |
| `CyberInvestigator` | Concrete specialization with its own procedure |
| `FieldInvestigator` | Concrete specialization with its own procedure |
| `Suspect` | Has identity, searchable data, and belongs to a case |
| `Witness` | Has identity, searchable data, and belongs to a case |
| `Report` | Has its own ID, timestamp, content and serialization behavior |
| `CaseClosedException` | Distinct failure type, caught specifically |
| `InvalidEvidenceException` | Distinct failure type, caught specifically |
| `InvalidInvestigatorException` | Distinct failure type, caught specifically |

---

## 3. Discarded candidates (37)

| Candidate | Filter | Reasoning / where it ended up |
|---|---|---|
| Investigative Agency | Vague | The real-world organization — outside the system boundary |
| Investigation | Redundant | Same concept as `Case` |
| Case Lifecycle | Attribute | Captured by `Case::status_` |
| Case State | Attribute | Realised as `enum class CaseStatus` (`OPEN`, `UNDER_INVESTIGATION`, `CLOSED`) |
| State Transition | Operation | `Case::changeStatus()` / `closeCase()` |
| Workflow Rule | Operation | `Case::requireOpen()` guard and the transition checks |
| Investigator Specialization | Attribute | Realised by subclass choice, tagged by `enum class InvestigatorType` |
| Caseload | Attribute | `Investigator::activeCaseload_` with `canAcceptCase()` |
| Roster | Attribute | `g_investigators` vector in `main.cpp` |
| Evidence Item | Redundant | Same concept as `Evidence` |
| Evidence Type | Attribute | Realised by subclass choice, tagged by `enum class EvidenceType` |
| Evidence Integrity | Operation | `verifyIntegrity()` |
| Checksum | Attribute | `Evidence::checksum_`, computed by `computeChecksum()` |
| Analysis Procedure | Operation | `analyzeEvidence()` → `typeSpecificAnalysis()` |
| Analysis Result | Attribute | `Evidence::analysisResult_` |
| Keyword Search | Operation | `search()` on `Case`, `Evidence`, `Suspect`, `Witness` |
| Keyword | Vague | Just a method parameter |
| Exception | Redundant | Base is the external `std::exception` |
| Custom Exception | Redundant | Collective name for the three surviving exception classes |
| System | Vague | Too generic — the whole application |
| Relationship | Vague | Modeled as associations in the diagram, not as objects |
| Class Hierarchy | Vague | Design vocabulary, not a domain noun |
| Case Data | Redundant | Same concept as `Case` |
| Save File | Operation | `saveCases()` / `loadCases()`; `cases.txt` is an external artifact |
| File System | Vague | External actor in the use case diagram, not a domain class |
| Console Interface | Vague | UI boundary implemented as free functions in `main.cpp` |
| Menu | Vague | `showMenu()` in `main.cpp` |
| Admin | Vague | Not implemented as a role: the app has a single console user, so there is no `Admin` class |
| Case ID | Attribute | `Case::caseId_` |
| Title | Attribute | `Case::title_` |
| Open Date | Attribute | `Case::openDate_` |
| Description | Attribute | `Evidence::description_` |
| Collector | Attribute | `Evidence::collectedBy_` |
| Critical Flag | Attribute | `Evidence::isCritical_` |
| Metadata Snapshot | Attribute | `DigitalEvidence::metadataSnapshot_` |
| Lab Results | Attribute | `PhysicalEvidence::labResults_` |
| Credibility Score | Attribute | `TestimonialEvidence::credibilityScore_` |

**Count check:** 14 survivors + 37 discarded = 51 raw candidates. ✓

---

## 4. Verb analysis — behaviors mapped to methods

| Verb phrase (from problem statement / menu) | Owner | Method |
|---|---|---|
| open / create a case | `main.cpp` → `Case` ctor | `createCase()` |
| add a suspect / witness | `Case` | `addSuspect()`, `addWitness()` |
| add / remove evidence | `Case` | `addEvidence()`, `removeEvidence()` |
| analyze evidence | `Evidence` (+ `Case` loop) | `analyzeEvidence()`, `analyzeAllEvidence()` |
| verify evidence integrity | `Evidence` (+ `Case`) | `verifyIntegrity()`, `verifyEvidence()` |
| assign an investigator | `Case` | `assignInvestigator()` |
| investigate | `Investigator` subclasses | `investigate()` |
| accept a case (caseload) | `Investigator` | `canAcceptCase()`, `incrementCaseload()` |
| move case through its lifecycle | `Case` | `changeStatus()`, `closeCase()` |
| reject modification of a closed case | `Case` | `requireOpen()` |
| search | `Case`, `Evidence`, `Suspect`, `Witness` | `search()` |
| generate a report | `Case`, `Report` | `getReportContent()`, `Report` ctor |
| save / resume | `main.cpp` | `saveCases()`, `loadCases()` |

---

## 5. Design decisions recorded

1. **No `Person` base class.** `Suspect` and `Witness` share an ID + name shape but no behavior; inheritance would add nothing, so they are two small concrete classes.
2. **No `Admin` class.** The system has one console user, so a role class would be data nobody uses. Earlier drafts of the diagrams modeled `Admin`; it is deliberately absent from the implementation.
3. **Enums, not classes, for state and type tags.** `CaseStatus`, `EvidenceType` and `InvestigatorType` are `enum class`; behavior varies through subclasses, not through the tag.
4. **`Report` is separate from `Case`.** `Case` produces the text; `Report` packages it with an ID and timestamp, keeping formatting and serialization out of the aggregate.
