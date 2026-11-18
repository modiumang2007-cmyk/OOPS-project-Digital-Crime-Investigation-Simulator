# CRC Cards — CaseFile

One **Class–Responsibility–Collaborator** card per class in `src/`. Cards were derived from the code as it stands (see [`noun-analysis.md`](noun-analysis.md) for how the class list was chosen). A blank collaborator cell means the responsibility is handled entirely inside the class.

**Legend:** *(abstract)* = cannot be instantiated · `Superclass` / `Subclasses` lines show the inheritance position.

---

## Domain classes

### Case

**Superclass:** none · **Subclasses:** none

| Responsibilities | Collaborators |
|---|---|
| Hold case identity and lifecycle data: ID, title, status, open date, close date | `CaseStatus` |
| Own the case's evidence (polymorphic, `unique_ptr`) | `Evidence` |
| Hold the case's suspects and witnesses (by value) | `Suspect`, `Witness` |
| Hold a reference to the assigned investigator (`shared_ptr`) | `Investigator` |
| Guard every mutating operation: reject once `CLOSED` (`requireOpen()`) | `CaseClosedException` |
| Validate status transitions (`OPEN → UNDER_INVESTIGATION → CLOSED`); auto-promote to `UNDER_INVESTIGATION` on investigator assignment | |
| Refuse assignment to an investigator who is at maximum caseload | `Investigator`, `InvalidInvestigatorException` |
| Traverse evidence polymorphically: analyze all, verify one, list all | `Evidence` |
| Answer keyword search across its own fields and all children | `Suspect`, `Witness`, `Evidence` |
| Produce the text content of a case report | `Evidence`, `Investigator` |

---

### Evidence *(abstract)*

**Superclass:** none · **Subclasses:** `DigitalEvidence`, `PhysicalEvidence`, `TestimonialEvidence`

| Responsibilities | Collaborators |
|---|---|
| Hold shared evidence data: ID, description, collector, type, critical flag, analyzed flag, analysis result, checksum | `EvidenceType` |
| Reject malformed data at construction (empty ID / description / collector) | `InvalidEvidenceException` |
| Compute the integrity checksum at construction (djb2 over description + collector) | |
| Provide `analyzeEvidence()` as a **Template Method**: call the type-specific hook, then mark itself analyzed | subclasses (via `typeSpecificAnalysis()`) |
| Declare the polymorphic contract: `typeSpecificAnalysis()`, `verifyIntegrity()`, `getReportContent()` | `DigitalEvidence`, `PhysicalEvidence`, `TestimonialEvidence` |
| Match a keyword against its ID and description (case-insensitive) | |
| Expose validated getters/setters (`setCritical`, `setDescription`) | |

---

### DigitalEvidence

**Superclass:** `Evidence`

| Responsibilities | Collaborators |
|---|---|
| Store a metadata snapshot (e.g. file hash, image path) | |
| Implement `typeSpecificAnalysis()` — scan the metadata snapshot, cross-reference artifacts | `Evidence` |
| Implement `verifyIntegrity()` — recompute checksum; on mismatch report "data may have been tampered" | `Evidence` |
| Implement `getReportContent()` with metadata and analysis result | |

---

### PhysicalEvidence

**Superclass:** `Evidence`

| Responsibilities | Collaborators |
|---|---|
| Store lab results summary | |
| Implement `typeSpecificAnalysis()` — run the lab-test protocol and report results | `Evidence` |
| Implement `verifyIntegrity()` — on mismatch report "chain-of-custody broken" | `Evidence` |
| Implement `getReportContent()` with lab results and analysis result | |

---

### TestimonialEvidence

**Superclass:** `Evidence`

| Responsibilities | Collaborators |
|---|---|
| Store witness name and credibility score (0.0–10.0) | |
| Implement `typeSpecificAnalysis()` — evaluate credibility; flag scores below 3.0 for review | `Evidence` |
| Implement `verifyIntegrity()` — on mismatch report "statement modified" | `Evidence` |
| Implement `getReportContent()` with witness, credibility and analysis result | |

---

### Investigator *(abstract)*

**Superclass:** none · **Subclasses:** `CyberInvestigator`, `FieldInvestigator`

