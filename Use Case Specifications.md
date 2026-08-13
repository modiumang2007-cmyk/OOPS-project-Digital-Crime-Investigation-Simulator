# Use Case Specifications — CaseFile

Three use cases are specified in full below, chosen because together they exercise every relationship type from the use case diagram: **Add Evidence** (contains an `«include»`), **Analyze Evidence** (has an `«extend»` with a guard condition), and **Close Case** (contains an `«include»` and demonstrates a strict precondition/postcondition contract).

---

## UC-1: Add Evidence

| Field | Description |
|---|---|
| **Use Case ID** | UC-1 |
| **Primary Actor** | Cyber Investigator, Field Investigator |
| **Stakeholders and Interests** | **Investigator** — wants evidence logged quickly and without data loss. **Admin** — wants every evidence record traceable to a valid, open case. **File System (secondary actor)** — must receive a well-formed write request. |
| **Preconditions** | 1. The case exists and its status is `OPEN` or `UNDER_INVESTIGATION` (not `CLOSED`). 2. The investigator is assigned to the case. |
| **Trigger** | Investigator selects "Add Evidence" from the active case menu. |
| **Postconditions (success)** | A new `Evidence` object (of the correct subclass) is added to the case's evidence collection, and the updated case is persisted to file via the included **Save Case Data** use case. |
| **Postconditions (failure)** | No partial or malformed evidence record is added; the case's evidence collection is left exactly as it was before the use case began. |

### Main Flow

1. Investigator selects the active case from the case list.
2. System displays the evidence entry form (evidence type, description, collected-by).
3. Investigator selects an evidence type: Digital, Physical, or Testimonial.
4. Investigator enters the evidence description and collector name.
5. System validates the input (non-empty fields, valid type).
6. System instantiates the corresponding `Evidence` subclass and assigns it a unique evidence ID.
7. System appends the new evidence object to the case's evidence collection.
8. System **includes** the **«include» Save Case Data** use case to persist the updated case to file.
9. System confirms the addition and displays the updated evidence list.

### Alternate Flow A1 — Case Is Closed

- **Branch point:** Step 1.
- 1a. Admin/system detects the selected case's status is `CLOSED`.
- 1b. System raises `CaseClosedException` and displays "Cannot add evidence to a closed case."
- 1c. Use case ends; no evidence is added.

### Alternate Flow A2 — Invalid Evidence Data

- **Branch point:** Step 5.
- 5a. Validation fails (empty description, missing collector, or unrecognized type).
- 5b. System raises `InvalidEvidenceException` and displays the specific validation error.
- 5c. Flow returns to Step 3 so the investigator can re-enter the evidence.

### Alternate Flow A3 — Persistence Failure

- **Branch point:** Step 8.
- 8a. The included Save Case Data use case fails (e.g., file write error, disk full).
- 8b. System keeps the evidence in memory, flags the case as having unsaved changes, and displays a warning.
- 8c. System prompts the investigator to retry the save; use case resumes at Step 8 on retry, or ends with the case marked "unsaved" if the investigator declines.

---

## UC-2: Analyze Evidence

| Field | Description |
|---|---|
| **Use Case ID** | UC-2 |
| **Primary Actor** | Cyber Investigator, Field Investigator |
| **Stakeholders and Interests** | **Investigator** — wants an accurate, type-appropriate analysis without manually branching on evidence type. **Admin** — wants critical findings escalated automatically rather than sitting unnoticed in a case file. **Case** *(as a stakeholder concept, not actor)* — its integrity depends on every evidence item eventually being analyzed. |
| **Preconditions** | 1. The evidence item exists and belongs to a case with status `UNDER_INVESTIGATION`. 2. The investigator is assigned to that case. |
| **Trigger** | Investigator selects an evidence item and chooses "Analyze." |
| **Postconditions (success)** | The evidence item's status is updated to `Analyzed`, its analysis result is stored, and — if flagged critical — the case has been escalated via the extension use case. |
| **Postconditions (failure)** | The evidence item's status and stored data remain unchanged from before the use case began. |

