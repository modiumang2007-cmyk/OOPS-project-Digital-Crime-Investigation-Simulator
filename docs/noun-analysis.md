# Noun–Verb Analysis — CaseFile

Source material: `problem-statement.md`, `docs/use-cases.md` (UC-1 Add Evidence, UC-2 Analyze Evidence, UC-3 Close Case).

Filter set used: **Redundant, Vague, Attribute, Operation** (standard Abbott/Booch four-filter model). If your course uses differently labeled filters, the classifications below still hold — only the filter *name* changes, not the reasoning.

## 1. Raw Candidate List

All nouns and noun phrases extracted from the three use case specifications, in order of first appearance (44 total):

Case, Evidence, Evidence Type, Digital Evidence, Physical Evidence, Testimonial Evidence, Investigator, Cyber Investigator, Field Investigator, Admin, File System, Case Status, Evidence Entry Form, Description, Collected-By, Evidence ID, Evidence Collection, Save Case Data, CaseClosedException, InvalidEvidenceException, Evidence List, Analysis Result, Review, Evidence Status, Timestamp, Escalate Case, Critical Flag, Guard Condition, Notification, Investigator Assignment, Generate Report, Case Record, Closure Timestamp, Report, InvalidInvestigatorException, Persistence Failure, Unsaved Changes, Suspect, Witness, Stakeholder, Trigger, Precondition, Postcondition, System

## 2. Surviving Classes (15)

| Class | Notes |
|---|---|
| `Case` | Core aggregate; owns evidence, suspects, investigators |
| `Evidence` | Abstract base class |
| `DigitalEvidence` | Concrete evidence subtype |
| `PhysicalEvidence` | Concrete evidence subtype |
| `TestimonialEvidence` | Concrete evidence subtype |
| `Investigator` | Abstract base class |
| `CyberInvestigator` | Concrete investigator subtype |
| `FieldInvestigator` | Concrete investigator subtype |
| `Admin` | Surfaced by this analysis as a domain class candidate, not just a use-case actor — open design decision, see Section 4 |
| `Suspect` | `Person` subtype |
| `Witness` | `Person` subtype |
| `Report` | Generated/archived object with its own content and date |
| `CaseClosedException` | Custom exception, modeled as a class |
| `InvalidEvidenceException` | Custom exception, modeled as a class |
| `InvalidInvestigatorException` | Custom exception, modeled as a class |

## 3. Discarded Candidates and Filter Applied

| Discarded Candidate | Filter | Reasoning |
|---|---|---|
| Evidence Type | Attribute | Captured by which `Evidence` subclass is instantiated, not a separate class |
| Case Status | Attribute | Enumerated value on `Case` (`OPEN` / `UNDER_INVESTIGATION` / `CLOSED`) |
| Description | Attribute | Property of `Evidence` |
| Collected-By | Attribute | Property of `Evidence` |
| Evidence ID | Attribute | Property of `Evidence` |
| Evidence Collection | Attribute | Represents the "Case has-many Evidence" association, not an object with its own behavior |
| Unsaved Changes | Attribute | Boolean/status flag on `Case` |
| Analysis Result | Attribute | Property of `Evidence`, set by `analyzeEvidence()` |
| Evidence Status | Attribute | Property of `Evidence` (e.g. "Analyzed") |
| Timestamp | Attribute | Property attached to review/closure events |
| Critical Flag | Attribute | Property of `Evidence`/`Case`, used as the `«extend»` guard condition |
| Closure Timestamp | Attribute | Property of `Case` |
| Save Case Data | Operation | A behavior the system performs (persistence), not a thing |
| Review | Operation | An action the investigator performs, not an object |
| Escalate Case | Operation | Behavior triggered on `Case`, not a standalone persistent object |
| Notification | Operation | Spec describes "notifies Admin" as an action, not a modeled object |
| Generate Report | Operation | The *act* of producing a report — the resulting object, `Report`, survives separately |
| Investigator Assignment | Operation | Describes the act of assigning, not a class with its own identity |
| Evidence List | Redundant | Same concept as `Evidence Collection`, different wording |
| Case Record | Redundant | Same concept as `Case`, different wording |
| File System | Redundant | Already represented as an external actor in the use case model; re-adding it as a domain class duplicates that representation |
| Evidence Entry Form | Vague | UI/interaction construct, not a bounded domain concept |
| Persistence Failure | Vague | Described narratively as a failure scenario, not a well-defined type |
| Guard Condition | Vague | UML modeling terminology, not a domain noun |
| Stakeholder | Vague | Use-case template meta-term |
| Trigger | Vague | Use-case template meta-term |
| Precondition | Vague | Use-case template meta-term |
| Postcondition | Vague | Use-case template meta-term |
| System | Vague | Too generic — refers to the whole system, not a bounded class |

**Count check:** 15 survivors + 29 discarded = 44 raw candidates. ✓

## 4. Open Design Question — Admin

The PRD and use case diagram model `Admin` only as a use-case actor. This noun analysis surfaces `Admin` as a legitimate domain class candidate as well (a `Person` who performs administrative actions on a `Case`). Before finalizing the class diagram, decide deliberately:

- **Option A:** `Admin` stays actor-only — no corresponding domain class, since the system doesn't need to persist "admin" as data.
- **Option B:** `Admin` becomes a `Person` subtype alongside `Investigator`, `Suspect`, and `Witness`, if you want role-based data (e.g. tracking which admin closed which case).

Either is defensible; the PRD currently assumes Option A. Pick one explicitly and note the reasoning in your class diagram documentation so it doesn't look like an oversight in viva.