| Responsibilities | Collaborators |
|---|---|
| Hold investigator ID, name, specialization type and active caseload | `InvestigatorType` |
| Reject empty ID/name at construction (`std::invalid_argument`) | |
| Enforce the caseload ceiling (`MAX_CASELOAD_ = 5`) via `canAcceptCase()` | |
| Track caseload (`incrementCaseload()`, `decrementCaseload()`) | `Case` (called when assigned) |
| Declare `investigate()` as the specialization-specific procedure | `CyberInvestigator`, `FieldInvestigator` |
| Restrict construction to subclasses (protected constructor) | |

---

### CyberInvestigator

**Superclass:** `Investigator`

| Responsibilities | Collaborators |
|---|---|
| Implement `investigate()` as a digital-forensics workflow (drive imaging, network logs, IP cross-referencing, decryption) | `Investigator` |

---

### FieldInvestigator

**Superclass:** `Investigator`

| Responsibilities | Collaborators |
|---|---|
| Implement `investigate()` as an on-site workflow (secure scene, document evidence, take statements, coordinate with lab) | `Investigator` |

---

### Suspect

**Superclass:** none (flat record)

| Responsibilities | Collaborators |
|---|---|
| Hold suspect ID, name and personal details behind private fields with getters | |
| Match a keyword against ID, name and details (case-sensitive) | |

---

### Witness

**Superclass:** none (flat record)

| Responsibilities | Collaborators |
|---|---|
| Hold witness ID, name and statement summary behind private fields with getters | |
| Match a keyword against ID, name and statement (case-sensitive) | |

---

### Report

**Superclass:** none

| Responsibilities | Collaborators |
|---|---|
| Snapshot a case's report text at construction time (separation of concerns: formatting lives in `Case`, packaging lives here) | `Case` |
| Hold report ID, generation timestamp and content | |
| Serialize to / deserialize from a pipe-delimited line | |
| Stream itself to `std::ostream` via `operator<<` | |

---

## Exception classes

### CaseClosedException

| Responsibilities | Collaborators |
|---|---|
| Signal an operation attempted on a `CLOSED` case; carry the case ID in `what()` | `Case` (thrown by) |

### InvalidEvidenceException

| Responsibilities | Collaborators |
|---|---|
| Signal evidence that failed construction-time validation | `Evidence` (thrown by) |

### InvalidInvestigatorException

| Responsibilities | Collaborators |
|---|---|
| Signal an investigator that cannot take a case (caseload full) | `Case` (thrown by), `Investigator` |

---

## Application layer (not a class)

### Console App — `main.cpp` *(free functions + globals)*

| Responsibilities | Collaborators |
|---|---|
| Own the application state: `map<string, Case> g_cases`, `vector<shared_ptr<Investigator>> g_investigators` | `Case`, `Investigator` |
| Present the 15-option menu and route each choice to a handler | |
| Create concrete `Evidence` / `Investigator` subclasses from user input (the only place the concrete types are named) | `DigitalEvidence`, `PhysicalEvidence`, `TestimonialEvidence`, `CyberInvestigator`, `FieldInvestigator` |
| Catch domain exceptions and print them to the user | `CaseClosedException`, `InvalidInvestigatorException` |
| Enforce "investigator required" before closing a case | `Case` |
| Generate IDs (`CASE-n`, `EVID-n`, `SUS-n`, `WIT-n`, `INV-n`, `RPT-n`) | |
| Save and load cases to/from `cases.txt`, rebuilding the right `Evidence` subclass from a type tag | `Case`, `Evidence` |
| Build and print a `Report`; trigger the investigator's `investigate()` procedure when applicable | `Report`, `Investigator` |

---

## Enumerations (not classes)

| Enum | Values | Used by |
|---|---|---|
| `CaseStatus` | `OPEN`, `UNDER_INVESTIGATION`, `CLOSED` | `Case` |
| `EvidenceType` | `DIGITAL`, `PHYSICAL`, `TESTIMONIAL` | `Evidence`, save/load tag |
| `InvestigatorType` | `CYBER`, `FIELD` | `Investigator` |
