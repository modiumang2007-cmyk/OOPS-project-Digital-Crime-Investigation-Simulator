# CRC Cards — CaseFile

One CRC (Class-Responsibility-Collaborator) card per class surviving the noun–verb analysis (`docs/noun-analysis.md`). Each row pairs one responsibility with the collaborator(s) it depends on, where applicable — a blank collaborator cell means that responsibility is handled internally with no dependency on another class.

---

### Case

| Responsibilities | Collaborators |
|---|---|
| Track case ID, title, status, and open date | |
| Maintain the collections of associated Suspects, Evidence, and Investigators | `Suspect`, `Evidence`, `Investigator` |
| Enforce valid status transitions (`OPEN` → `UNDER_INVESTIGATION` → `CLOSED`) | |
| Reject any modification once status is `CLOSED` | `CaseClosedException` |
| Coordinate closure — trigger final report generation and persist the closed state | `Report`, `Admin` |

---

### Evidence *(abstract)*

| Responsibilities | Collaborators |
|---|---|
| Store shared evidence data: ID, description, collector name | `Case` (belongs to) |
| Declare `analyzeEvidence()` as the type-specific hook implemented by subclasses | `DigitalEvidence`, `PhysicalEvidence`, `TestimonialEvidence` |
| Implement keyword matching for search (`Searchable`) | |
| Contribute evidence-level content to case reports (`Reportable`) | `Report` |
| Reject malformed data at construction | `InvalidEvidenceException` |

---

### DigitalEvidence

| Responsibilities | Collaborators |
|---|---|
| Implement `analyzeEvidence()` with digital-specific analysis (metadata scan) | `Evidence` (inherits from) |
| Implement `verifyIntegrity()` for its evidence type | |
| Belong to a case's evidence collection | `Case` |

---

### PhysicalEvidence

| Responsibilities | Collaborators |
|---|---|
| Implement `analyzeEvidence()` with physical/lab-test-specific analysis | `Evidence` (inherits from) |
| Belong to a case's evidence collection | `Case` |

---

### TestimonialEvidence

| Responsibilities | Collaborators |
|---|---|
| Implement `analyzeEvidence()` with credibility-scoring logic | `Evidence` (inherits from) |
| Track the witness who provided the statement | `Witness` |
| Belong to a case's evidence collection | `Case` |

---

### Investigator *(abstract)*

| Responsibilities | Collaborators |
|---|---|
| Store investigator ID, name, and current active caseload | |
| Declare `investigate()` as the type-specific hook implemented by subclasses | `CyberInvestigator`, `FieldInvestigator` |
| Enforce maximum caseload before accepting a new case assignment | `InvalidInvestigatorException` |
| Be assignable to and track a case | `Case` |

---

### CyberInvestigator

| Responsibilities | Collaborators |
|---|---|
| Implement `investigate()` with digital forensics procedure steps | `Investigator` (inherits from) |
| Drive analysis of digital evidence items | `DigitalEvidence` |
| Work within an assigned case | `Case` |

---

### FieldInvestigator

| Responsibilities | Collaborators |
|---|---|
| Implement `investigate()` with on-site investigation procedure steps | `Investigator` (inherits from) |
| Drive analysis of physical and testimonial evidence items | `PhysicalEvidence`, `TestimonialEvidence` |
| Work within an assigned case | `Case` |

---

### Admin

*(Domain-class status still open — see `docs/noun-analysis.md` Section 4. Card written assuming Option B, where Admin is modeled as data.)*

| Responsibilities | Collaborators |
|---|---|
| Create new cases | `Case` |
| Assign investigators to cases | `Investigator`, `Case` |
| Initiate case closure | `Case`, `Report` |

---

### Suspect

| Responsibilities | Collaborators |
|---|---|
| Store personal details relevant to the case | `Case` (belongs to) |
| Support keyword search (`Searchable`) | |

---

### Witness

| Responsibilities | Collaborators |
|---|---|
| Store personal details and statement metadata | `Case` (belongs to) |
| Be linked to the testimonial evidence they provided | `TestimonialEvidence` |

---

### Report

| Responsibilities | Collaborators |
|---|---|
| Aggregate a case's summary content (evidence, investigators, outcome) | `Case`, `Evidence`, `Investigator` |
| Record its own generation timestamp | |
| Be archived alongside the closed case | `Case` |

---

### CaseClosedException

| Responsibilities | Collaborators |
|---|---|
| Signal that an operation was attempted on a closed case | `Case` (thrown by) |
| Carry a descriptive error message for the caller | |

---

### InvalidEvidenceException

| Responsibilities | Collaborators |
|---|---|
| Signal that evidence data failed validation at creation | `Evidence` (thrown during construction) |
| Carry a descriptive validation error message | |

---

### InvalidInvestigatorException

| Responsibilities | Collaborators |
|---|---|
| Signal an invalid investigator reference or a caseload limit violation | `Investigator` (thrown by), `Case` (thrown during assignment) |
| Carry a descriptive error message | |