### Main Flow

1. Investigator selects an evidence item from the case's evidence list.
2. System determines the evidence item's concrete runtime type (Digital / Physical / Testimonial).
3. System invokes `analyzeEvidence()`, which dispatches polymorphically to the type-specific implementation.
4. System displays the analysis result to the investigator.
5. Investigator reviews the result and marks it as reviewed.
6. System updates the evidence status to `Analyzed` and timestamps the review.

### Extension — «extend» Escalate Case *(guard condition)*

- **Extension point:** after Step 4, before Step 5.
- **Guard condition:** `[evidence marked critical]`
- 4a. During review, the investigator flags the evidence as critical.
- 4b. System **triggers the extending use case, Escalate Case**: it prompts the investigator for confirmation, updates the case's priority flag, and notifies the Admin actor.
- 4c. Flow rejoins the main flow at Step 5.
- *(This extension is optional and only fires when the guard condition holds — it is not part of every execution of Analyze Evidence.)*

### Alternate Flow A1 — Evidence Already Analyzed

- **Branch point:** Step 1.
- 1a. The selected evidence item's status is already `Analyzed`.
- 1b. System asks the investigator whether to re-analyze.
- 1c. If declined, use case ends with no change. If confirmed, flow continues at Step 2.

### Alternate Flow A2 — Unresolvable Evidence Type

- **Branch point:** Step 2.
- 2a. The evidence record's type cannot be resolved (e.g., corrupted data from a failed load).
- 2b. System raises `InvalidEvidenceException` and logs the error.
- 2c. Use case ends; evidence status remains unchanged.

---

## UC-3: Close Case

| Field | Description |
|---|---|
| **Use Case ID** | UC-3 |
| **Primary Actor** | Admin |
| **Stakeholders and Interests** | **Admin** — wants a clean, irreversible closure with no loose ends. **Investigators** — want the case's final state to correctly reflect their work before it becomes read-only. **Suspects/Witnesses** *(interested parties, not system actors)* — the case outcome affects them, so the closure record must be accurate. |
| **Preconditions** | 1. The case exists and its status is `UNDER_INVESTIGATION`. 2. At least one investigator has been assigned to the case at some point. |
| **Trigger** | Admin selects "Close Case" for a specific case. |
| **Postconditions (success)** | Case status is set to `CLOSED`, a closure timestamp is recorded, a final report has been generated and archived via the included use case, and the case becomes read-only (no further evidence, suspects, or status changes accepted). |
| **Postconditions (failure)** | Case status remains `UNDER_INVESTIGATION`; no partial closure state is possible — closure either fully completes or does not happen. |

### Main Flow

1. Admin selects a case with status `UNDER_INVESTIGATION`.
2. System verifies the case has at least one assigned investigator.
3. System **includes** the **«include» Generate Report** use case to produce the case's final summary report.
4. System sets the case status to `CLOSED`.
5. System records the closure timestamp.
6. System archives the generated report alongside the case record.
7. System confirms the closure to the Admin.

### Alternate Flow A1 — Case Already Closed

- **Branch point:** Step 1.
- 1a. The selected case's status is already `CLOSED`.
- 1b. System displays "This case is already closed" and takes no further action.
- 1c. Use case ends.

### Alternate Flow A2 — No Investigator Ever Assigned

- **Branch point:** Step 2.
- 2a. System finds no investigator has ever been assigned to the case.
- 2b. System raises `InvalidInvestigatorException`-style validation, blocks closure, and displays "Assign an investigator before closing this case."
- 2c. Use case ends; case status remains `UNDER_INVESTIGATION`.

### Alternate Flow A3 — Report Generation Fails

- **Branch point:** Step 3.
- 3a. The included Generate Report use case fails (e.g., missing or corrupted evidence data needed for the report).
- 3b. System aborts the closure — status is **not** changed to `CLOSED` — and displays the underlying error to the Admin.
- 3c. Use case ends; Admin may retry after resolving the underlying data issue.
